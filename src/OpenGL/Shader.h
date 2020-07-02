#pragma once

#include <string>

#include "geomath.h"

namespace pepcy::renderer {

class Shader {
  public:
    Shader();

    void Attach(unsigned int sh_id) const;
    bool Link() const;

    void Use() const;

    void SetInt(const std::string &name, int value) const;
    void SetFloat(const std::string &name, float value) const;
    void SetVec3(const std::string &name, const gm::Vector3 &v) const;
    void SetVec3(const std::string &name, const gm::Color &c) const;
    void SetVec4(const std::string &name, const gm::Vector4 &v) const;
    void SetVec4(const std::string &name, const gm::Color &c) const;
    void SetMat3(const std::string &name, const gm::Matrix3 &m) const;
    void SetMat4(const std::string &name, const gm::Matrix4 &m) const;

  private:
    unsigned int id;
};

}