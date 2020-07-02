#pragma once

#include "vector.h"
#include "ray.h"

namespace pepcy::gm {

struct BBox {
    BBox() : p_min(Vector3()), p_max(Vector3()) {}
    BBox(const Vector3 &p_min, const Vector3 &p_max) :
        p_min(p_min), p_max(p_max) {}

    bool Contains(const Vector3 &point) const {
        return p_min[0] <= point[0] && point[0] <= p_max[0] &&
               p_min[1] <= point[1] && point[1] <= p_max[1] &&
               p_min[2] <= point[2] && point[2] <= p_max[2];
    }

    void Expand(const BBox &rhs) {
        p_min = Min(p_min, rhs.p_min);
        p_max = Max(p_max, rhs.p_max);
    }
    friend BBox Combine(const BBox &a, const BBox &b) {
        return BBox(Min(a.p_min, b.p_min), Max(a.p_max, b.p_max));
    }

    bool Intersect(const Ray &r, float &t0, float &t1) const {
        t0 = r.t_min;
        t1 = r.t_max;
        for (int d = 0; d < 3; d++) {
            float tt0 = std::min((p_min[d] - r.orig[d]) / r.dir[d], (p_max[d] - r.orig[d]) / r.dir[d]);
            float tt1 = std::max((p_min[d] - r.orig[d]) / r.dir[d], (p_max[d] - r.orig[d]) / r.dir[d]);
            t0 = std::max(t0, tt0);
            t1 = std::min(t1, tt1);
            if (t0 > t1) return false;
        }
        return t0 < r.t_max;
    }

    float SurfaceArea() const {
        return (p_max[0] - p_min[0]) * (p_max[1] - p_min[1]) * (p_max[2] - p_min[2]);
    }
    Vector3 Centroid() const {
        return (p_min + p_max) / 2.0f;
    }

    Vector3 p_min, p_max;

    friend class Transform;
};

}