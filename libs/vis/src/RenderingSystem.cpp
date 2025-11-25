#include <neocad/domain/Components.hpp>
#include <neocad/rendering/ICamera.hpp>
#include <neocad/rendering/RenderingSystem.hpp>

using namespace nc;

void RenderingSystem::Update(const Registry& ecs, const ICamera& cam) {
    m_Renderer.BeginFrame(cam);

    for (Entity e : ecs.Entities()) {
        if (auto* comp = ecs.TryGetComponent<MeshComponent>(e)) {
            auto& mesh = m_Meshes[e];
            if (!mesh)
                mesh = std::make_shared<MeshGPU>();
            mesh->Upload(comp->mesh);
            glm::mat4 model = glm::mat4(1.0f);
            m_Renderer.DrawMesh(*mesh, model);
        }

        if (auto* comp = ecs.TryGetComponent<LineSetComponent>(e)) {
            auto& lines = m_Meshes[e];
            if (!lines)
                lines = std::make_shared<LineSetGPU>();
            lines->Upload(comp->lineSet);
            m_Renderer.DrawLineSet(*lines, glm::mat4(1.0f));
        }

        if (auto* comp = ecs.TryGetComponent<PointComponent>(e)) {
            auto& pts = m_Meshes[e];
            if (!pts)
                pts = std::make_shared<PointGPU>();
            pts->Upload(comp->points);
            m_Renderer.DrawPoints(*pts, glm::mat4(1.0f));
        }
    }

    m_Renderer.EndFrame();
}
