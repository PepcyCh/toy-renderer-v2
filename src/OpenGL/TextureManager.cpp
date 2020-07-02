#include "TextureManager.h"

#include <iostream>

#include "glad/glad.h"

#include "stb_image.h"

namespace pepcy::renderer {

static void GetFormat(int nn, GLenum *&format, GLenum *&iformat, bool gamma) {
    if (nn == 1) {
        *format = GL_RED;
        *iformat = GL_RED;
    } else if (nn == 3) {
        *format = GL_RGB;
        *iformat = GL_SRGB;
    } else if (nn == 4) {
        *format = GL_RGBA;
        *iformat = GL_SRGB_ALPHA;
    } else {
        delete format;
        delete iformat;
        format = nullptr;
        iformat = nullptr;
    }
    if (!gamma) {
        *iformat = *format;
    }
}

TextureManager::~TextureManager() {
    for (const auto &[_, id] : texture) {
        glDeleteTextures(1, &id);
    }
    for (const auto &[_, id] : cube_map) {
        glDeleteTextures(1, &id);
    }
}

unsigned int TextureManager::LoadTexture(const std::string &path, bool gamma) {
    if (texture.count(path) != 0) {
        return texture[path];
    }

    int nx, ny, nn;
    unsigned char *data = stbi_load(path.c_str(), &nx, &ny, &nn, 0);
    if (data == nullptr) {
        std::cout << "Fail to load texture '" << path << "'" << std::endl;
        return 0;
    }

    GLenum *format = new GLenum, *iformat = new GLenum;
    GetFormat(nn, format, iformat, gamma);
    if (format == nullptr) {
        stbi_image_free(data);
        return 0;
    }

    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, *iformat, nx, ny, 0, *format,
        GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
    delete format;
    delete iformat;
    texture[path] = id;
    return id;
}

unsigned int TextureManager::LoadCubemap(const std::string &path,
        const std::string &suffix, bool gamma) {
    if (cube_map.count(path + suffix) != 0) {
        return cube_map[path + suffix];
    }

    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    for (int i = 0; i < 6; i++) {
        static const char *cubemap_name[6] = {
            "right", "left", "top",
            "bottom", "front", "back"
        };
        int nx, ny, nn;
        unsigned char *data = stbi_load(
                (path + cubemap_name[i] + suffix).c_str(), &nx, &ny, &nn, 0);
        if (data == nullptr) {
            std::cout << "Fail to load texture '" <<
                (path + cubemap_name[i] + suffix) << "'" << std::endl;
            return 0;
        }

        GLenum *format = new GLenum, *iformat = new GLenum;
        GetFormat(nn, format, iformat, gamma);
        if (format == nullptr) {
            stbi_image_free(data);
            glDeleteTextures(1, &id);
            return 0;
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, *iformat,
                nx, ny, 0, *format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        delete format;
        delete iformat;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    cube_map[path + suffix] = id;
    return id;
}

unsigned int TextureManager::LoadHDR(const std::string &path) {
    if (texture.count(path) != 0) {
        return texture[path];
    }

    int nx, ny, nn;
    stbi_set_flip_vertically_on_load(true);
    float *data = stbi_loadf(path.c_str(), &nx, &ny, &nn, 0);
    if (data == nullptr) {
        std::cout << "Fail to load texture '" << path << "'" << std::endl;
        return 0;
    }
    stbi_set_flip_vertically_on_load(false);

    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, nx, ny, 0, GL_RGB, GL_FLOAT, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
    texture[path] = id;
    return id;
}

}