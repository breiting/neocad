#pragma once
#include <neocad/domain/Registry.hpp>
#include <neocad/rendering/Mesh.hpp>
#include <neocad/rendering/Renderer.hpp>
#include <unordered_map>

namespace nc::vis {

class RenderingSystem {
   public:
    RenderingSystem(Renderer& r) : m_Renderer(r) {
    }

    void Update(const Registry& ecs, const ICamera& cam);

   private:
    Renderer& m_Renderer;
    std::unordered_map<Entity, std::shared_ptr<Mesh>> m_Meshes;
};

}  // namespace nc
