#pragma once

#include "vector.h"

namespace pepcy::gm {

struct Ray {
    Ray(const Vector3 &orig, const Vector3 &dir) : orig(orig), dir(Normalize(dir)),
        t_min(0.001f), t_max(100.0f) {}

    Vector3 orig, dir;
    float t_min;
    mutable float t_max;
};

}