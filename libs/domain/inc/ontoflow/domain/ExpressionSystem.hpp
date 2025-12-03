#pragma once
#include <ontoflow/domain/Registry.hpp>

namespace of::domain {

class ExpressionSystem final {
   public:
    ExpressionSystem(Registry& registry);
    void UpdateExpressions();

   private:
    Registry& m_registry;
    void EvaluateExpression(EntityID expressionId);
};

}  // namespace of::domain
