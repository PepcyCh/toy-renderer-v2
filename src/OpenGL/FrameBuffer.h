#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace pepcy::renderer {

struct FrameBuffer {
    static FrameBuffer DefaultBuffer(int width, int height);
    static FrameBuffer SimpleBuffer(int width, int height);
    static FrameBuffer GBuffer(int width, int height);
    static FrameBuffer DepthBuffer(int width, int height);
    static FrameBuffer CubeDepthBuffer(int width);

    void Use() const;
    bool IsValid() const;

    const std::unordered_map<std::string, unsigned int> &GetTextures() const;

    int width, height;
    unsigned int id = 0;
    std::unordered_map<std::string, unsigned int> attachs;
    unsigned int render_buf = 0;
};

}