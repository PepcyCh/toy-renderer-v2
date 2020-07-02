#pragma once

#include <unordered_map>

#include "FrameBuffer.h"
#include "Light.h"

namespace pepcy::renderer {

class LightManager {
  public:
    FrameBuffer GetDepthBuffer(const DirectionalLight &light);
    FrameBuffer GetDepthBuffer(const PointLight &light);
    FrameBuffer GetDepthBuffer(const SpotLight &light);
    
    void GenerateDepthBuffer(const DirectionalLight &light,
        const FrameBuffer &buffer);
    void GenerateDepthBuffer(const PointLight &light,
        const FrameBuffer &buffer);
    void GenerateDepthBuffer(const SpotLight &light,
        const FrameBuffer &buffer);

    void Clear();

  private:
    std::unordered_map<gid::GID, FrameBuffer, gid::GIDHasher> depth_buffers;
};

}