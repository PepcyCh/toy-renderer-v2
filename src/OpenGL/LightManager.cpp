#include "LightManager.h"

#include "glad/glad.h"
#include "OpenGLViewer.h"

namespace pepcy::renderer {

FrameBuffer LightManager::GetDepthBuffer(const DirectionalLight &light) {
    gid::GID id = light.GetID();
    if (depth_buffers.count(id) != 0) {
        return depth_buffers.at(id);
    }

    FrameBuffer buffer = FrameBuffer::DepthBuffer(1024, 1024);
    return depth_buffers[id] = buffer;
}

FrameBuffer LightManager::GetDepthBuffer(const PointLight &light) {
    gid::GID id = light.GetID();
    if (depth_buffers.count(id) != 0) {
        return depth_buffers.at(id);
    }

    FrameBuffer buffer = FrameBuffer::CubeDepthBuffer(1024);
    return depth_buffers[id] = buffer;
}

FrameBuffer LightManager::GetDepthBuffer(const SpotLight &light) {
    gid::GID id = light.GetID();
    if (depth_buffers.count(id) != 0) {
        return depth_buffers.at(id);
    }

    FrameBuffer buffer = FrameBuffer::DepthBuffer(1024, 1024);
    return depth_buffers[id] = buffer;
}

void LightManager::GenerateDepthBuffer(const DirectionalLight &light,
        const FrameBuffer &buffer) {
    Camera dir_cam = light.GetCamera();
    std::vector<RenderTask *> tasks = {
        new CustomTask(
            [&buffer]() {
                opengl_viewer.UseShader(opengl_viewer.shaders["depth"]);
                buffer.Use();
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_CULL_FACE);
                glClear(GL_DEPTH_BUFFER_BIT);
            }
        ),
        new SetCamera(&dir_cam),
        new MeshLoop(opengl_viewer.GetConfig().scene,
            new DrawMesh()
        ),
        new CustomTask(
            []() { opengl_viewer.Reset(); }
        )
    };
    for (const auto &task : tasks) {
        task->Run();
    }
    for (auto &task : tasks) {
        delete task;
        task = nullptr;
    }
}

void LightManager::GenerateDepthBuffer(const PointLight &light,
        const FrameBuffer &buffer) {
    CubeCamera point_cam(light.pos);
    std::vector<RenderTask *> tasks = {
        new CustomTask(
            [&buffer]() {
                Shader sh = opengl_viewer.shaders["cube depth"];
                opengl_viewer.UseShader(sh);
                buffer.Use();
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_CULL_FACE);
                glClear(GL_DEPTH_BUFFER_BIT);
            }
        ),
        new SetCubeCamera(&point_cam),
        new MeshLoop(opengl_viewer.GetConfig().scene,
            new DrawMesh()
        ),
        new CustomTask(
            []() { opengl_viewer.Reset(); }
        )
    };
    for (const auto &task : tasks) {
        task->Run();
    }
    for (auto &task : tasks) {
        delete task;
        task = nullptr;
    }
}

void LightManager::GenerateDepthBuffer(const SpotLight &light,
        const FrameBuffer &buffer) {
    Camera spot_cam = light.GetCamera();
    std::vector<RenderTask *> tasks = {
        new CustomTask(
            [&buffer]() {
                opengl_viewer.UseShader(opengl_viewer.shaders["depth"]);
                buffer.Use();
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_CULL_FACE);
                glClear(GL_DEPTH_BUFFER_BIT);
            }
        ),
        new SetCamera(&spot_cam),
        new MeshLoop(opengl_viewer.GetConfig().scene,
            new DrawMesh()
        ),
        new CustomTask(
            []() { opengl_viewer.Reset(); }
        )
    };
    for (const auto &task : tasks) {
        task->Run();
    }
    for (auto &task : tasks) {
        delete task;
        task = nullptr;
    }
}

void LightManager::Clear() {
    depth_buffers.clear();
}

}