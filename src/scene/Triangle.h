#pragma once

#include "Mesh.h"

namespace pepcy::renderer {

class Triangle : public Primitive {
  public:
    Triangle(const Shape *sh, int v0, int v1, int v2);

    bool Intersect(const gm::Ray &r) const override;
    bool Intersect(const gm::Ray &r, Intersection &inter) const override;
    gm::BBox GetBBox() const override;

    const Material &GetMaterial() const;

  private:
    const Shape *sh;
    int v0, v1, v2;
};

}