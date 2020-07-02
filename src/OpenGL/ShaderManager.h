#pragma once

#include <unordered_map>
#include <string>

namespace pepcy::renderer {

class ShaderManager {
  public:
    unsigned int VertexShader(const std::string &path);
    unsigned int FragmentShader(const std::string &path);
    unsigned int GeometryShader(const std::string &path);

  private:
    std::unordered_map<std::string, unsigned int> vert;
    std::unordered_map<std::string, unsigned int> frag;
    std::unordered_map<std::string, unsigned int> geom;
};

}