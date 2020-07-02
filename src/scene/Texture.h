#pragma once

#include <string>
#include <variant>

#include "geomath.h"

namespace pepcy::renderer {

class Texture {
  public:
    Texture(const gm::Color &color = gm::Color(0.0f, 0.0f, 0.0f, 1.0f));
    Texture(const std::string &path, bool gamma = false);

    void SetColor(const gm::Color &color);
    void SetPath(const std::string &path, bool gamma = false);
    void Switch();

    bool IsColor() const;
    gm::Color GetColor() const;
    std::string GetPath() const;
    bool NeedGamma() const;

  private:
    struct Img { std::string path; bool gamma; } img;
    gm::Color col;
    bool is_color = true;
};

}