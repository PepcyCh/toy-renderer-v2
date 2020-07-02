#include "FrameBuffer.h"

#include "glad/glad.h"

#include <iostream>

struct FrameBufferConfig {
    int width, height;
    GLenum iformat = GL_RGB, format = GL_RGB, type = GL_UNSIGNED_BYTE;
    GLenum filter = GL_NEAREST, wrap = GL_REPEAT;
    GLenum attach;
    bool is_cube = false;
};

struct RenderBufferConfig {
    int width, height;
    GLenum format = GL_DEPTH24_STENCIL8, attach = GL_DEPTH_STENCIL_ATTACHMENT;
};

static unsigned int CreateFrameBuffer(const FrameBufferConfig &conf) {
    unsigned int id;
    glGenTextures(1, &id);
    if (!conf.is_cube) {
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, conf.iformat, conf.width, conf.height, 0,
            conf.format, conf.type, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, conf.filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, conf.filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, conf.wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, conf.wrap);
        glFramebufferTexture2D(GL_FRAMEBUFFER, conf.attach, GL_TEXTURE_2D, id, 0);
    } else {
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
        for (int i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, conf.iformat,
                conf.width, conf.height, 0, conf.format, conf.type, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, conf.filter);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, conf.filter);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, conf.wrap);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, conf.wrap);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, conf.wrap);
        glFramebufferTexture(GL_FRAMEBUFFER, conf.attach, id, 0);
    }
    return id;
}
static unsigned int CreateRenderBuffer(const RenderBufferConfig &conf) {
    unsigned int id;
    glGenRenderbuffers(1, &id);
    glBindRenderbuffer(GL_RENDERBUFFER, id);
    glRenderbufferStorage(GL_RENDERBUFFER, conf.format, conf.width, conf.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, conf.attach, GL_RENDERBUFFER, id);
    return id;
}

namespace pepcy::renderer {

FrameBuffer FrameBuffer::DefaultBuffer(int width, int height) {
    FrameBuffer buffer;
    buffer.width = width;
    buffer.height = height;
    buffer.id = 0;
    return buffer;
}

FrameBuffer FrameBuffer::SimpleBuffer(int width, int height) {
    FrameBuffer buffer;
    buffer.width = width;
    buffer.height = height;

    glGenFramebuffers(1, &buffer.id);
    glBindFramebuffer(GL_FRAMEBUFFER, buffer.id);

    FrameBufferConfig col_conf { width, height };
    col_conf.attach = GL_COLOR_ATTACHMENT0;
    unsigned int col = CreateFrameBuffer(col_conf);
    buffer.attachs["color"] = col;

    RenderBufferConfig dep_conf { width, height };
    buffer.render_buf = CreateRenderBuffer(dep_conf);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "error on simple buffer" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    return buffer;
}

FrameBuffer FrameBuffer::GBuffer(int width, int height) {
    FrameBuffer buffer;
    buffer.width = width;
    buffer.height = height;

    glGenFramebuffers(1, &buffer.id);
    glBindFramebuffer(GL_FRAMEBUFFER, buffer.id);

    FrameBufferConfig pos_conf { width, height };
    pos_conf.iformat = GL_RGBA16F;
    pos_conf.format = GL_RGBA;
    pos_conf.type = GL_FLOAT;
    pos_conf.attach = GL_COLOR_ATTACHMENT0;
    unsigned int g_pos = CreateFrameBuffer(pos_conf);
    buffer.attachs["g_pos"] = g_pos;

    FrameBufferConfig norm_conf { width, height };
    norm_conf.iformat = GL_RGB16F;
    norm_conf.type = GL_FLOAT;
    norm_conf.attach = GL_COLOR_ATTACHMENT1;
    unsigned int g_norm = CreateFrameBuffer(norm_conf);
    buffer.attachs["g_normal"] = g_norm;

    FrameBufferConfig albedo_conf { width, height };
    albedo_conf.type = GL_FLOAT;
    albedo_conf.attach = GL_COLOR_ATTACHMENT2;
    unsigned int g_albedo = CreateFrameBuffer(albedo_conf);
    buffer.attachs["g_albedo"] = g_albedo;

    FrameBufferConfig amb_conf { width, height };
    amb_conf.type = GL_FLOAT;
    amb_conf.attach = GL_COLOR_ATTACHMENT3;
    unsigned int g_amb = CreateFrameBuffer(amb_conf);
    buffer.attachs["g_ambient"] = g_amb;

    FrameBufferConfig spec_conf { width, height };
    spec_conf.iformat = GL_RGBA;
    spec_conf.type = GL_RGBA;
    spec_conf.type = GL_FLOAT;
    spec_conf.attach = GL_COLOR_ATTACHMENT4;
    unsigned int g_spec = CreateFrameBuffer(spec_conf);
    buffer.attachs["g_spec"] = g_spec;

    const GLuint attachs[] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4,
    };
    glDrawBuffers(sizeof(attachs) / sizeof(GLuint), attachs);

    RenderBufferConfig dep_conf { width, height };
    buffer.render_buf = CreateRenderBuffer(dep_conf);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "error on G-buffers" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    return buffer;
}

FrameBuffer FrameBuffer::DepthBuffer(int width, int height) {
    FrameBuffer buffer;
    buffer.width = width;
    buffer.height = height;

    glGenFramebuffers(1, &buffer.id);
    glBindFramebuffer(GL_FRAMEBUFFER, buffer.id);

    FrameBufferConfig dep_conf { width, height };
    dep_conf.iformat = GL_DEPTH_COMPONENT;
    dep_conf.format = GL_DEPTH_COMPONENT;
    dep_conf.type = GL_FLOAT;
    dep_conf.attach = GL_DEPTH_ATTACHMENT;
    dep_conf.wrap = GL_CLAMP_TO_BORDER;
    unsigned int dep = CreateFrameBuffer(dep_conf);
    const float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    buffer.attachs["depth"] = dep;
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "error on depth buffers" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return buffer;
}

FrameBuffer FrameBuffer::CubeDepthBuffer(int width) {
    FrameBuffer buffer;
    buffer.width = width;
    buffer.height = width;

    glGenFramebuffers(1, &buffer.id);
    glBindFramebuffer(GL_FRAMEBUFFER, buffer.id);

    FrameBufferConfig dep_conf { width, width };
    dep_conf.iformat = GL_DEPTH_COMPONENT;
    dep_conf.format = GL_DEPTH_COMPONENT;
    dep_conf.type = GL_FLOAT;
    dep_conf.attach = GL_DEPTH_ATTACHMENT;
    dep_conf.wrap = GL_CLAMP_TO_EDGE;
    dep_conf.is_cube = true;
    unsigned int dep = CreateFrameBuffer(dep_conf);
    buffer.attachs["depth"] = dep;
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "error on cube depth buffers" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return buffer;
}

void FrameBuffer::Use() const {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0, 0, width, height);
}

bool FrameBuffer::IsValid() const {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    bool ret = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return ret;
}

const std::unordered_map<std::string, unsigned int> &
FrameBuffer::GetTextures() const {
    return attachs;
}

}