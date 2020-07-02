#include "VertexArray.h"

#include "glad/glad.h"

namespace pepcy::renderer {

void VertexArray::Draw() const {
    glBindVertexArray(vao_id);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

}