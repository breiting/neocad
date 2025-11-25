#pragma once
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/ViewController.hpp>
#include <neocad/vis/Mesh.hpp>
#include <neocad/vis/Renderer.hpp>
#include <unordered_map>

namespace nc::vis {

class RenderingSystem {
   public:
    RenderingSystem(Renderer& r) : m_Renderer(r) {
    }

    void Update(domain::Registry& registry, const editor::ViewState& view);
    void Render();

   private:
    Renderer& m_Renderer;
    editor::ViewState m_ViewState;
    std::unordered_map<domain::Entity, std::shared_ptr<Mesh>> m_Meshes;
};

}  // namespace nc::vis
