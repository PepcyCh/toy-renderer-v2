#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

out vec4 frag_pos;

uniform mat4 cubecam_mat[6];

void main() {
    for (int face = 0; face < 6; face++) {
        gl_Layer = face;
        for (int i = 0; i < 3; i++) {
            frag_pos = gl_in[i].gl_Position;
            gl_Position = cubecam_mat[face] * frag_pos;
            EmitVertex();
        }
        EndPrimitive();
    }
}