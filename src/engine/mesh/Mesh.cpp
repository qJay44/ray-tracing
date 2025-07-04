#include "Mesh.hpp"
#include "vertex.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "utils/status.hpp"
#include "utils/clrp.hpp"

template<>
Mesh<Vertex4>::Mesh(const std::vector<Vertex4>& vertices, const std::vector<GLuint>& indices, GLenum mode, bool autoclear)
  : MeshBase(indices, mode, autoclear),
    vertices(vertices),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(Vertex4) * vertices.size())),
    ebo(1, indices.data(), sizeof(GLuint) * indices.size())
{
  this->vertices.resize(vertices.size());
  this->vertices.reserve(vertices.size());

  vao.bind();
  vbo.bind();
  ebo.bind();

  size_t typeSize = sizeof(float);
  GLsizei stride = static_cast<GLsizei>(MESH_VERTEX_ATTRIBUTES * typeSize);

  vao.linkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize));
  vao.linkAttrib(1, 3, GL_FLOAT, stride, (void*)(3 * typeSize));
  vao.linkAttrib(2, 2, GL_FLOAT, stride, (void*)(6 * typeSize));
  vao.linkAttrib(3, 3, GL_FLOAT, stride, (void*)(8 * typeSize));

  VAO::unbind();
  VBO::unbind();
  EBO::unbind();
}

template<>
Mesh<Vertex1>::Mesh(const std::vector<Vertex1>& vertices, const std::vector<GLuint>& indices, GLenum mode, bool autoclear)
  : MeshBase(indices, mode, autoclear),
    vertices(vertices),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(Vertex1) * vertices.size())),
    ebo(1, indices.data(), sizeof(GLuint) * indices.size())
{
  this->vertices.resize(vertices.size());
  this->vertices.reserve(vertices.size());

  vao.bind();
  vbo.bind();
  ebo.bind();

  vao.linkAttrib(0, 3, GL_FLOAT, 3 * sizeof(float), (void*)(0));

  VAO::unbind();
  VBO::unbind();
  EBO::unbind();
}

template<>
Mesh<VertexPT>::Mesh(const std::vector<VertexPT>& vertices, const std::vector<GLuint>& indices, GLenum mode, bool autoclear)
  : MeshBase(indices, mode, autoclear),
    vertices(vertices),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(VertexPT) * vertices.size())),
    ebo(1, indices.data(), sizeof(GLuint) * indices.size())
{
  this->vertices.resize(vertices.size());
  this->vertices.reserve(vertices.size());

  vao.bind();
  vbo.bind();
  ebo.bind();

  size_t typeSize = sizeof(float);
  GLsizei stride = static_cast<GLsizei>((3 + 2) * typeSize);

  vao.linkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize));
  vao.linkAttrib(1, 2, GL_FLOAT, stride, (void*)(3 * typeSize));

  VAO::unbind();
  VBO::unbind();
  EBO::unbind();
}

template<>
Mesh<VertexPC>::Mesh(const std::vector<VertexPC>& vertices, const std::vector<GLuint>& indices, GLenum mode, bool autoclear)
  : MeshBase(indices, mode, autoclear),
    vertices(vertices),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(VertexPC) * vertices.size())),
    ebo(1, indices.data(), sizeof(GLuint) * indices.size())
{
  this->vertices.resize(vertices.size());
  this->vertices.reserve(vertices.size());

  vao.bind();
  vbo.bind();
  ebo.bind();

  size_t typeSize = sizeof(float);
  GLsizei stride = static_cast<GLsizei>((3 + 3) * typeSize);

  vao.linkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize));
  vao.linkAttrib(1, 3, GL_FLOAT, stride, (void*)(3 * typeSize));

  VAO::unbind();
  VBO::unbind();
  EBO::unbind();
}

template<>
Mesh<Vertex4>::Mesh(const std::vector<Vertex4>& vertices, GLenum mode, bool autoclear)
  : MeshBase(std::vector<GLuint>(), mode, autoclear),
    vertices(vertices),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(Vertex4) * vertices.size()))
{
  this->vertices.resize(vertices.size());
  this->vertices.reserve(vertices.size());
  vao.bind();
  vbo.bind();

  size_t typeSize = sizeof(float);
  GLsizei stride = static_cast<GLsizei>(MESH_VERTEX_ATTRIBUTES * typeSize);

  vao.linkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize));
  vao.linkAttrib(1, 3, GL_FLOAT, stride, (void*)(3 * typeSize));
  vao.linkAttrib(2, 2, GL_FLOAT, stride, (void*)(6 * typeSize));
  vao.linkAttrib(3, 3, GL_FLOAT, stride, (void*)(8 * typeSize));

  VAO::unbind();
  VBO::unbind();
}

template<>
Mesh<Vertex1>::Mesh(const std::vector<Vertex1>& vertices, GLenum mode, bool autoclear)
  : MeshBase(std::vector<GLuint>(), mode, autoclear),
    vertices(vertices),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(Vertex1) * vertices.size()))
{
  this->vertices.resize(vertices.size());
  this->vertices.reserve(vertices.size());
  vao.bind();
  vbo.bind();

  vao.linkAttrib(0, 3, GL_FLOAT, 3 * sizeof(float), (void*)(0));

  VAO::unbind();
  VBO::unbind();
}

template<>
Mesh<Vertex4> Mesh<Vertex4>::loadObj(const fspath& file, bool printInfo) {
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

  std::vector<Vertex4> vertices;

  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {
    // Loop over faces(polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

      // Loop over vertices in the face.
      for (size_t v = 0; v < fv; v++) {
        // access to vertex
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        size_t idxVert = 3 * size_t(idx.vertex_index);
        Vertex4 vertex;

        vertex.position.x = attrib.vertices[idxVert + 0];
        vertex.position.y = attrib.vertices[idxVert + 1];
        vertex.position.z = attrib.vertices[idxVert + 2];

        // Check if `normal_index` is zero or positive. negative = no normal data
        if (idx.normal_index >= 0) {
          vertex.normal.x = attrib.normals[3*size_t(idx.normal_index)+0];
          vertex.normal.y = attrib.normals[3*size_t(idx.normal_index)+1];
          vertex.normal.z = attrib.normals[3*size_t(idx.normal_index)+2];
        }

        // Check if `texcoord_index` is zero or positive. negative = no texcoord data
        if (idx.texcoord_index >= 0) {
          vertex.texture.x = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
          vertex.texture.y = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
        }

        // Optional: vertex colors
        vertex.color.x = attrib.colors[3*size_t(idx.vertex_index)+0];
        vertex.color.y = attrib.colors[3*size_t(idx.vertex_index)+1];
        vertex.color.z = attrib.colors[3*size_t(idx.vertex_index)+2];

        vertices.push_back(vertex);
      }
      index_offset += fv;

      // per-face material
      // shapes[s].mesh.material_ids[f];
    }
  }

  // ============ Print info ============ //

  if (printInfo) {
    clrp::clrp_t cfmt{
      .attr = clrp::ATTRIBUTE::BOLD,
      .fg = clrp::FG::CYAN
    };
    std::string cname = clrp::format(std::format("[{}]", file.string()), cfmt);
    std::string infoLoad = std::format("[load]\nvertices: {}\ncolors:   {}\ntextures: {}\nnormals:  {}", attrib.vertices.size() / 3, attrib.colors.size() / 3, attrib.texcoords.size() / 2, attrib.normals.size() / 3);
    std::string infoFinal = std::format("[final]\nvertices: {}\n", vertices.size());
    printf("\n==================== %s ====================\n\n%s\n\n%s\n\n", cname.c_str(), infoLoad.c_str(), infoFinal.c_str());

    std::string end = "============================================";
    for (u32 i = 0; i < file.string().size(); i++)
      end += "=";
    end += "\n";

    puts(end.c_str());
  }

  // ==================================== //

  status::end(true);

  return Mesh<Vertex4>(vertices, GL_TRIANGLES, false);
}

template<>
Mesh<VertexPC>::Mesh(const std::vector<VertexPC>& vertices, GLenum mode, bool autoclear)
  : MeshBase(std::vector<GLuint>(), mode, autoclear),
    vertices(vertices),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(VertexPC) * vertices.size()))
{
  this->vertices.resize(vertices.size());
  this->vertices.reserve(vertices.size());
  vao.bind();
  vbo.bind();

  size_t typeSize = sizeof(float);
  GLsizei stride = static_cast<GLsizei>((3 + 3) * typeSize);

  vao.linkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize));
  vao.linkAttrib(1, 3, GL_FLOAT, stride, (void*)(3 * typeSize));

  VAO::unbind();
  VBO::unbind();
}

template<>
Mesh<VertexPC>::Mesh(const std::vector<VertexPC>& vertices, const std::vector<mat4>& mats, GLenum mode, bool autoclear)
  : MeshBase(std::vector<GLuint>(), mode, autoclear),
    vertices(vertices),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(VertexPC) * vertices.size())),
    instancingVBO(1, mats.data(), sizeof(mat4) * mats.size()),
    instancingCount(mats.size())
{
  this->vertices.resize(vertices.size());
  this->vertices.reserve(vertices.size());
  vao.bind();
  vbo.bind();

  vao.linkAttrib(0, 3, GL_FLOAT, sizeof(VertexPC), (void*)(0));
  vao.linkAttrib(1, 3, GL_FLOAT, sizeof(VertexPC), (void*)(sizeof(vec3)));

  instancingVBO.bind();

  vao.linkAttrib(2, 4, GL_FLOAT, sizeof(mat4), (void*)(0 * sizeof(vec4)));
  vao.linkAttrib(3, 4, GL_FLOAT, sizeof(mat4), (void*)(1 * sizeof(vec4)));
  vao.linkAttrib(4, 4, GL_FLOAT, sizeof(mat4), (void*)(2 * sizeof(vec4)));
  vao.linkAttrib(5, 4, GL_FLOAT, sizeof(mat4), (void*)(3 * sizeof(vec4)));

  glVertexAttribDivisor(2, 1);
  glVertexAttribDivisor(3, 1);
  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);

  VAO::unbind();
  VBO::unbind();
}

