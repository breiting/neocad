#include <ontoflow/core/Logger.hpp>
#include <ontoflow/domain/Components.hpp>
#include <ontoflow/engine/GraphEvaluator.hpp>
#include <ontoflow/engine/NodeRegistry.hpp>

namespace of::engine {

GraphEvaluator::GraphEvaluator(domain::Registry& registry) : m_Registry(registry) {
}

void GraphEvaluator::Evaluate(domain::EntityID nodeID) {
    if (!m_Registry.HasComponent<domain::NodeComponent>(nodeID)) {
        return;
    }

    auto* node = m_Registry.GetComponent<domain::NodeComponent>(nodeID);

    // 1. Recursively Evaluate Inputs
    for (auto& inputPin : node->inputs) {
        if (inputPin.connection.targetNodeID != domain::INVALID_ENTITY_ID) {
            // Recursion: Ensure dependency is up-to-date
            Evaluate(inputPin.connection.targetNodeID);

            // Data Transfer (Pull)
            auto* sourceNode = m_Registry.GetComponent<domain::NodeComponent>(inputPin.connection.targetNodeID);
            if (sourceNode && inputPin.connection.targetPinIdx < sourceNode->outputs.size()) {
                inputPin.value = sourceNode->outputs[inputPin.connection.targetPinIdx].value;
            } else {
                LOG(Warn) << "GraphEvaluator: Invalid connection on node " << nodeID;
            }
        }
    }

    // 2. Compute if Dirty (or always for now to keep it simple)
    // In a real system we check isDirty. For this task, we just compute.
    if (node->isDirty) {
        const auto* def = NodeRegistry::Instance().GetDefinition(node->operationID);
        if (def && def->compute) {
            def->compute(*node, m_Registry);
        }
        node->isDirty = false;
    }
}

}  // namespace of::engine
