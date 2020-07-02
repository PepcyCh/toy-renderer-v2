#include "Material.h"

namespace pepcy::renderer {

Material::Material(const std::unordered_map<std::string, Texture> &textures) :
    textures(textures) {}

void Material::AddTexture(const std::string &type, const Texture &tex) {
    textures[type] = tex;
}

bool Material::HasTexture(const std::string &type) const {
    return textures.count(type) != 0;
}

Texture Material::GetTexture(const std::string &type) const {
    return textures.count(type) == 0 ? Texture() : textures.at(type);
}

const std::unordered_map<std::string, Texture> &Material::GetTextures() const {
    return textures;
}
std::unordered_map<std::string, Texture> &Material::GetTextures() {
    return textures;
}

void Material::CheckPhong() {
    if (textures.count("albedo") == 0) {
        textures["albedo"] = Texture(gm::Color(1.0f, 1.0f, 1.0f));
    }
    if (textures.count("specular") == 0) {
        textures["specular"] = Texture(gm::Color(1.0f, 1.0f, 1.0f));
    }
    if (textures.count("ambient") == 0) {
        textures["ambient"] = Texture(gm::Color(0.1f, 0.1f, 0.1f));
    }
    if (textures.count("exponent") == 0) {
        textures["exponent"] = Texture(gm::Color(1.0f, 1.0f, 1.0f));
    }
    if (textures.count("normal") == 0) {
        textures["normal"] = Texture(gm::Color(0.5f, 0.5f, 1.0f));
    }
}

void Material::CheckTorrance() {
    if (textures.count("albedo") == 0) {
        textures["albedo"] = Texture(gm::Color(1.0f, 1.0f, 1.0f));
    }
    if (textures.count("ambient") == 0) {
        textures["ambient"] = Texture(gm::Color(1.0f, 1.0f, 1.0f));
    }
    if (textures.count("roughness") == 0) {
        textures["roughness"] = Texture(gm::Color(1.0f, 1.0f, 1.0f));
    }
    if (textures.count("metallic") == 0) {
        textures["metallic"] = Texture(gm::Color(0.0f, 0.0f, 0.0f));
    }
}

Material Material::BasicPhongMat() {
    static Material mat;
    static bool init = false;
    if (!init) {
        mat.CheckPhong();
    }
    return mat;
}
Material Material::BasicTorranceMat() {
    static Material mat;
    static bool init = false;
    if (!init) {
        mat.CheckTorrance();
    }
    return mat;
}

}