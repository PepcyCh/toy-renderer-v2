#include "Shader.h"

#include <iostream>

#include "glad/glad.h"

namespace pepcy::renderer {

Shader::Shader() {
    id = glCreateProgram();
}

void Shader::Attach(unsigned int sh_id) const {
    glAttachShader(id, sh_id);
}

bool Shader::Link() const {
    glLinkProgram(id);

    int ret;
    glGetProgramiv(id, GL_LINK_STATUS, &ret);
    if (!ret) {
        char log_info[512];
        glGetProgramInfoLog(id, 512, nullptr, log_info);
        std::cout << "link error: " << log_info << std::endl;
        return false;
    }
    return true;
}

void Shader::Use() const {
    glUseProgram(id);
}

void Shader::SetInt(const std::string &name, int value) const {
    int loc = glGetUniformLocation(id, name.c_str());
    // if (loc == -1) std::cout << "can't find '" << name << "'" << std::endl;
    glUniform1i(loc, value);
}

void Shader::SetFloat(const std::string &name, float value) const {
    int loc = glGetUniformLocation(id, name.c_str());
    // if (loc == -1) std::cout << "can't find '" << name << "'" << std::endl;
    glUniform1f(loc, value);
}

void Shader::SetVec3(const std::string &name, const gm::Vector3 &v) const {
    int loc = glGetUniformLocation(id, name.c_str());
    // std::cout << "set '" << name << "' to " << v << std::endl;
    // if (loc == -1) std::cout << "can't find '" << name << "'" << std::endl;
    glUniform3fv(loc, 1, v.Data());
}

void Shader::SetVec3(const std::string &name, const gm::Color &c) const {
    int loc = glGetUniformLocation(id, name.c_str());
    // std::cout << "set '" << name << "' to " << c << std::endl;
    // if (loc == -1) std::cout << "can't find '" << name << "'" << std::endl;
    glUniform3f(loc, c.r, c.g, c.b);
}

void Shader::SetVec4(const std::string &name, const gm::Vector4 &v) const {
    int loc = glGetUniformLocation(id, name.c_str());
    // if (loc == -1) std::cout << "can't find '" << name << "'" << std::endl;
    glUniform4fv(loc, 1, v.Data());
}

void Shader::SetVec4(const std::string &name, const gm::Color &c) const {
    int loc = glGetUniformLocation(id, name.c_str());
    // if (loc == -1) std::cout << "can't find '" << name << "'" << std::endl;
    glUniform4f(loc, c.r, c.g, c.b, c.a);
}

void Shader::SetMat3(const std::string &name, const gm::Matrix3 &m) const {
    int loc = glGetUniformLocation(id, name.c_str());
    // if (loc == -1) std::cout << "can't find '" << name << "'" << std::endl;
    glUniformMatrix3fv(loc, 1, GL_FALSE, m.Data());
}

void Shader::SetMat4(const std::string &name, const gm::Matrix4 &m) const {
    int loc = glGetUniformLocation(id, name.c_str());
    // if (loc == -1) std::cout << "can't find '" << name << "'" << std::endl;
    glUniformMatrix4fv(loc, 1, GL_FALSE, m.Data());
}

}