#include "MeshRT.hpp"

#include <cassert>
#include <tiny_obj_loader.h>

#include "utils/utils.hpp"
#include "utils/status.hpp"
#include "utils/clrp.hpp"
#include "glm/gtc/quaternion.hpp"

void MeshRT::loadOBJ(const fspath& file, float scale, const vec3& offset, bool printInfo) {
  tinyobj::ObjReaderConfig readerConfig;
  tinyobj::ObjReader reader;
  status::start("Loading", file.string());

  if (!reader.ParseFromFile(file.string(), readerConfig)) {
    std::string msg = "ParseFromFile error";
    if (!reader.Error().empty())
      msg = "TinyObjReader: " + reader.Error();

    status::end(false);
    error(msg);
  }

  if (!reader.Warning().empty())
    warning(std::format("TinyObjReader: {}", reader.Warning()));

  const tinyobj::attrib_t& attrib = reader.GetAttrib();
  const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
  // const std::vector<tinyobj::material_t>& materials = reader.GetMaterials();

  // Loop over shapes
  u32 numVertices = 0;
  for (size_t s = 0; s < shapes.size(); s++) {
    // Loop over faces(polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

      // Loop over vertices
      Triangle tri;
      for (size_t v = 0; v < fv; v++) {
        // access to vertex
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        size_t idxVert = 3 * size_t(idx.vertex_index);
        vec3* pos = nullptr;
        vec3* normal = nullptr;
        switch (v) {
          case 0:
            pos = &tri.a;
            normal = &tri.normalA;
            break;
          case 1:
            pos = &tri.b;
            normal = &tri.normalB;
            break;
          case 2:
            pos = &tri.c;
            normal = &tri.normalC;
            break;
          default:
            error("[MeshRT::loadOBJ] Unexpected vertex index [{}]", v);
        }

        vec3& p = *pos;
        p.x = attrib.vertices[idxVert + 0];
        p.y = attrib.vertices[idxVert + 1];
        p.z = attrib.vertices[idxVert + 2];
        p *= scale;
        p += offset;

        meshInfo.boundsMin = glm::min(meshInfo.boundsMin, p);
        meshInfo.boundsMax = glm::max(meshInfo.boundsMax, p);

        // Check if `normal_index` is zero or positive. negative = no normal data
        if (idx.normal_index >= 0) {
          normal->x = attrib.normals[3*size_t(idx.normal_index)+0];
          normal->y = attrib.normals[3*size_t(idx.normal_index)+1];
          normal->z = attrib.normals[3*size_t(idx.normal_index)+2];
        }
        numVertices++;
      }
      triangles.push_back(tri);
      index_offset += fv;

      // per-face material
      // shapes[s].mesh.material_ids[f];
    }
  }

  assert(numVertices % 3 == 0);

  RayTracingMaterial material;
  material.color = vec4(1.f);
  material.emissionColor = vec4(0.f);
  material.emissionStrength = 0.f;

  meshInfo.numTriangles = numVertices / 3;
  meshInfo.material = material;

  // ============ Print info ============ //

  if (printInfo) {
    clrp::clrp_t cfmt{
      .attr = clrp::ATTRIBUTE::BOLD,
      .fg = clrp::FG::CYAN
    };
    std::string cname = clrp::format(std::format("[{}]", file.string()), cfmt);
    std::string infoLoad = std::format("[load]\nvertices: {}\ncolors:   {}\ntextures: {}\nnormals:  {}", attrib.vertices.size() / 3, attrib.colors.size() / 3, attrib.texcoords.size() / 2, attrib.normals.size() / 3);
    std::string infoFinal = std::format("[final]\nvertices: {}\n", numVertices);
    printf("\n==================== %s ====================\n\n%s\n\n%s\n\n", cname.c_str(), infoLoad.c_str(), infoFinal.c_str());

    std::string end = "============================================";
    for (u32 i = 0; i < file.string().size(); i++)
      end += "=";
    end += "\n";

    puts(end.c_str());
  }

  // ==================================== //

  status::end(true);
}

void MeshRT::createQuad(const vec3& bottomLeft, const vec3& axisY, const vec3& axisX, const vec3& normal, const vec2& size, const RayTracingMaterial& material) {
  Triangle tri1;
  tri1.a = bottomLeft + axisY * size.y;
  tri1.b = bottomLeft;
  tri1.c = bottomLeft + axisY * size.y + axisX * size.x;
  tri1.normalA = normal;
  tri1.normalB = normal;
  tri1.normalC = normal;
  triangles.push_back(tri1);

  Triangle tri2;
  tri2.a = bottomLeft + axisX * size.x;
  tri2.b = bottomLeft + axisY * size.y + axisX * size.x;
  tri2.c = bottomLeft;
  tri2.normalA = normal;
  tri2.normalB = normal;
  tri2.normalC = normal;
  triangles.push_back(tri2);

  meshInfo.numTriangles = 2;
  meshInfo.boundsMin = min(min(min(meshInfo.boundsMin, tri1.a), tri1.b), tri1.c);
  meshInfo.boundsMin = min(min(min(meshInfo.boundsMin, tri2.a), tri2.b), tri2.c);
  meshInfo.boundsMax = max(max(max(meshInfo.boundsMax, tri1.a), tri1.b), tri1.c);
  meshInfo.boundsMax = max(max(max(meshInfo.boundsMax, tri2.a), tri2.b), tri2.c);
  meshInfo.material = material;
}

void MeshRT::rotate(float rad, const vec3& axis) {
  glm::quat q = glm::angleAxis(rad, axis);

  for (Triangle& tri : triangles) {
    tri.a = q * tri.a;
    tri.b = q * tri.b;
    tri.c = q * tri.c;
    tri.normalA = q * tri.normalA;
    tri.normalB = q * tri.normalB;
    tri.normalC = q * tri.normalC;
  }
}
