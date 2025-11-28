#pragma once
#include <neocad/domain/Registry.hpp>

namespace nc::domain {

class ExpressionSystem final {
   public:
    ExpressionSystem(Registry& registry);
    void UpdateExpressions();

   private:
    Registry& m_registry;
    void EvaluateExpression(EntityID expressionId);
};

}  // namespace nc::domain
