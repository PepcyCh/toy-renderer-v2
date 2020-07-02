#include "ShaderManager.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "glad/glad.h"

static unsigned int LoadShader(const std::string &path, GLenum type) {
    std::ifstream fin(path);
    if (!fin) {
        std::cout << "can't open file '" << path << "'" << std::endl;
        return 0;
    }
    std::stringstream ss;
    ss << fin.rdbuf();
    std::string code = ss.str();
    fin.close();

    unsigned int id = glCreateShader(type);
    const char *p_code = code.c_str();
    glShaderSource(id, 1, &p_code, nullptr);
    glCompileShader(id);

    int ret;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ret);
    if (!ret) {
        char log_info[512];
        glGetShaderInfoLog(id, 512, nullptr, log_info);
        std::string sh_str;
        if (type == GL_VERTEX_SHADER) {
            sh_str = "vertex";
        } else if (type == GL_FRAGMENT_SHADER) {
            sh_str = "fragment";
        } else if (type == GL_GEOMETRY_SHADER) {
            sh_str = "geometry";
        }
        std::cout << "CE on " << sh_str << " shader: " << log_info << std::endl;
        glDeleteShader(id);
        id = 0;
    }

    return id;
}

namespace pepcy::renderer {

unsigned int ShaderManager::VertexShader(const std::string &path) {
    if (vert.count(path) != 0) {
        return vert[path];
    }
    unsigned int id = LoadShader(path, GL_VERTEX_SHADER);
    if (id != 0) {
        vert[path] = id;
    }
    return id;
}

unsigned int ShaderManager::FragmentShader(const std::string &path) {
    if (frag.count(path) != 0) {
        return frag[path];
    }
    unsigned int id = LoadShader(path, GL_FRAGMENT_SHADER);
    if (id != 0) {
        frag[path] = id;
    }
    return id;
}

unsigned int ShaderManager::GeometryShader(const std::string &path) {
    if (geom.count(path) != 0) {
        return geom[path];
    }
    unsigned int id = LoadShader(path, GL_GEOMETRY_SHADER);
    if (id != 0) {
        geom[path] = id;
    }
    return id;
}

}