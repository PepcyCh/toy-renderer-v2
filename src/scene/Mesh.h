#pragma once

#include <memory>

#include "Material.h"
#include "Shape.h"

namespace pepcy::renderer {

class Mesh {
  public:
    Mesh(const std::shared_ptr<Shape> &shape, const Material &mat) :
        shape(shape), mat(mat), model() {}

    const std::shared_ptr<Shape> &GetShape() const;
    const Material &GetMaterial() const;
    Material &GetMaterial();
    gm::Transform GetModel() const;
    gm::Vector3 GetCentroid() const;

    void SetMaterial(const Material &mat);
    void SetModel(const gm::Transform &trans);
    void ApplyTransform();

  private:
    std::shared_ptr<Shape> shape;
    Material mat;
    gm::Transform model;
};

}