#include <memory>
#include <neocad/core/Colors.hpp>
#include <neocad/core/Logger.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Query.hpp>
#include <neocad/editor/ViewController.hpp>
#include <neocad/vis/DirectionalLight.hpp>
#include <neocad/vis/FlatShadedMaterial.hpp>
#include <neocad/vis/LineSetMaterial.hpp>
#include <neocad/vis/PointSetMaterial.hpp>
#include <neocad/vis/RenderingSystem.hpp>

using namespace nc::domain;
using namespace nc::editor;

// Default color for the main light source (Sun)
const glm::vec3 SUN_LIGHT = {1.0f, 0.95f, 0.9f};

namespace nc::vis {

/**
 * \brief Constructs a RenderingSystem.
 * Initializes the light source and materials for points.
 * \param r An r-value reference to a unique_ptr to the IRenderer implementation.
 */
RenderingSystem::RenderingSystem(std::unique_ptr<IRenderer> r) : m_Renderer(std::move(r)) {
    m_Light = std::make_shared<DirectionalLight>();
    m_Light->SetColor(SUN_LIGHT);
    m_PointSetMaterial = std::make_shared<PointSetMaterial>();
    m_Points = std::make_shared<PointSet>();
}

/**
 * \brief Sets the viewport size for the underlying renderer.
 * \param w Width of the viewport in pixels.
 * \param h Height of the viewport in pixels.
 */
void RenderingSystem::SetViewportSize(int w, int h) {
    m_Renderer->SetViewportSize(w, h);
}

/**
 * \brief Initializes the rendering system.
 * Registers callbacks to the ECS registry to react to component changes.
 * \param registry The central ECS registry.
 * \return True if initialization was successful, false otherwise.
 */
bool RenderingSystem::Init(Registry& registry) {
    m_Axis = std::make_unique<AxisRenderer>();

    // Register change callbacks for dirty flags and resource cleanup
    registry.OnComponentAdded<PositionComponent>([&](Entity) {
        m_PointsDirty = true;
    });
    registry.OnComponentModified<PositionComponent>([&](Entity) {
        m_PointsDirty = true;
    });
    registry.OnComponentRemoved<PositionComponent>([&](Entity) {
        m_PointsDirty = true;
    });

    registry.OnComponentRemoved<EdgeComponent>([&](Entity e) {
        // When an EdgeComponent is removed, remove its corresponding LineSet from rendering.
        m_Lines.erase(e);
        m_Material.erase(e); // Also remove its material
    });
    
    // TODO: Add callbacks for MeshComponent and FaceComponent removal for cleanup.
    // For MeshComponent: registry.OnComponentRemoved<MeshComponent>([&](Entity e) { m_Meshes.erase(e); m_Material.erase(e); });
    // For FaceComponent: This creates LineSets, which are already handled by EdgeComponent removal if Face consists of edges.
    // Need to consider if FaceComponents directly create unique renderable objects.


    return m_Axis->Init();
}

/**
 * \brief Sets the visibility of the coordinate axis display.
 * \param b True to show the axis, false to hide.
 */
void RenderingSystem::SetShowAxis(bool b) {
    m_Axis->SetVisible(b);
}

/**
 * \brief Updates the rendering system's internal state based on the ECS registry.
 * This method processes ECS components and prepares renderable objects.
 *
 * \note Current implementation:
 * - For MeshComponent, if a rendering mesh does not exist, it's created and uploaded.
 *   Vertices and indices are copied from domain::Mesh. Normals are recalculated.
 *   Material color is hardcoded (Nord11).
 * - For FaceComponent and EdgeComponent, similar logic for LineSets.
 *   Material colors are hardcoded (Nord12, Nord13).
 * - For PositionComponent, a single PointSet is rebuilt and uploaded if any position
 *   component changed (`m_PointsDirty`).
 *
 * \warning Efficiency: The current approach for meshes and lines re-copies and re-uploads
 * data if they are new. For existing meshes/lines, a more granular update mechanism
 * (e.g., checking if the domain::Mesh/Line data itself is dirty) would be more efficient.
 * For points, a full rebuild is done if any position changes.
 *
 * \param registry The central ECS registry.
 */
void RenderingSystem::Update(Registry& registry) {
    // Process MeshComponents
    {
        auto entities = HasComponentQuery<MeshComponent>().Execute(registry);
        for (Entity e : entities) {
            auto* comp = registry.GetComponent<MeshComponent>(e);
            if (!comp)
                continue;

            auto& mesh = m_Meshes[e];
            auto& material = m_Material[e];
            if (!mesh) { // Only create if it doesn't exist
                LOG(Info) << "Creating Mesh for entity: " << e;
                mesh = std::make_shared<Mesh>();
                // Material color is hardcoded here.
                material =
                    std::make_shared<FlatShadedMaterial>(glm::vec3(core::nord::Nord11.r, core::nord::Nord11.g, core::nord::Nord11.b));

                mesh->SetVertices(comp->mesh.vertices);
                for (size_t i = 0; i + 2 < comp->mesh.indices.size(); i += 3) {
                    mesh->AddTriangle(comp->mesh.indices[i], comp->mesh.indices[i + 1], comp->mesh.indices[i + 2]);
                }
                mesh->RecalculateNormals();
                mesh->Upload();
            }
            // TODO: If comp->mesh data changes, update existing vis::Mesh.
            // This would require a dirty flag on domain::MeshComponent or similar.
        }
    }

    // Process FaceComponents (rendered as LineSets)
    {
        auto entities = HasComponentQuery<FaceComponent>().Execute(registry);
        for (Entity e : entities) {
            auto* comp = registry.GetComponent<FaceComponent>(e);
            if (!comp)
                continue;

            auto& lines = m_Lines[e];
            auto& material = m_Material[e];
            if (!lines) { // Only create if it doesn't exist
                LOG(Info) << "Creating LineSet for Face entity: " << e;
                lines = std::make_shared<LineSet>();
                material = std::make_shared<LineSetMaterial>();

                std::vector<Vertex> vertices;
                vertices.reserve(comp->vertices.size()); // Reserve space for vertices

                for (Entity vEnt : comp->vertices) {
                    if (auto* pos = registry.GetComponent<PositionComponent>(vEnt)) {
                        Vertex v;
                        v.SetPosition(pos->position);
                        // Material color is hardcoded here.
                        v.SetColor({core::nord::Nord12.r, core::nord::Nord12.g, core::nord::Nord12.b});
                        vertices.push_back(v);
                    }
                }
                lines->SetVertices(vertices);
                lines->Upload();
            }
            // TODO: If comp->vertices data changes, update existing vis::LineSet.
        }
    }

    // Process EdgeComponents (rendered as LineSets)
    {
        auto entities = HasComponentQuery<EdgeComponent>().Execute(registry);
        for (Entity e : entities) {
            auto* comp = registry.GetComponent<EdgeComponent>(e);
            if (!comp)
                continue;

            auto& lines = m_Lines[e];
            auto& material = m_Material[e];
            if (!lines) { // Only create if it doesn't exist
                LOG(Info) << "Creating LineSet for Edge entity: " << e;
                lines = std::make_shared<LineSet>();
                material = std::make_shared<LineSetMaterial>();

                Vertex v0, v1;
                auto p0 = registry.GetComponent<PositionComponent>(comp->p0);
                auto p1 = registry.GetComponent<PositionComponent>(comp->p1);

                if(p0 && p1) { // Ensure points exist
                    v0.SetPosition(p0->position);
                    // Material color is hardcoded here.
                    v0.SetColor({core::nord::Nord13.r, core::nord::Nord13.g, core::nord::Nord13.b});
                    v1.SetPosition(p1->position);
                    v1.SetColor({core::nord::Nord13.r, core::nord::Nord13.g, core::nord::Nord13.b});
                    lines->SetVertices({v0, v1});
                    lines->Upload();
                } else {
                    LOG(Warn) << "RenderingSystem: Edge " << e << " refers to non-existent PositionComponents.";
                    // Consider removing this line set if its points are invalid.
                    m_Lines.erase(e);
                    m_Material.erase(e);
                }
            }
            // TODO: If associated PositionComponents change, update existing vis::LineSet.
        }
    }

    // Process PositionComponents (rendered as a single PointSet)
    {
        if (m_PointsDirty) {
            auto entities = HasComponentQuery<PositionComponent>().Execute(registry);
            LOG(Info) << "Creating/Updating PointSet (Count: " << entities.size() << ")";
            
            std::vector<Vertex> vertices;
            vertices.reserve(entities.size());

            for (Entity e : entities) {
                if (auto* pc = registry.GetComponent<domain::PositionComponent>(e)) {
                    Vertex v;
                    v.SetPosition(pc->position);
                    // Material color is hardcoded here.
                    v.SetColor({core::nord::Nord12.r, core::nord::Nord12.g, core::nord::Nord12.b});
                    vertices.push_back(v);
                }
            }
            m_Points->SetVertices(std::move(vertices));
            m_Points->Upload();
            m_PointsDirty = false;
        }
    }
}

/**
 * \brief Renders the current scene.
 * \param cam A pointer to the active camera.
 *
 * \note The directional light's direction is currently set to the camera's view direction,
 * meaning the light always follows the camera. This might be a design choice but could
 * be made configurable.
 */
void RenderingSystem::Render(ICamera* cam) {
    if (!cam) {
        LOG(Error) << "RenderingSystem::Render called with null camera.";
        return;
    }
    if (m_Light) {
        m_Light->SetDirection(cam->GetViewDirection()); // Light always comes from camera for now.
    }
    m_Renderer->BeginFrame(cam->GetViewMatrix(), cam->GetProjectionMatrix(), m_Light);

    if (m_Axis && m_Axis->IsVisible()) // Only render axis if visible
        m_Axis->Render(glm::mat4(1.0f), cam->GetViewMatrix(), cam->GetProjectionMatrix());

    // Render Meshes
    for (auto& [e, mesh] : m_Meshes) {
        // Ensure material exists for this mesh
        auto it = m_Material.find(e);
        if (it != m_Material.end() && it->second) {
            m_Renderer->DrawMesh(mesh, it->second, glm::mat4(1.0f));
        } else {
            LOG(Warn) << "RenderingSystem: No material found for mesh entity " << e;
        }
    }
    // Render LineSets
    for (auto& [e, lines] : m_Lines) {
        // Ensure material exists for this lineset
        auto it = m_Material.find(e);
        if (it != m_Material.end() && it->second) {
            m_Renderer->DrawLineSet(lines, it->second, glm::mat4(1.0f));
        } else {
            LOG(Warn) << "RenderingSystem: No material found for lineset entity " << e;
        }
    }
    // Batch render points
    if (m_Points && m_PointSetMaterial)
        m_Renderer->DrawPoints(m_Points, m_PointSetMaterial, glm::mat4(1.0f));

    m_Renderer->EndFrame();
}

}  // namespace nc::vis
