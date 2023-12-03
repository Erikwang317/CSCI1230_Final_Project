#include "model.h"
#include <iostream>
#include <sstream>
#include <map>
#include <tinyxml2.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include "texture.h"

using namespace glm;
using namespace std;
using namespace ogl;
using namespace tinyxml2;


void loadOBJWithTiny(
    const string& path,
    vector<vec3>& vertices,
    vector<vec2>& uvs,
    vector<vec3>& normals,
    vector<unsigned int>& indices) {
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;

    string err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str())) {
        throw runtime_error(err);
    }

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            vec3 vertex = {
                           attrib.vertices[3 * index.vertex_index + 0],
                           attrib.vertices[3 * index.vertex_index + 1],
                           attrib.vertices[3 * index.vertex_index + 2]};
            if (attrib.texcoords.size() != 0) {
                vec2 uv = {
                           attrib.texcoords[2 * index.texcoord_index + 0],
                           1 - attrib.texcoords[2 * index.texcoord_index + 1]};
                uvs.push_back(uv);
            }
            if (attrib.normals.size() != 0) {
                vec3 normal = {
                               attrib.normals[3 * index.normal_index + 0],
                               attrib.normals[3 * index.normal_index + 1],
                               attrib.normals[3 * index.normal_index + 2]};
                normals.push_back(normal);
            }

            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
    }

    // TODO .mtl loader
}

struct PackedVertex {
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
    bool operator<(const PackedVertex that) const {
        return memcmp((void*) this, (void*) &that, sizeof(PackedVertex)) > 0;
    };
};

bool getSimilarVertexIndex(
    PackedVertex& packed,
    map<PackedVertex, unsigned int>& vertexToOutIndex,
    unsigned int& result) {
    map<PackedVertex, unsigned int>::iterator it = vertexToOutIndex.find(packed);
    if (it == vertexToOutIndex.end()) {
        return false;
    } else {
        result = it->second;
        return true;
    }
}

void indexVBO(
    const vector<vec3>& in_vertices,
    const vector<vec2>& in_uvs,
    const vector<vec3>& in_normals,
    vector<unsigned int>& out_indices,
    vector<vec3>& out_vertices,
    vector<vec2>& out_uvs,
    vector<vec3>& out_normals) {
    map<PackedVertex, unsigned int> vertexToOutIndex;

    // For each input vertex
    for (int i = 0; i < static_cast<int>(in_vertices.size()); i++) {
        vec3 vertices = in_vertices[i];
        vec2 uvs;
        vec3 normals;
        if (in_uvs.size() != 0) uvs = in_uvs[i];
        if (in_normals.size() != 0) normals = in_normals[i];
        PackedVertex packed = {vertices, uvs, normals};

        // Try to find a similar vertex in out_XXXX
        unsigned int index;
        bool found = getSimilarVertexIndex(packed, vertexToOutIndex, index);

        if (found) { // A similar vertex is already in the VBO, use it instead !
            out_indices.push_back(index);
        } else { // If not, it needs to be added in the output data.
            out_vertices.push_back(vertices);
            if (in_uvs.size() != 0) out_uvs.push_back(uvs);
            if (in_normals.size() != 0) out_normals.push_back(normals);
            unsigned int newindex = (unsigned int) out_vertices.size() - 1;
            out_indices.push_back(newindex);
            vertexToOutIndex[packed] = newindex;
        }
    }
}

Drawable::Drawable(string path) {
    if (path.substr(path.size() - 3, 3) == "obj") {
        loadOBJWithTiny(path.c_str(), vertices, uvs, normals, VEC_UINT_DEFAUTL_VALUE);
    } else {
        throw runtime_error("File format not supported: " + path);
    }

    createContext();
}

Drawable::Drawable(const vector<vec3>& vertices, const vector<vec2>& uvs,
                   const vector<vec3>& normals) : vertices(vertices), uvs(uvs), normals(normals) {
    createContext();
}

Drawable::~Drawable() {
    glDeleteBuffers(1, &verticesVBO);
    glDeleteBuffers(1, &uvsVBO);
    glDeleteBuffers(1, &normalsVBO);
    glDeleteBuffers(1, &elementVBO);
    glDeleteBuffers(1, &VAO);
}

void Drawable::bind() {
    glBindVertexArray(VAO);
}

void Drawable::draw(int mode) {
    glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, NULL);
}

void Drawable::createContext() {
    indices = vector<unsigned int>();
    indexVBO(vertices, uvs, normals, indices, indexedVertices, indexedUVS, indexedNormals);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &verticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, indexedVertices.size() * sizeof(vec3),
                 &indexedVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    if (indexedNormals.size() != 0) {
        glGenBuffers(1, &normalsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
        glBufferData(GL_ARRAY_BUFFER, indexedNormals.size() * sizeof(vec3),
                     &indexedNormals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
    }

    if (indexedUVS.size() != 0) {
        glGenBuffers(1, &uvsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, uvsVBO);
        glBufferData(GL_ARRAY_BUFFER, indexedUVS.size() * sizeof(vec2),
                     &indexedUVS[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
    }

    // Generate a buffer for the indices as well
    glGenBuffers(1, &elementVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);
}

/*****************************************************************************/

Mesh::Mesh(
    const vector<vec3>& vertices,
    const vector<vec2>& uvs,
    const vector<vec3>& normals,
    const Material& mtl)
    : vertices{vertices}, uvs{uvs}, normals{normals}, mtl{mtl} {
    createContext();
}

Mesh::Mesh(Mesh&& other)
    : vertices{std::move(other.vertices)}, normals{std::move(other.normals)},
    indexedVertices{std::move(other.indexedVertices)}, indexedNormals{std::move(other.indexedNormals)},
    uvs{std::move(other.uvs)}, indexedUVS{std::move(other.indexedUVS)},
    indices{std::move(other.indices)}, mtl{std::move(other.mtl)},
    VAO{other.VAO}, verticesVBO{other.verticesVBO}, normalsVBO{other.normalsVBO},
    uvsVBO{other.uvsVBO}, elementVBO{other.elementVBO} {
    other.VAO = 0;
    other.verticesVBO = 0;
    other.normalsVBO = 0;
    other.uvsVBO = 0;
    other.elementVBO = 0;
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &verticesVBO);
    glDeleteBuffers(1, &uvsVBO);
    glDeleteBuffers(1, &normalsVBO);
    glDeleteBuffers(1, &elementVBO);
    glDeleteVertexArrays(1, &VAO);
}

void Mesh::bind() {
    glBindVertexArray(VAO);
}

void Mesh::draw(int mode) {
    glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, NULL);
}

void Mesh::createContext() {
    indices = vector<unsigned int>();
    indexVBO(vertices, uvs, normals, indices, indexedVertices, indexedUVS, indexedNormals);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &verticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, indexedVertices.size() * sizeof(vec3),
                 &indexedVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    if (indexedNormals.size() != 0) {
        glGenBuffers(1, &normalsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
        glBufferData(GL_ARRAY_BUFFER, indexedNormals.size() * sizeof(vec3),
                     &indexedNormals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
    }

    if (indexedUVS.size() != 0) {
        glGenBuffers(1, &uvsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, uvsVBO);
        glBufferData(GL_ARRAY_BUFFER, indexedUVS.size() * sizeof(vec2),
                     &indexedUVS[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
    }

    // Generate a buffer for the indices as well
    glGenBuffers(1, &elementVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);
}

Model::Model(string path, Model::MTLUploadFunction* uploader)
    : uploadFunction{uploader} {
    if (path.substr(path.size() - 3, 3) == "obj") {
        loadOBJWithTiny(path.c_str());
    } else {
        throw runtime_error("File format not supported: " + path);
    }
}

Model::~Model() {
    for (const auto& t : textures) {
        glDeleteTextures(1, &t.second);
    }
}

void Model::draw() {
    for (auto& mesh : meshes) {
        mesh.bind();
        if (uploadFunction)
            uploadFunction(mesh.mtl);
        mesh.draw();
    }
}

void Model::loadOBJWithTiny(const std::string& filename) {
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;

    string err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str())) {
        throw runtime_error(err);
    }

    for (const auto& material : materials) {
        loadTexture(material.ambient_texname);
        loadTexture(material.diffuse_texname);
        loadTexture(material.specular_texname);
        loadTexture(material.specular_highlight_texname);
    }

    for (const auto& shape : shapes) {
        vector<vec3> vertices{};
        vector<vec2> uvs{};
        vector<vec3> normals{};
        for (const auto& index : shape.mesh.indices) {
            int vertex_index = index.vertex_index;
            if (vertex_index < 0) vertex_index += attrib.vertices.size() / 3;
            vec3 vertex = {
                           attrib.vertices[3 * vertex_index + 0],
                           attrib.vertices[3 * vertex_index + 1],
                           attrib.vertices[3 * vertex_index + 2]};
            if (attrib.texcoords.size() != 0) {
                int texcoord_index = index.texcoord_index;
                if (texcoord_index < 0) texcoord_index += attrib.texcoords.size() / 2;
                vec2 uv = {
                           attrib.texcoords[2 * texcoord_index + 0],
                           1 - attrib.texcoords[2 * texcoord_index + 1]};
                uvs.push_back(uv);
            }
            if (attrib.normals.size() != 0) {
                int normal_index = index.normal_index;
                if (normal_index < 0) normal_index += attrib.normals.size() / 3;
                vec3 normal = {
                               attrib.normals[3 * normal_index + 0],
                               attrib.normals[3 * normal_index + 1],
                               attrib.normals[3 * normal_index + 2]};
                normals.push_back(normal);
            }
            vertices.push_back(vertex);
        }
        Material mtl{};
        if (materials.size() > 0 && shape.mesh.material_ids.size() > 0) {
            int idx = shape.mesh.material_ids[0];
            if (idx < 0 || idx >= static_cast<int>(materials.size()))
                idx = static_cast<int>(materials.size()) - 1;
            tinyobj::material_t mat = materials[idx];
            mtl = {
                {mat.ambient[0], mat.ambient[1], mat.ambient[2], 1},
                {mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1},
                {mat.specular[0], mat.specular[1], mat.specular[2], 1},
                mat.shininess,
                textures[mat.ambient_texname],
                textures[mat.diffuse_texname],
                textures[mat.specular_texname],
                textures[mat.specular_highlight_texname]
            };
            if (mtl.texKa) mtl.Ka.r = -1.0f;
            if (mtl.texKd) mtl.Kd.r = -1.0f;
            if (mtl.texKs) mtl.Ks.r = -1.0f;
            if (mtl.texNs) mtl.Ns = -1.0f;
        }
        meshes.emplace_back(vertices, uvs, normals, mtl);
    }
}

void Model::loadTexture(const std::string& filename) {
    if (filename.length() == 0) return;
    if (textures.find(filename) == end(textures)) {
        GLuint id = loadSOIL(filename.c_str());
        if (!id) throw std::runtime_error("Failed to load texture: " + filename);
        textures[filename] = id;
    }
}
