#pragma once

#include <limits>

#include "matrix.h"

namespace pepcy::gm {

const float PI = 3.141592653589793238463f;
const float PI_INV = 0.3183098861837907f;

inline float Radians(float degree) {
    return degree / 180 * PI;
}
inline double Radians(double degree) {
    return degree / 180 * PI;
}
inline float Degree(float radians) {
    return radians / PI * 180;
}
inline double Degree(double radians) {
    return radians / PI * 180;
}

inline float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}
inline double Lerp(double a, double b, double t) {
    return a + (b - a) * t;
}

inline Matrix4 Translate(const Vector3 &v) {
    return Matrix4(1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f,
                   v[0], v[1], v[2], 1.0f);
}
inline Matrix4 Translate(float x, float y, float z) {
    return Matrix4(1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f,
                   x, y, z, 1.0f);
}

inline Matrix4 Scale(const Vector3 &v) {
    return Matrix4(v[0], 0.0f, 0.0f, 0.0f,
                   0.0f, v[1], 0.0f, 0.0f,
                   0.0f, 0.0f, v[2], 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f);
}
inline Matrix4 Scale(float x, float y, float z) {
    return Matrix4(x, 0.0f, 0.0f, 0.0f,
                   0.0f, y, 0.0f, 0.0f,
                   0.0f, 0.0f, z, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f);
}

inline Matrix4 RotateX(float angle) {
   float c = std::cos(angle), s = std::sin(angle);
   return Matrix4(1.0f, 0.0f, 0.0f, 0.0f,
                  0.0f, c, s, 0.0f,
                  0.0f, -s, c, 0.0f,
                  0.0f, 0.0f, 0.0f, 1.0f);
}
inline Matrix4 RotateY(float angle) {
    float c = std::cos(angle), s = std::sin(angle);
    return Matrix4(c, 0.0f, -s, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   s, 0.0f, c, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f);
}
inline Matrix4 RotateZ(float angle) {
    float c = std::cos(angle), s = std::sin(angle);
    return Matrix4(c, s, 0.0f, 0.0f,
                   -s, c, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f);
}
inline Matrix4 Rotate(const Vector3 &axis, float angle) {
    Vector3 a = Normalize(axis);
    float c = std::cos(angle), s = std::sin(angle), mc = 1.0f - c;
    return Matrix4(c + a[0] * a[0] * mc, a[0] * a[1] * mc + a[2] * s,
                       a[2] * a[0] * mc - a[1] * s, 0.0f,
                   a[0] * a[1] * mc - a[2] * s, c + a[1] * a[1] * mc,
                       a[1] * a[2] * mc + a[0] * s, 0.0f,
                   a[2] * a[0] * mc + a[1] * s, a[1] * a[2] * mc - a[0] * s,
                       c + a[2] * a[2] * mc, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f);
}

inline Matrix4
LookAt(const Vector3 &pos, const Vector3 &look, const Vector3 &up) {
    Vector3 w = Normalize(pos - look);
    Vector3 u = Normalize(Cross(up, w));
    Vector3 v = Cross(w, u);
    return Matrix4(u[0], v[0], w[0], 0.0f,
                   u[1], v[1], w[1], 0.0f,
                   u[2], v[2], w[2], 0.0f,
                   -Dot(u, pos), -Dot(v, pos), -Dot(w, pos), 1.0f);
}

inline Matrix4 Perspective(float fov, float aspect, float n, float f) {
    float t = std::tan(fov * 0.5f), invz = 1.0f / (f - n);
    return Matrix4(1.0f / (aspect * t), 0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f / t, 0.0f, 0.0f,
                   0.0f, 0.0f, -(f + n) * invz, -1.0f,
                   0.0f, 0.0f, -2.0f * f * n * invz, 0.0f);
}

inline Matrix4
Orthographic(float l, float r, float b, float t, float n, float f) {
    float invw = 1.0f / (r - l);
    float invh = 1.0f / (t - b);
    float invd = 1.0f / (f - n);
    return Matrix4(2.0f * invw, 0.0f,  0.0f, 0.0f,
                   0.0f, 2.0f * invh,  0.0f, 0.0f,
                   0.0f, 0.0f, -2.0f * invd, 0.0f,
                   -(l + r) * invw, -(b + t) * invh, -(n + f) * invd, 1.0f);
}

}