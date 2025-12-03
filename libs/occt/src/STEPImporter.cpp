#include <BRepMesh_IncrementalMesh.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <STEPControl_Reader.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <ontoflow/core/Logger.hpp>
#include <ontoflow/domain/Components.hpp>
#include <ontoflow/domain/Entity.hpp>
#include <ontoflow/domain/Vertex.hpp>
#include <ontoflow/occt/STEPImporter.hpp>

namespace of::occt {

using namespace of::domain;

namespace {
void FixNormals(domain::MeshComponent& mc) {
    auto& verts = mc.mesh.vertices;
    auto& idx = mc.mesh.indices;

    std::vector<glm::vec3> normals(verts.size(), glm::vec3(0.0f));

    for (size_t i = 0; i < idx.size(); i += 3) {
        uint32_t i0 = idx[i];
        uint32_t i1 = idx[i + 1];
        uint32_t i2 = idx[i + 2];

        glm::vec3 p0 = verts[i0].GetPosition();
        glm::vec3 p1 = verts[i1].GetPosition();
        glm::vec3 p2 = verts[i2].GetPosition();

        glm::vec3 n = glm::normalize(glm::cross(p1 - p0, p2 - p0));

        normals[i0] += n;
        normals[i1] += n;
        normals[i2] += n;
    }

    // Average & assign
    for (size_t i = 0; i < verts.size(); ++i) {
        verts[i].SetNormal(glm::normalize(normals[i]));
    }
}
} // namespace

Entity STEPImporter::Load(const std::string& filename, Registry& registry) {
    LOG(Info) << "STEPImporter: loading file: " << filename;

    STEPControl_Reader reader;
    IFSelect_ReturnStatus stat = reader.ReadFile(filename.c_str());
    if (stat != IFSelect_ReturnStatus::IFSelect_RetDone) {
        LOG(Error) << "STEPImporter: failed to read STEP file: " << filename;
        return INVALID_ENTITY;
    }

    // Alle Roots übertragen, dann eine kombinierte Shape holen
    reader.TransferRoots();
    TopoDS_Shape shape = reader.OneShape();

    // 1) Topologie → ECS (Points, Edges, Faces)
    ExtractTopology(shape, registry);

    // 2) Triangulation → MeshComponent
    MeshComponent meshComp;
    TriangulateShape(shape, meshComp);

    if (meshComp.mesh.vertices.empty() || meshComp.mesh.indices.empty()) {
        LOG(Warn) << "STEPImporter: triangulation produced no mesh.";
        return INVALID_ENTITY;
    }

    Entity meshEntity = registry.CreateEntity();
    registry.AddComponent<MeshComponent>(meshEntity, meshComp);
    registry.AddComponent<NameComponent>(meshEntity, NameComponent{"STEP_Mesh"});

    LOG(Info) << "STEPImporter: done. Mesh entity = " << meshEntity;
    return meshEntity;
}

void STEPImporter::ExtractTopology(const TopoDS_Shape& shape, Registry& registry) {
    TopTools_IndexedMapOfShape vertexMap;
    TopTools_IndexedMapOfShape edgeMap;
    TopTools_IndexedMapOfShape faceMap;

    TopExp::MapShapes(shape, TopAbs_VERTEX, vertexMap);
    TopExp::MapShapes(shape, TopAbs_EDGE, edgeMap);
    TopExp::MapShapes(shape, TopAbs_FACE, faceMap);

    const int nbV = vertexMap.Extent();
    const int nbE = edgeMap.Extent();
    const int nbF = faceMap.Extent();

    if (nbV == 0 && nbE == 0 && nbF == 0) {
        LOG(Warn) << "STEPImporter: no topological data found.";
        return;
    }

    // 1) Alle Vertices → PositionComponent-Entities
    std::vector<Entity> vertexEntities(nbV + 1, INVALID_ENTITY);  // 1-based
    for (int i = 1; i <= nbV; ++i) {
        TopoDS_Vertex v = TopoDS::Vertex(vertexMap(i));
        gp_Pnt p = BRep_Tool::Pnt(v);

        PositionComponent pc;
        pc.position = vec3(static_cast<float>(p.X()), static_cast<float>(p.Y()), static_cast<float>(p.Z()));

        Entity e = registry.CreateEntity();
        registry.AddComponent<PositionComponent>(e, pc);
        vertexEntities[i] = e;
    }

    // 2) Alle Edges → EdgeComponent-Entities (p0/p1 verweisen auf Position-Entities)
    std::vector<Entity> edgeEntities(nbE + 1, INVALID_ENTITY);
    for (int i = 1; i <= nbE; ++i) {
        TopoDS_Edge edge = TopoDS::Edge(edgeMap(i));

        Standard_Real first, last;
        Handle(Geom_Curve) geomCurve = BRep_Tool::Curve(edge, first, last);

        // --- Circle?
        if (!geomCurve.IsNull()) {
            Handle(Geom_Circle) circle = Handle(Geom_Circle)::DownCast(geomCurve);
            if (!circle.IsNull()) {
                gp_Ax2 axis = circle->Position();
                gp_Pnt center = axis.Location();
                double r = circle->Radius();

                Entity e = registry.CreateEntity();
                RadiusComponent rc;
                rc.radius = r;
                registry.AddComponent<RadiusComponent>(e, rc);
                PositionComponent pc;
                pc.position = vec3(static_cast<float>(center.X()), static_cast<float>(center.Y()),
                                   static_cast<float>(center.Z()));
                registry.AddComponent<PositionComponent>(e, pc);

                edgeEntities[i] = e;
                continue;  // KEINE EdgeComponent notwendig!
            }
        }

        // --- Edge hat 2 Eckpunkte → Line/EdgeComponent ---
        TopoDS_Vertex v1, v2;
        TopExp::Vertices(edge, v1, v2);
        if (!v1.IsNull() && !v2.IsNull()) {
            int idx1 = vertexMap.FindIndex(v1);
            int idx2 = vertexMap.FindIndex(v2);
            if (idx1 > 0 && idx2 > 0) {
                Entity e = registry.CreateEntity();
                registry.AddComponent<EdgeComponent>(e, {vertexEntities[idx1], vertexEntities[idx2]});
                edgeEntities[i] = e;
                continue;
            }
        }

        // --- Unknown?
        if (!geomCurve.IsNull()) {
            LOG(Warn) << "Not supported curve found";
        }
    }

    // 3) Faces → FaceComponent-Entities (mit Liste von Vertex & Edge-Entities)
    for (int i = 1; i <= nbF; ++i) {
        TopoDS_Face face = TopoDS::Face(faceMap(i));

        std::vector<Entity> faceEdgeEntities;
        std::vector<Entity> faceVertexEntities;

        // Edges auf dem Face sammeln
        for (TopExp_Explorer ex(face, TopAbs_EDGE); ex.More(); ex.Next()) {
            TopoDS_Edge ed = TopoDS::Edge(ex.Current());
            int eIdx = edgeMap.FindIndex(ed);
            if (eIdx <= 0)
                continue;

            Entity edgeEnt = edgeEntities[eIdx];
            if (edgeEnt == INVALID_ENTITY)
                continue;

            faceEdgeEntities.push_back(edgeEnt);

            // Optional: Vertices pro Edge einsammeln
            TopoDS_Vertex v1, v2;
            TopExp::Vertices(ed, v1, v2);
            if (!v1.IsNull()) {
                int vi = vertexMap.FindIndex(v1);
                if (vi > 0) {
                    Entity vEnt = vertexEntities[vi];
                    if (vEnt != INVALID_ENTITY)
                        faceVertexEntities.push_back(vEnt);
                }
            }
            if (!v2.IsNull()) {
                int vi = vertexMap.FindIndex(v2);
                if (vi > 0) {
                    Entity vEnt = vertexEntities[vi];
                    if (vEnt != INVALID_ENTITY)
                        faceVertexEntities.push_back(vEnt);
                }
            }
        }

        // Duplikate bei Vertices rauswerfen
        std::sort(faceVertexEntities.begin(), faceVertexEntities.end());
        faceVertexEntities.erase(std::unique(faceVertexEntities.begin(), faceVertexEntities.end()),
                                 faceVertexEntities.end());

        // Wenn nichts drin → face skippen
        if (faceEdgeEntities.empty() && faceVertexEntities.empty())
            continue;

        FaceComponent fc;
        fc.edges = std::move(faceEdgeEntities);
        fc.vertices = std::move(faceVertexEntities);

        Entity fEnt = registry.CreateEntity();
        registry.AddComponent<FaceComponent>(fEnt, fc);
        // Optional Name/Tag:
        // registry.AddComponent<NameComponent>(fEnt, NameComponent{"STEP_Face"});
    }

    LOG(Info) << "STEPImporter: topology extracted: " << nbV << " vertices, " << nbE << " edges, " << nbF << " faces.";
}

void STEPImporter::TriangulateShape(const TopoDS_Shape& shape, MeshComponent& outMesh) {
    auto& mesh = outMesh.mesh;

    const double deflection = 0.5;
    BRepMesh_IncrementalMesh mesher(shape, deflection, false);

    // Wir sammeln alle Dreiecke aus allen Faces in EINEM Mesh
    TopExp_Explorer faceExp(shape, TopAbs_FACE);
    for (; faceExp.More(); faceExp.Next()) {
        TopoDS_Face face = TopoDS::Face(faceExp.Current());

        TopLoc_Location loc;
        Handle(Poly_Triangulation) tri = BRep_Tool::Triangulation(face, loc);
        if (tri.IsNull())
            continue;

        // Transformation berücksichtigen (z.B. bei Assemblies)
        gp_Trsf trsf = loc.Transformation();

        // ⚠ Orientation check – WICHTIG!
        bool flipWinding = (face.Orientation() == TopAbs_REVERSED);

        // --- 1) VERTICES ---
        std::size_t baseIndex = mesh.vertices.size();  // Startindex dieses Faces

        const int nbV = tri->NbNodes();
        for (int i = 1; i <= nbV; ++i) {
            gp_Pnt p = tri->Node(i).Transformed(trsf);

            Vertex v;
            v.SetPosition(glm::vec3(static_cast<float>(p.X()), static_cast<float>(p.Y()), static_cast<float>(p.Z())));
            v.SetColor(glm::vec3(0.7f, 0.7f, 0.8f));

            mesh.vertices.push_back(v);
        }

        // --- 2) TRIANGLES (mit Face Orientation fixen!) ---
        const int nbT = tri->NbTriangles();
        for (int i = 1; i <= nbT; ++i) {
            Poly_Triangle t = tri->Triangle(i);
            int i1, i2, i3;
            t.Get(i1, i2, i3);

            if (flipWinding)
                std::swap(i2, i3);

            mesh.indices.push_back(static_cast<uint32_t>(baseIndex + i1 - 1));
            mesh.indices.push_back(static_cast<uint32_t>(baseIndex + i2 - 1));
            mesh.indices.push_back(static_cast<uint32_t>(baseIndex + i3 - 1));
        }
    }

    // Normals berechnen, falls dein Mesh das noch nicht selber macht
    if (!mesh.vertices.empty() && mesh.indices.size() % 3 == 0) {
        std::vector<glm::vec3> normals(mesh.vertices.size(), glm::vec3(0.0f));

        for (std::size_t i = 0; i < mesh.indices.size(); i += 3) {
            uint32_t ia = mesh.indices[i + 0];
            uint32_t ib = mesh.indices[i + 1];
            uint32_t ic = mesh.indices[i + 2];

            glm::vec3 a = mesh.vertices[ia].GetPosition();
            glm::vec3 b = mesh.vertices[ib].GetPosition();
            glm::vec3 c = mesh.vertices[ic].GetPosition();

            glm::vec3 n = glm::normalize(glm::cross(b - a, c - a));
            normals[ia] += n;
            normals[ib] += n;
            normals[ic] += n;
        }

        for (std::size_t i = 0; i < mesh.vertices.size(); ++i) {
            glm::vec3 n = normals[i];
            if (glm::length(n) > 0.0f)
                n = glm::normalize(n);
            mesh.vertices[i].SetNormal(n);
        }
    }

    FixNormals(outMesh);

    LOG(Info) << "STEPImporter: triangulation → " << mesh.vertices.size() << " vertices, " << mesh.indices.size() / 3
              << " triangles.";
}
}  // namespace of::occt
