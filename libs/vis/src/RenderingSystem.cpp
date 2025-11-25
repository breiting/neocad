#include <neocad/domain/Components.hpp>
#include <neocad/domain/Query.hpp>
#include <neocad/rendering/ICamera.hpp>
#include <neocad/rendering/RenderingSystem.hpp>

using namespace nc::domain;

namespace nc::vis {
void RenderingSystem::Update(const Registry& registry, const ICamera& cam) {
    m_Renderer.BeginFrame(cam);

    // 1) RENDER MESHES
    {
        auto entities = Query{}.Where(HasComponent<MeshComponent>()).Execute(registry);
        for (Entity e : entities) {
            auto* comp = registry.GetComponent<MeshComponent>(e);
            if (!comp)
                continue;

            auto& gpuMesh = m_GraphicCache[e];
            if (!gpuMesh)
                gpuMesh = std::make_shared<MeshGPU>();

            gpuMesh->Upload(comp->mesh);
            m_Renderer.DrawMesh(*gpuMesh, glm::mat4(1.0f));
        }
    }

    // 2) RENDER LINE SETS
    {
        auto entities = Query{}.Where(HasComponent<LineSetComponent>()).Execute(registry);
        for (Entity e : entities) {
            auto* comp = registry.GetComponent<LineSetComponent>(e);
            if (!comp)
                continue;

            auto& gpuLines = m_GraphicCache[e];
            if (!gpuLines)
                gpuLines = std::make_shared<LineSetGPU>();

            gpuLines->Upload(comp->lineSet);
            m_Renderer.DrawLineSet(*gpuLines, glm::mat4(1.0f));
        }
    }

    // 3) RENDER POINTS
    {
        auto entities = Query{}.Where(HasComponent<PointComponent>()).Execute(registry);
        for (Entity e : entities) {
            auto* comp = registry.GetComponent<PointComponent>(e);
            if (!comp)
                continue;

            auto& gpuPoints = m_GraphicCache[e];
            if (!gpuPoints)
                gpuPoints = std::make_shared<PointGPU>();

            gpuPoints->Upload(comp->points);
            m_Renderer.DrawPoints(*gpuPoints, glm::mat4(1.0f));
        }
    }

    m_Renderer.EndFrame();
}
}  // namespace nc::vis
