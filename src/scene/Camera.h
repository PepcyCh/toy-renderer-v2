#pragma once

#include "geomath.h"

namespace pepcy::renderer {

class Camera {
  public:
    Camera(const gm::Vector3 &pos, const gm::Vector3 &look_at,
           const gm::Vector3 &up, float fov, float aspect,
           float near = 0.1f, float far = 100.0f, bool perspective = true);

    void Switch();

    gm::Matrix4 GetMatrix() const;
    gm::Matrix4 GetViewMatrix() const;
    gm::Matrix4 GetProjectionMatrix() const;
    gm::Vector3 GetPosition() const;
    gm::Vector3 GetLookAt() const;

    void SetAspect(float new_aspect);
    void LookAt(const gm::Vector3 &look_at);

    void MoveForward(float delta);
    void MoveUp(float delta);
    void MoveRight(float delta);
    void Rotate(float delta_x, float delta_y);
    void OrbitH(float delta, const gm::Vector3 &focus);
    void OrbitV(float delta, const gm::Vector3 &focus);
    void ZoomIn(float delta);
    void ZoomOut(float delta);

    gm::Ray GenRay(float x, float y) const;

  private:
    gm::Matrix4 GetOrthographicsMatrix() const;

    gm::Vector3 pos;
    gm::Vector3 forward, up, right;
    gm::Matrix4 view, projection;
    float fov, aspect;
    float near, far;
    bool perspective;
};

class CubeCamera {
  public:
    CubeCamera(const gm::Vector3 &pos, float near = 0.1f, float far = 100.0f);

    gm::Matrix4 GetMatrix(int i) const;
    gm::Vector3 GetPosition() const;

    void MoveX(float delta);
    void MoveY(float delta);
    void MoveZ(float delta);

  private:
    void ConstructMatrices();

    gm::Vector3 pos;
    gm::Matrix4 mat[6];
    float near, far;
};

}