#pragma once

#include "util.h"
#include "bbox.h"
#include "ray.h"

namespace pepcy::gm {

class Transform {
  public:
    Transform() : mat(gm::Matrix4(1.0f)), inv_t(gm::Matrix4(1.0f)) {}
    Transform(const Matrix4 &mat) : mat(mat), inv_t(Transpose(Inverse(mat))) {}
    Transform(const Matrix4 &mat, const Matrix4 &inv_t) :
        mat(mat), inv_t(inv_t) {}

    Matrix4 GetMatrix() const {
        return mat;
    }
    Matrix4 GetITMatrix() const {
        return inv_t;
    }

    Vector3 TransformVector(const Vector3 &vec) const {
        return Vector3(mat * Vector4(vec, 0.0f));
    }
    Vector3 TransformPoint(const Vector3 &point) const {
        return Vector3(mat * Vector4(point, 1.0f));
    }
    Vector3 TransformNormal(const Vector3 &norm) const {
        return Vector3(inv_t * Vector4(norm, 0.0f));
    }
    BBox TransformBBox(const BBox &bbox) const {
        Vector3 p_min = bbox.p_min;
        Vector3 p_max = bbox.p_max;
        Vector3 p1 = TransformPoint(p_min);
        Vector3 p2 = TransformPoint(p_max);
        Vector3 p3 = TransformPoint(Vector3(p_min[0], p_min[1], p_max[2]));
        Vector3 p4 = TransformPoint(Vector3(p_min[0], p_max[1], p_min[2]));
        Vector3 p5 = TransformPoint(Vector3(p_max[0], p_min[1], p_min[2]));
        Vector3 p6 = TransformPoint(Vector3(p_min[0], p_max[1], p_max[2]));
        Vector3 p7 = TransformPoint(Vector3(p_max[0], p_min[1], p_max[2]));
        Vector3 p8 = TransformPoint(Vector3(p_max[0], p_max[1], p_min[2]));
        p_min = Min(Min(Min(p1, p2), Min(p3, p4)),
                    Min(Min(p5, p6), Min(p7, p8)));
        p_max = Max(Max(Max(p1, p2), Max(p3, p4)),
                    Max(Max(p5, p6), Max(p7, p8)));
        return BBox(p_min, p_max);
    }
    Ray TransformRay(const Ray &r) const {
        Vector3 orig = TransformPoint(r.orig);
        Vector3 dir = TransformVector(r.dir);
        Ray ret(orig, dir);
        float dir_len = dir.Norm();
        ret.t_min = r.t_min * dir_len;
        ret.t_max = r.t_max * dir_len;
        return ret;
    }
    Ray InvTransformRay(const Ray &r) const {
        auto inv = Transpose(inv_t);
        Vector3 orig = Vector3(inv * Vector4(r.orig, 1.0f));
        Vector3 dir = Vector3(inv * Vector4(r.dir, 0.0f));
        Ray ret(orig, dir);
        float dir_len = dir.Norm();
        ret.t_min = r.t_min * dir_len;
        ret.t_max = r.t_max * dir_len;
        return ret;
    }

    bool operator==(const Transform &rhs) const {
        return mat == rhs.mat && inv_t == rhs.inv_t;
    }
    bool operator!=(const Transform &rhs) const {
        return !(*this == rhs);
    }

    Transform operator*(const Transform &rhs) const {
        return Transform(mat * rhs.mat, inv_t * rhs.inv_t);
    }
    Transform &operator*=(const Transform &rhs) {
        mat *= rhs.mat;
        inv_t *= rhs.inv_t;
        return *this;
    }

    friend Transform Transpose(const Transform &trans) {
        return Transform(Transpose(trans.mat), Transpose(trans.inv_t));
    }
    friend Transform Inverse(const Transform &trans) {
        return Transform(Transpose(trans.inv_t), Transpose(trans.mat));
    }

    friend std::ostream &operator<<(std::ostream &out, const Transform &trans) {
        return out << "{ mat: " << trans.mat << ", inv_t: " << trans.inv_t << " }";
    }

  private:
    Matrix4 mat, inv_t;
};

}
