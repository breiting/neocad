#include <neocad/core/Colors.hpp>
#include <neocad/core/Logger.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Query.hpp>
#include <neocad/editor/ViewController.hpp>
#include <neocad/vis/DirectionalLight.hpp>
#include <neocad/vis/FlatShadedMaterial.hpp>
#include <neocad/vis/LineSetMaterial.hpp>
#include <neocad/vis/RenderingSystem.hpp>

using namespace nc::domain;
using namespace nc::editor;

const glm::vec3 SUN_LIGHT = {1.0f, 0.95f, 0.9f};

/**
 * Make sure to convert all geometry into our coordinate system where +z is the height. Therefore we just flip Y and Z
 */
const glm::mat4 GLOBAL_WORLD_TRANSFORM = glm::mat4(1, 0, 0, 0,   //
                                                   0, 0, -1, 0,  //
                                                   0, 1, 0, 0,   //
                                                   0, 0, 0, 1);

namespace nc::vis {

RenderingSystem::RenderingSystem(std::unique_ptr<IRenderer> r) : m_Renderer(std::move(r)) {
    m_Light = std::make_shared<DirectionalLight>();
    m_Light->SetColor(SUN_LIGHT);
}

void RenderingSystem::SetViewportSize(int w, int h) {
    m_Renderer->SetViewportSize(w, h);
}

bool RenderingSystem::Init() {
    m_Axis = std::make_unique<AxisRenderer>();
    return m_Axis->Init();
}

void RenderingSystem::SetShowAxis(bool b) {
    m_Axis->SetVisible(b);
}

void RenderingSystem::Update(Registry& registry) {
    {
        auto entities = HasComponentQuery<MeshComponent>().Execute(registry);
        for (Entity e : entities) {
            auto* comp = registry.GetComponent<MeshComponent>(e);
            if (!comp)
                continue;

            auto& mesh = m_Meshes[e];
            auto& material = m_Material[e];
            if (!mesh && !material) {
                LOG(INFO) << "Creating Mesh ...";
                mesh = std::make_shared<Mesh>();
                material =
                    std::make_shared<FlatShadedMaterial>(glm::vec3(core::Nord11.r, core::Nord11.g, core::Nord11.b));

                mesh->SetVertices(comp->mesh.vertices);
                for (size_t i = 0; i + 2 < comp->mesh.indices.size(); i += 3) {
                    mesh->AddTriangle(comp->mesh.indices[i], comp->mesh.indices[i + 1], comp->mesh.indices[i + 2]);
                }
                mesh->RecalculateNormals();
                mesh->Upload();
            }
        }
    }

    {
        auto entities = HasComponentQuery<FaceComponent>().Execute(registry);
        for (Entity e : entities) {
            auto* comp = registry.GetComponent<FaceComponent>(e);
            if (!comp)
                continue;

            auto& lines = m_Lines[e];
            auto& material = m_Material[e];
            if (!lines) {
                LOG(INFO) << "Creeating LineSet ...";
                lines = std::make_shared<LineSet>();
                material = std::make_shared<LineSetMaterial>();

                std::vector<Vertex> vertices;
                vertices.reserve(comp->vertices.size());

                for (Entity vEnt : comp->vertices) {
                    if (auto* pos = registry.GetComponent<PositionComponent>(vEnt)) {
                        Vertex v;
                        printf("%f %f %f\n", pos->position.x, pos->position.y, pos->position.z);
                        v.SetPosition(pos->position);
                        v.SetColor({core::Nord12.r, core::Nord12.g, core::Nord12.b});
                        vertices.push_back(v);
                    }
                }
                lines->SetVertices(vertices);
                lines->Upload();
            }
        }
    }

    // 3) RENDER POINTS
    // {
    //     auto entities = HasComponentQuery<PointComponent>().Execute(registry);
    //     for (Entity e : entities) {
    //         auto* comp = registry.GetComponent<PointComponent>(e);
    //         if (!comp)
    //             continue;
    //
    //         auto& gpuPoints = m_GraphicCache[e];
    //         if (!gpuPoints)
    //             gpuPoints = std::make_shared<PointGPU>();
    //
    //         gpuPoints->Upload(comp->points);
    //         m_Renderer.DrawPoints(*gpuPoints, glm::mat4(1.0f));
    //     }
    // }
}

void RenderingSystem::Render(ICamera* cam) {
    if (!cam)
        return;
    if (m_Light) {
        m_Light->SetDirection(cam->GetViewDirection());
    }
    m_Renderer->BeginFrame(cam->GetViewMatrix(), cam->GetProjectionMatrix(), m_Light);

    if (m_Axis)
        m_Axis->Render(GLOBAL_WORLD_TRANSFORM, cam->GetViewMatrix(), cam->GetProjectionMatrix());

    for (auto& [e, mesh] : m_Meshes) {
        m_Renderer->DrawMesh(mesh, m_Material[e], GLOBAL_WORLD_TRANSFORM * glm::mat4(1.0f));
    }
    for (auto& [e, lines] : m_Lines) {
        m_Renderer->DrawLineSet(lines, m_Material[e], GLOBAL_WORLD_TRANSFORM * glm::mat4(1.0f));
    }
    m_Renderer->EndFrame();
}

}  // namespace nc::vis
