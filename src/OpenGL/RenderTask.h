#pragma once

#include <any>
#include <functional>

#include "Camera.h"
#include "Scene.h"

namespace pepcy::renderer {

class RenderTask {
  public:
    virtual void Run(const std::any &a = std::any()) const = 0;
};

class NullTask : public RenderTask {
  public:
    void Run(const std::any &a = std::any()) const {}
};

class SetCamera : public RenderTask {
  public:
    SetCamera(const Camera *cam) : cam(cam) {}

    void Run(const std::any &a = std::any()) const;

  private:
    const Camera *cam;
};

class SetCubeCamera : public RenderTask {
  public:
    SetCubeCamera(const CubeCamera *cam) : cam(cam) {}

    void Run(const std::any &a = std::any()) const;

  private:
    const CubeCamera *cam;
};

class DrawMesh : public RenderTask {
  public:
    DrawMesh() = default;
    DrawMesh(const Shape *mesh) : default_mesh(mesh) {}

    void Run(const std::any &a = std::any()) const override;

  private:
    const Shape *default_mesh = nullptr;
};

class DrawShape : public RenderTask {
  public:
    DrawShape() = default;
    DrawShape(const Shape *shape) : default_shape(shape) {}

    void Run(const std::any &a = std::any()) const override;

  private:
    const Shape *default_shape = nullptr;
};

class LightLoop : public RenderTask {
  public:
    LightLoop(const Scene *scene, const RenderTask *sub_task) :
        scene(scene), sub_task(sub_task) {}
    ~LightLoop() { delete sub_task; }

    void Run(const std::any &a = std::any()) const override;

  private:
    const Scene *scene;
    const RenderTask *sub_task;
};

class MeshLoop : public RenderTask {
  public:
    MeshLoop(const Scene *scene, const RenderTask *sub_task) :
        scene(scene), sub_task(sub_task) {}
    ~MeshLoop() { delete sub_task; }

    void Run(const std::any &a = std::any()) const override;

  private:
    const Scene *scene;
    const RenderTask *sub_task;
};

class CustomTask : public RenderTask {
  public:
    CustomTask(const std::function<void()> &task) : task(task) {}

    void Run(const std::any &a = std::any()) const override;

  private:
    std::function<void()> task;
};

}