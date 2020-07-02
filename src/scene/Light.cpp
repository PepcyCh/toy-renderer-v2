#include "Light.h"

namespace pepcy::renderer {

Light::Light(const gm::Color &color) : color(color), id(gid::NewGID()) {}

gid::GID Light::GetID() const {
    return id;
}

DirectionalLight::DirectionalLight(const gm::Color &color,
    const gm::Vector3 &dir) : Light(color), dir(dir) {}

Camera DirectionalLight::GetCamera(float size, float near, float far) const {
    float cos = gm::Dot(gm::Vector3(0, -1, 0), dir);
    cos *= cos;
    float tan = sqrt((1.0f - cos) / cos);
    float len = far - size * tan;
    gm::Vector3 pos = gm::Vector3(0, 0, 0) - dir * len;
    return Camera(pos, gm::Vector3(0, 0, 0), gm::Vector3(0, 1, 0),
        2 * atan2(size, near), 1.0f, near, far, false);
}

PointLight::PointLight(const gm::Color &color, const gm::Vector3 &pos,
    float kc, float kl, float kq) :
    Light(color), pos(pos), kc(kc), kl(kl), kq(kq) {}

float PointLight::GetAtten(float dist) const {
    return 1.0f / (kc + kl * dist + kq * dist * dist);
}

SpotLight::SpotLight(const gm::Color &color, const gm::Vector3 &pos,
    const gm::Vector3 &dir,
    float kc, float kl, float kq, float cutoff, float outer_cutoff) :
    Light(color), pos(pos), dir(dir), kc(kc), kl(kl), kq(kq),
    cutoff(cutoff), outer_cutoff(outer_cutoff) {}

Camera SpotLight::GetCamera(float near, float far) const {
    return Camera(pos, pos + dir, gm::Vector3(0, 1, 0),
        gm::Radians(outer_cutoff), 1.0f, near, far);
}

float SpotLight::GetAtten(float dist, float theta) const {
    if (theta > outer_cutoff) {
        return 0.0f;
    }
    float atten = std::min(1.0f, (outer_cutoff - theta) / (outer_cutoff - cutoff));
    return atten / (kc + kl * dist + kq * dist * dist);
}

}