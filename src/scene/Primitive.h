#pragma once

#include "geomath.h"
#include "Intersection.h"

namespace pepcy::renderer {

struct Intersection;

class Primitive {
  public:
    virtual bool Intersect(const gm::Ray &r) const = 0;
    virtual bool Intersect(const gm::Ray &r, Intersection &inter) const = 0;
    virtual gm::BBox GetBBox() const = 0;
};

}