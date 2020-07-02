#pragma once

#include <vector>

#include "Primitive.h"
#include "Material.h"
#include "GID.h"

namespace pepcy::renderer {

class Shape : public Primitive {
  public:
    Shape() : model() {}
    Shape(const std::vector<unsigned int> &indices,
        const std::vector<gm::Vector3> &posistions,
        const std::vector<gm::Vector3> &normals,
        const std::vector<gm::Vector2> &texcoords,
        const std::vector<gm::Vector3> &tangents = std::vector<gm::Vector3>());

    const float *GetPositions() const;
    gm::Vector3 GetPosition(int i) const;
    const float *GetNormals() const;
    gm::Vector3 GetNormal(int i) const;
    const float *GetTexcoords() const;
    gm::Vector2 GetTexcoord(int i) const;
    const float *GetTangents() const;
    gm::Vector3 GetTangent(int i) const;
    const float *GetBitangents() const;
    gm::Vector3 GetBitangent(int i) const;
    const unsigned int *GetIndices() const;
    int GetVertexCount() const;
    int GetIndexCount() const;

    gm::Transform GetModel() const;
    void SetModel(const gm::Transform &trans);
    gm::BBox GetBBox() const override;
    gid::GID GetID() const;
    gm::Vector3 GetCentroid() const;

    const Material &GetMaterial() const;
    Material &GetMaterial();
    void SetMaterial(const Material &mat);

    bool Intersect(const gm::Ray &r) const override;
    bool Intersect(const gm::Ray &r, Intersection &inter) const override;

  protected:
    gm::Transform model;
    mutable gm::BBox bbox;
    mutable bool bbox_valid;
    mutable gm::Vector3 centroid;
    mutable bool centroid_valid = false;
    gid::GID id;
    Material mat;

    std::vector<unsigned> indices;
    std::vector<gm::Vector3> positions;
    std::vector<gm::Vector3> normals;
    std::vector<gm::Vector2> texcoords;
    std::vector<gm::Vector3> tangents;
    std::vector<gm::Vector3> bitangents;
};

}