#pragma once

#include "geomath.h"
#include "Primitive.h"

namespace pepcy::renderer {

class Primitive;

struct Intersection {
    float t;
    const Primitive *prim;
    gm::Vector3 norm, tan;

    void TransformedBy(const gm::Transform &trans);
};

}