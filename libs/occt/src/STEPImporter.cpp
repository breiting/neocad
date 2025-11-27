#include <BRepMesh_IncrementalMesh.hxx>
#include <BRep_Tool.hxx>
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
#include <neocad/core/Logger.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Entity.hpp>
#include <neocad/domain/Vertex.hpp>
#include <neocad/occt/STEPImporter.hpp>

namespace nc::occt {

using namespace nc::domain;

Entity STEPImporter::Load(const std::string& filename, Registry& registry) {
    LOG(INFO) << "STEPImporter: loading file: " << filename;

    STEPControl_Reader reader;
    IFSelect_ReturnStatus stat = reader.ReadFile(filename.c_str());
    if (stat != IFSelect_ReturnStatus::IFSelect_RetDone) {
        LOG(ERROR) << "STEPImporter: failed to read STEP file: " << filename;
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
        LOG(WARN) << "STEPImporter: triangulation produced no mesh.";
        return INVALID_ENTITY;
    }

    Entity meshEntity = registry.CreateEntity();
    registry.AddComponent<MeshComponent>(meshEntity, meshComp);
    registry.AddComponent<NameComponent>(meshEntity, NameComponent{"STEP_Mesh"});

    LOG(INFO) << "STEPImporter: done. Mesh entity = " << meshEntity;
    return meshEntity;
}

// ---------------------------
//  Topology → ECS
// ---------------------------

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
        LOG(WARN) << "STEPImporter: no topological data found.";
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
        TopoDS_Edge ed = TopoDS::Edge(edgeMap(i));

        TopoDS_Vertex v1, v2;
        TopExp::Vertices(ed, v1, v2);
        if (v1.IsNull() || v2.IsNull())
            continue;

        int idx1 = vertexMap.FindIndex(v1);
        int idx2 = vertexMap.FindIndex(v2);
        if (idx1 <= 0 || idx2 <= 0)
            continue;

        EdgeComponent ec;
        ec.p0 = vertexEntities[idx1];
        ec.p1 = vertexEntities[idx2];

        Entity e = registry.CreateEntity();
        registry.AddComponent<EdgeComponent>(e, ec);
        edgeEntities[i] = e;
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

    LOG(INFO) << "STEPImporter: topology extracted: " << nbV << " vertices, " << nbE << " edges, " << nbF << " faces.";
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

        gp_Trsf trsf = loc.Transformation();

        std::size_t baseIndex = mesh.vertices.size();

        const int nbV = tri->NbNodes();
        for (int i = 1; i <= nbV; ++i) {
            gp_Pnt p = tri->Node(i).Transformed(trsf);
            Vertex v;
            v.SetPosition(glm::vec3(static_cast<float>(p.X()), static_cast<float>(p.Y()), static_cast<float>(p.Z())));
            // Farbe erstmal neutral, kann später per Face-ID gesetzt werden
            v.SetColor(glm::vec3(0.7f, 0.7f, 0.8f));
            mesh.vertices.push_back(v);
        }

        const int nbT = tri->NbTriangles();
        for (int i = 1; i <= nbT; ++i) {
            Poly_Triangle t = tri->Triangle(i);
            int i1, i2, i3;
            t.Get(i1, i2, i3);

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

    LOG(INFO) << "STEPImporter: triangulation → " << mesh.vertices.size() << " vertices, " << mesh.indices.size() / 3
              << " triangles.";
}
}  // namespace nc::occt
