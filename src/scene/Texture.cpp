#include "Texture.h"

namespace pepcy::renderer {

Texture::Texture(const gm::Color &color) {
    SetColor(color);
}

Texture::Texture(const std::string &path, bool gamma) {
    SetPath(path, gamma);
}

void Texture::SetColor(const gm::Color &color) {
    col = color;
    is_color = true;
}

void Texture::SetPath(const std::string &path, bool gamma) {
    img = Img { path, gamma };
    is_color = false;
}

void Texture::Switch() {
    is_color = !is_color;
}

bool Texture::IsColor() const {
    return is_color;
}

gm::Color Texture::GetColor() const {
    return col;
}

std::string Texture::GetPath() const {
    return img.path;
}

bool Texture::NeedGamma() const {
    return img.gamma;
}

}