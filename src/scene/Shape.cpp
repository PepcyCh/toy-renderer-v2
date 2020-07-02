#include "Shape.h"

#include "Triangle.h"

namespace pepcy::renderer {

Shape::Shape(const std::vector<unsigned int> &indices,
        const std::vector<gm::Vector3> &positions,
        const std::vector<gm::Vector3> &normals,
        const std::vector<gm::Vector2> &texcoords,
        const std::vector<gm::Vector3> &tangents) :
        indices(indices), positions(positions), normals(normals),
        texcoords(texcoords), tangents(tangents), model() {
    int N = positions.size();

    if (!tangents.empty()) {
        bitangents.resize(N);
        for (int i = 0; i < N; i++) {
            bitangents[i] = gm::Cross(normals[i], tangents[i]);
        }
    }

    gm::Vector3 p_min = positions[0], p_max = positions[0];
    for (int i = 1; i < N; i++) {
        p_min = gm::Min(p_min, positions[i]);
        p_max = gm::Max(p_max, positions[i]);
    }
    bbox = gm::BBox(p_min, p_max);
    bbox_valid = true;

    id = gid::NewGID();
}

const float *Shape::GetPositions() const {
    return positions[0].Data();
}
gm::Vector3 Shape::GetPosition(int i) const {
    return positions[i];
}
const float *Shape::GetNormals() const {
    return normals[0].Data();
}
gm::Vector3 Shape::GetNormal(int i) const {
    return normals[i];
}
const float *Shape::GetTexcoords() const {
    return texcoords[0].Data();
}
gm::Vector2 Shape::GetTexcoord(int i) const {
    return texcoords[i];
}
const float *Shape::GetTangents() const {
    return tangents[0].Data();
}
gm::Vector3 Shape::GetTangent(int i) const {
    return tangents[i];
}
const float *Shape::GetBitangents() const {
    return bitangents[0].Data();
}
gm::Vector3 Shape::GetBitangent(int i) const {
    return bitangents[i];
}
const unsigned int *Shape::GetIndices() const {
    return indices.data();
}

int Shape::GetVertexCount() const {
    return positions.size();
}
int Shape::GetIndexCount() const {
    return indices.size();
}

gm::BBox Shape::GetBBox() const {
    if (!bbox_valid) {
        int N = positions.size();
        gm::Vector3 p_min = positions[0], p_max = positions[0];
        for (int i = 1; i < N; i++) {
            p_min = gm::Min(p_min, positions[i]);
            p_max = gm::Max(p_max, positions[i]);
        }
        bbox = gm::BBox(p_min, p_max);
        bbox_valid = true;
    }
    return model.TransformBBox(bbox);
}

gm::Transform Shape::GetModel() const {
    return model;
}
void Shape::SetModel(const gm::Transform &trans) {
    model = trans;
}

gid::GID Shape::GetID() const {
    return id;
}

gm::Vector3 Shape::GetCentroid() const {
    if (!centroid_valid) {
        centroid = gm::Vector3(0.0f);
        for (const auto &v : positions) {
            centroid += v;
        }
        centroid /= positions.size();
        centroid_valid = true;
    }
    return model.TransformPoint(centroid);
}

const Material &Shape::GetMaterial() const {
    return mat;
}
Material &Shape::GetMaterial() {
    return mat;
}
void Shape::SetMaterial(const Material &mat) {
    this->mat = mat;
}

bool Shape::Intersect(const gm::Ray &r) const {
    int M = indices.size();
    for (int i = 0; i < M; i += 3) {
        if (Triangle(this, indices[i], indices[i + 1], indices[i + 2]).Intersect(r)) {
            return true;
        }
    }
    return false;
}
bool Shape::Intersect(const gm::Ray &r, Intersection &inter) const {
    int M = indices.size();
    for (int i = 0; i < M; i += 3) {
        if (Triangle(this, indices[i], indices[i + 1], indices[i + 2])
                .Intersect(r, inter)) {
            return true;
        }
    }
    return false;
}

}