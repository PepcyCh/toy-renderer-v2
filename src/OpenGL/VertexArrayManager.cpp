#include "VertexArrayManager.h"

#include "glad/glad.h"

namespace pepcy::renderer {

static VertexArray CreateVertexArray(const Shape *shape) {
    VertexArray vao;
    vao.count = shape->GetIndexCount();

    glGenVertexArrays(1, &vao.vao_id);
    glBindVertexArray(vao.vao_id);

    glGenBuffers(1, &vao.ebo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao.ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape->GetIndexCount() *
        sizeof(unsigned int), shape->GetIndices(), GL_STATIC_DRAW);

    int n = shape->GetVertexCount();
    int buffer_size = n * (3 + 3 + 2 + 3 + 3) * sizeof(float);
    glGenBuffers(1, &vao.vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vao.vbo_id);
    glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_STATIC_DRAW);
    
    int offset = 0;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) offset);
    glBufferSubData(GL_ARRAY_BUFFER, offset,
        n * 3 * sizeof(float), shape->GetPositions());

    offset += n * 3 * sizeof(float);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *) offset);
    glBufferSubData(GL_ARRAY_BUFFER, offset,
        n * 3 * sizeof(float), shape->GetNormals());

    offset += n * 3 * sizeof(float);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void *) offset);
    glBufferSubData(GL_ARRAY_BUFFER, offset,
        n * 2 * sizeof(float), shape->GetTexcoords());

    offset += n * 2 * sizeof(float);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void *) offset);
    glBufferSubData(GL_ARRAY_BUFFER, offset,
        n * 3 * sizeof(float), shape->GetTangents());

    offset += n * 3 * sizeof(float);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void *) offset);
    glBufferSubData(GL_ARRAY_BUFFER, offset,
        n * 3 * sizeof(float), shape->GetBitangents());

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return vao;
}

VertexArray
VertexArrayManager::GetVertexArray(const Shape *shape) {
    gid::GID id = shape->GetID();
    if (vaos.count(id) != 0) {
        return vaos.at(id);
    }

    VertexArray vao = CreateVertexArray(shape);
    return vaos[id] = vao;
}

void VertexArrayManager::Clear() {
    for (const auto &[_, vao] : vaos) {
        glDeleteVertexArrays(1, &vao.vao_id);
        glDeleteBuffers(1, &vao.vbo_id);
        glDeleteBuffers(1, &vao.ebo_id);
    }
    vaos.clear();
}

}