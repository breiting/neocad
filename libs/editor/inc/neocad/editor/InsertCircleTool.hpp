#pragma once
#include <neocad/domain/Entity.hpp>
#include <neocad/editor/ITool.hpp>
#include <optional>

namespace nc::editor {

class InsertCircleTool : public ITool {
   public:
    void OnEnter(ToolContext&) override {
        m_Center.reset();
    }
    void OnExit(ToolContext&) override {
        m_Center.reset();
    }
    void OnInput(const InputEvent& event, ToolContext& ctx) override;

    void Update(ToolContext& /*m_Ctx*/, double /*dt*/) override {
    }

   private:
    std::optional<domain::Entity> m_Center;  // first click stores center
};

}  // namespace nc::editor
