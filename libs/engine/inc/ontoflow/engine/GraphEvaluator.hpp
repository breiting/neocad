#pragma once

#include <ontoflow/domain/Registry.hpp>
#include <ontoflow/domain/Types.hpp>

namespace of::engine {

/**
 * @brief Recursively evaluates the Dataflow Graph.
 */
class GraphEvaluator {
   public:
    GraphEvaluator(domain::Registry& registry);

    /**
     * @brief Pull-based evaluation of a specific node.
     * Recursively solves dependencies, then executes the node's logic.
     * @param nodeID The node to solve.
     */
    void Evaluate(domain::EntityID nodeID);

   private:
    domain::Registry& m_Registry;
};

}  // namespace of::engine
