#include <neocad/domain/Components.hpp>
#include <neocad/domain/Query.hpp>
#include <neocad/editor/ViewController.hpp>
#include <neocad/vis/RenderingSystem.hpp>

using namespace nc::domain;
using namespace nc::editor;

namespace nc::vis {
void RenderingSystem::Update(Registry& registry, const ViewState& state) {
    m_ViewState = state;
    {
        auto entities = HasComponentQuery<MeshComponent>().Execute(registry);
        for (Entity e : entities) {
            auto* comp = registry.GetComponent<MeshComponent>(e);
            if (!comp)
                continue;

            auto& mesh = m_Meshes[e];
            if (!mesh) {
                mesh = std::make_shared<Mesh>();

                mesh->SetVertices(comp->mesh.vertices);
                for (size_t i = 0; i + 2 < comp->mesh.indices.size(); i += 3) {
                    mesh->AddTriangle(comp->mesh.indices[i], comp->mesh.indices[i + 1], comp->mesh.indices[i + 2]);
                }
                mesh->RecalculateNormals();
                mesh->Upload();
            }

            m_Renderer.DrawMesh(*mesh, glm::mat4(1.0f));
        }
    }

#if 0
    // 2) RENDER LINE SETS
    {
        auto entities = HasComponentQuery<LineComponent>().Execute(registry);
        for (Entity e : entities) {
            auto* comp = registry.GetComponent<LineComponent>(e);
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
        auto entities = HasComponentQuery<PointComponent>().Execute(registry);
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
#endif

    m_Renderer.EndFrame();
}

void RenderingSystem::Render() {
    m_Renderer.BeginFrame(m_ViewState.view, m_ViewState.proj);

    for (auto& [e, geom] : m_Meshes) {
        m_Renderer.DrawMesh(*geom, glm::mat4(1.0f));
    }
    // TODO
}

}  // namespace nc::vis
