#pragma once

#include "Shape.h"

namespace pepcy::renderer {

class Cube : public Shape {
  public:
    Cube();

  private:
    inline static gid::GID g_gid;
};

class Sphere : public Shape {
  public:
    Sphere();

    // bool Intersect(const gm::Ray &r) const override;
    // bool Intersect(const gm::Ray &r, Intersection &inter) const override;

  private:
    inline static gid::GID g_gid;
};

class Plane : public Shape {
  public:
    Plane();

  private:
    inline static gid::GID g_gid;
};

class ScreenQuad : public Shape {
  public:
    ScreenQuad();

  private:
    inline static gid::GID g_gid;
};

class Cylinder : public Shape {
  public:
    Cylinder();

  private:
    inline static gid::GID g_gid;
};

class Capsule : public Shape {
  public:
    Capsule(float radius = 1.0f, float height = 1.0f);

  private:
    float radius, height;
};

}