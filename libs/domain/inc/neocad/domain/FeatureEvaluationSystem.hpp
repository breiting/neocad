#pragma once
#include <initializer_list>
#include <neocad/domain/IGeometryBackend.hpp>
#include <neocad/domain/Registry.hpp>

namespace nc::domain {

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

}  // namespace nc::domain
