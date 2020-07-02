#pragma once

#include "geomath.h"
#include "GID.h"
#include "Camera.h"

namespace pepcy::renderer {

class Light {
  public:
    gid::GID GetID() const;

  protected:
    Light(const gm::Color &color);

  public:
    gm::Color color;
    gid::GID id;
    bool shadow = false;
    bool realtime = false;
};

class DirectionalLight : public Light {
  public:
    DirectionalLight(const gm::Color &color, const gm::Vector3 &dir);
    Camera GetCamera(float size = 10.0f, float near = 0.1f,
                     float far = 20.0f) const;

    gm::Vector3 dir;
};

class PointLight : public Light {
  public:
    PointLight(const gm::Color &color, const gm::Vector3 &pos,
               float kc = 0.0f, float kl = 0.0f, float kq = 1.0f);

    float GetAtten(float dist) const;

    gm::Vector3 pos;
    float kc, kl, kq;
};

class SpotLight : public Light {
  public:
    SpotLight(const gm::Color &color, const gm::Vector3 &pos, 
              const gm::Vector3 &dir, 
              float kc = 0.0f, float kl = 0.0f, float kq = 1.0f,
              float cutoff = gm::PI / 6, float outer_cutoff = gm::PI / 3);
    Camera GetCamera(float near = 0.1f, float far = 100.0f) const;

    float GetAtten(float dist, float theta) const;

    gm::Vector3 pos, dir;
    float kc, kl, kq;
    float cutoff, outer_cutoff;
};

}