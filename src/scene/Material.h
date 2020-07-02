#pragma once

#include <unordered_map>

#include "Texture.h"

namespace pepcy::renderer {

class Material {
  public:
    Material() = default;
    Material(const std::unordered_map<std::string, Texture> &textures);

    void AddTexture(const std::string &type, const Texture &tex);
    bool HasTexture(const std::string &type) const;
    Texture GetTexture(const std::string &type) const;
    const std::unordered_map<std::string, Texture> &GetTextures() const;
    std::unordered_map<std::string, Texture> &GetTextures();

    static Material BasicPhongMat();
    static Material BasicTorranceMat();

    void CheckPhong();
    void CheckTorrance();

  private:
    std::unordered_map<std::string, Texture> textures;
};

}