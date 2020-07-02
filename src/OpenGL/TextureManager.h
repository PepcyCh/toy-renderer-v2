#pragma once

#include <string>
#include <unordered_map>

namespace pepcy::renderer {

class TextureManager {
  public:
    ~TextureManager();

    unsigned int LoadTexture(const std::string &path, bool gamma = false);
    unsigned int LoadCubemap(const std::string &path, const std::string &suffix,
        bool gamma = false);
    unsigned int LoadHDR(const std::string &path);

  private:
    std::unordered_map<std::string, unsigned int> texture;
    std::unordered_map<std::string, unsigned int> cube_map;
};

}