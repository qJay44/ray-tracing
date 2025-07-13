#include "MeshRT.hpp"
#include "utils/utils.hpp"
#include <cassert>

#include <tiny_obj_loader.h>

#include "utils/status.hpp"
#include "utils/clrp.hpp"

void MeshRT::loadOBJ(const fspath& file, bool printInfo) {
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
        vec3* normal;
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
