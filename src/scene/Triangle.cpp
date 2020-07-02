#include "Triangle.h"

namespace pepcy::renderer {

Triangle::Triangle(const Shape *sh, int v0, int v1, int v2) :
        sh(sh), v0(v0), v1(v1), v2(v2) {}

static bool DoesRayIntersectSegment(const gm::Ray &r, const gm::Vector3 &v0,
        const gm::Vector3 &v1, float &t) {
    gm::Vector3 u = v1 - v0;
    gm::Vector3 v = gm::Cross(u, r.dir);
    if (v.Norm2() == 0) {
        return false;
    }
    gm::Vector3 w = gm::Cross(u, v);
    float c = gm::Dot(w, v0);
    float temp = gm::Dot(w, r.dir);
    if (temp == 0) {
        return false;
    }
    t = (c - gm::Dot(w, r.orig)) / temp;
    gm::Vector3 p = r.orig + r.dir * t;
    float ut = gm::Dot(p - v0, u) / u.Norm();
    return ut >= 0.0f && ut <= 1.0f;
}

bool Triangle::Intersect(const gm::Ray &r_) const {
    auto trans = sh->GetModel();
    gm::Ray r = trans.InvTransformRay(r_);

    gm::Vector3 p0 = sh->GetPosition(v0);
    gm::Vector3 p1 = sh->GetPosition(v1);
    gm::Vector3 p2 = sh->GetPosition(v2);
    gm::Vector3 e1 = p1 - p0;
    gm::Vector3 e2 = p2 - p0;
    gm::Vector3 s = r.orig - p0;

    float det = gm::Dot(gm::Cross(e1, r.dir), e2);
    // float det = gm::Mix(e2, e1, r.dir);
    if (det != 0) {
        float du = -gm::Dot(gm::Cross(s, e2), r.dir);
        float dv = gm::Dot(gm::Cross(e1, r.dir), s);
        float dt = -gm::Dot(gm::Cross(s, e2), e1);
        float u = du / det;
        float v = dv / det;
        float t = dt / det;
        if (u < 0 || v < 0 || 1 - u - v < 0) {
            return false;
        } else {
            return !(t < r.t_min || t > r.t_max);
        }
    } else {
        float t;
        if (DoesRayIntersectSegment(r, p0, p1, t)) {
            return !(t < r.t_min || t > r.t_max);
        } else if (DoesRayIntersectSegment(r, p0, p2, t)) {
            return !(t < r.t_min || t > r.t_max);
        } else if (DoesRayIntersectSegment(r, p1, p2, t)) {
            return !(t < r.t_min || t > r.t_max);
        }
    }

    return false;
}

bool Triangle::Intersect(const gm::Ray &r_, Intersection &inter) const {
    auto trans = sh->GetModel();
    gm::Ray r = trans.InvTransformRay(r_);
    
    gm::Vector3 p0 = sh->GetPosition(v0);
    gm::Vector3 p1 = sh->GetPosition(v1);
    gm::Vector3 p2 = sh->GetPosition(v2);
    gm::Vector3 n0 = sh->GetNormal(v0);
    gm::Vector3 n1 = sh->GetNormal(v1);
    gm::Vector3 n2 = sh->GetNormal(v2);
    gm::Vector3 tan0 = sh->GetTangent(v0);
    gm::Vector3 tan1 = sh->GetTangent(v1);
    gm::Vector3 tan2 = sh->GetTangent(v2);
    gm::Vector3 e1 = p1 - p0;
    gm::Vector3 e2 = p2 - p0;
    gm::Vector3 s = r.orig - p0;

    float det = gm::Dot(gm::Cross(e1, r.dir), e2);
    if (det != 0) {
        float du = -gm::Dot(gm::Cross(s, e2), r.dir);
        float dv = gm::Dot(gm::Cross(e1, r.dir), s);
        float dt = -gm::Dot(gm::Cross(s, e2), e1);
        float u = du / det;
        float v = dv / det;
        float t = dt / det;
        if (u < 0 || v < 0 || 1 - u - v < 0) {
            return false;
        } else if (t < r.t_min || t > r.t_max) {
            return false;
        }
        inter.norm = n0 * u + n1 * v + n2 * (1 - u - v);
        inter.tan = tan0 * u + tan1 * v + tan2 * (1 - u - v);
        if (gm::Dot(inter.norm, r.dir) > 0) {
            inter.norm = -inter.norm;
            inter.tan = -inter.tan;
        }
        r.t_max = t;
        inter.prim = this;
        r = trans.TransformRay(r);
        inter.TransformedBy(trans);
        inter.t = r_.t_max = r.t_max;
        return true;
    } else {
        gm::Matrix3 inv(p0, p1, p2);
        inv = gm::Inverse(inv);
        bool flag = false;
        float t, u, v;
        if (DoesRayIntersectSegment(r, p0, p1, t)) {
            if (t < r.t_min || t > r.t_max) {
                return false;
            }
            gm::Vector3 ret = inv * (r.orig + r.dir * t);
            u = ret[0];
            v = ret[1];
            flag = true;
        } else if (DoesRayIntersectSegment(r, p0, p2, t)) {
            if (t < r.t_min || t > r.t_max) {
                return false;
            }
            gm::Vector3 ret = inv * (r.orig + r.dir * t);
            u = ret[0];
            v = ret[1];
            flag = true;
        } else if (DoesRayIntersectSegment(r, p1, p2, t)) {
            if (t < r.t_min || t > r.t_max) {
                return false;
            }
            gm::Vector3 ret = inv * (r.orig + r.dir * t);
            u = ret[0];
            v = ret[1];
            flag = true;
        }
        if (!flag) {
            return false;
        }
        inter.norm = n0 * u + n1 * v + n2 * (1 - u - v);
        inter.tan = tan0 * u + tan1 * v + tan2 * (1 - u - v);
        if (gm::Dot(inter.norm, r.dir) > 0) {
            inter.norm = -inter.norm;
            inter.tan = -inter.tan;
        }
        r.t_max = t;
        inter.prim = this;
        inter.TransformedBy(trans);
        r = trans.TransformRay(r);
        inter.t = r_.t_max = r.t_max;
        return true;
    }
    return false;
}

gm::BBox Triangle::GetBBox() const {
    gm::Vector3 p0 = sh->GetPosition(v0);
    gm::Vector3 p1 = sh->GetPosition(v1);
    gm::Vector3 p2 = sh->GetPosition(v2);
    gm::Vector3 pmin = gm::Min(p0, gm::Min(p1, p2));
    gm::Vector3 pmax = gm::Max(p0, gm::Max(p1, p2));
    pmin -= gm::Vector3(0.001f, 0.001f, 0.001f);
    pmax += gm::Vector3(0.001f, 0.001f, 0.001f);
    return sh->GetModel().TransformBBox(gm::BBox(pmin, pmax));
}

const Material &Triangle::GetMaterial() const {
    return sh->GetMaterial();
}

}