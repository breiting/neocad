#pragma once
#include <neocad/domain/Entity.hpp>
#include <neocad/editor/ITool.hpp>
#include <optional>

namespace nc::editor {

class InsertCircleTool : public ITool {
   public:
    void OnEnter(ToolContext& ctx) override;
    void OnExit(ToolContext& ctx) override;
    bool OnInput(const InputEvent& event, ToolContext& ctx) override;

    void Update(ToolContext& /*ctx*/, double /*dt*/) override {
    }

   private:
    std::optional<domain::Entity> m_Center;  // first click stores center
};

}  // namespace nc::editor
