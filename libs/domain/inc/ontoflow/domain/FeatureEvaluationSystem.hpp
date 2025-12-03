#pragma once
#include <initializer_list>
#include <ontoflow/domain/IGeometryBackend.hpp>
#include <ontoflow/domain/Registry.hpp>

namespace of::domain {

class FeatureEvaluationSystem final {
   public:
    FeatureEvaluationSystem(Registry& registry, IGeometryBackend& backend);
    void EvaluateFeatures();

   private:
    Registry& m_registry;
    IGeometryBackend& m_backend;

    void EvaluateBoxFeature(EntityID boxEntityId);
    void EvaluateCylinderFeature(EntityID cylinderEntityId);

    uint64_t GetHighestExpressionVersion(std::initializer_list<EntityID> expressionIds);
};

}  // namespace of::domain
