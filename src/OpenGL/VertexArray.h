#pragma once

namespace pepcy::renderer {

struct VertexArray {
    void Draw() const;

    unsigned int vao_id, ebo_id, vbo_id;
    int count;
};

}