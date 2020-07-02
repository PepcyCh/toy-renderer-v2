#include "Camera.h"

#include <cmath>

namespace pepcy::renderer {

Camera::Camera(const gm::Vector3 &pos, const gm::Vector3 &look_at,
               const gm::Vector3 &up, float fov, float aspect,
               float near, float far, bool perspective) :
               pos(pos), fov(fov), aspect(aspect), near(near), far(far),
               perspective(perspective), up(up) {
    LookAt(look_at);
}

void Camera::Switch() {
    if (perspective) {
        perspective = false;
        projection = GetOrthographicsMatrix();
    } else {
        perspective = true;
        projection = gm::Perspective(fov, aspect, near, far);
    }
}

gm::Matrix4 Camera::GetMatrix() const {
    return projection * view;
}

gm::Matrix4 Camera::GetProjectionMatrix() const {
    return projection;
}

gm::Matrix4 Camera::GetViewMatrix() const {
    return view;
}

gm::Vector3 Camera::GetPosition() const {
    return pos;
}

gm::Vector3 Camera::GetLookAt() const {
    return pos + forward;
}

void Camera::SetAspect(float new_aspect) {
    if (aspect != new_aspect) {
        aspect = new_aspect;
        projection = perspective ? gm::Perspective(fov, aspect, near, far) :
            GetOrthographicsMatrix();
    }
}

void Camera::LookAt(const gm::Vector3 &look_at) {
    forward = gm::Normalize(look_at - pos);
    right = gm::Normalize(Cross(forward, gm::Vector3(0.0f, 1.0f, 0.0f)));
    up = gm::Cross(right, forward);

    view = gm::LookAt(pos, look_at, up);
    projection = perspective ? gm::Perspective(fov, aspect, near, far) :
        GetOrthographicsMatrix();
}

void Camera::MoveForward(float delta) {
    pos += forward * delta;
    view = gm::LookAt(pos, pos + forward, up);
}

void Camera::MoveRight(float delta) {
    pos += right * delta;
    view = gm::LookAt(pos, pos + forward, up);
}

void Camera::MoveUp(float delta) {
    pos += up * delta;
    view = gm::LookAt(pos, pos + forward, up);
}

void Camera::Rotate(float delta_x, float delta_y) {
    gm::Matrix4 m_horizontal = gm::Rotate(gm::Vector3(0, 1, 0), delta_x);
    forward = gm::Vector3(m_horizontal * gm::Vector4(forward, 0.0f));
    right = gm::Vector3(m_horizontal * gm::Vector4(right, 0.0f));
    up = gm::Vector4(m_horizontal * gm::Vector4(up, 0.0f));
    gm::Matrix4 m_vertical = gm::Rotate(right, delta_y);
    forward = gm::Vector4(m_vertical * gm::Vector4(forward, 0.0f));
    up = gm::Vector4(m_vertical * gm::Vector4(up, 0.0f));
    view = gm::LookAt(pos, pos + forward, up);
}

void Camera::OrbitH(float delta, const gm::Vector3 &focus) {
    gm::Matrix4 t_before = gm::Translate(-focus[0], 0.0f, -focus[2]);
    gm::Matrix4 t_rot = gm::RotateY(delta);
    gm::Matrix4 t_after = gm::Translate(focus[0], 0.0f, focus[2]);
    gm::Transform trans(t_after * t_rot * t_before);
    pos = trans.TransformPoint(pos);
    LookAt(focus);
}
void Camera::OrbitV(float delta, const gm::Vector3 &focus) {
    float dot = gm::Dot(forward, gm::Vector3(0.0f, 1.0f, 0.0f));
    if ((dot >= 0.99 && delta > 0) || (dot <= -0.99 && delta < 0)) {
        return;
    }
    gm::Matrix4 t_before = gm::Translate(-focus[0], -focus[1], -focus[2]);
    gm::Matrix4 t_rot = gm::Rotate(right, delta);
    gm::Matrix4 t_after = gm::Translate(focus[0], focus[1], focus[2]);
    gm::Transform trans(t_after * t_rot * t_before);
    pos = trans.TransformPoint(pos);
    LookAt(focus);
}

void Camera::ZoomIn(float delta) {
    fov = std::max(fov - delta, gm::Radians(5.0f));
    projection = perspective ? gm::Perspective(fov, aspect, near, far) :
        GetOrthographicsMatrix();
}
void Camera::ZoomOut(float delta) {
    fov = std::min(fov + delta, gm::Radians(150.0f));
    projection = perspective ? gm::Perspective(fov, aspect, near, far) :
        GetOrthographicsMatrix();
}

gm::Matrix4 Camera::GetOrthographicsMatrix() const {
    float tanfov = std::tan(fov / 2.0f);
    float t = near * tanfov;
    float r = aspect * t;
    return gm::Orthographic(-r, r, -t, t, near, far);
}

gm::Ray Camera::GenRay(float x, float y) const {
    x = x * 2.0f - 1.0f;
    y = y * 2.0f - 1.0f;
    float y_max = near * std::tan(fov / 2);
    float x_max = y_max * aspect;

    gm::Vector3 ret(x_max * x, y_max * y, -near);
    gm::Matrix3 mat(right, up, -forward);
    return gm::Ray(pos, mat * ret);
}

CubeCamera::CubeCamera(const gm::Vector3 &pos, float near, float far) :
        pos(pos), near(near), far(far) {
    ConstructMatrices();
}

gm::Matrix4 CubeCamera::GetMatrix(int i) const {
    return mat[i];
}

gm::Vector3 CubeCamera::GetPosition() const {
    return pos;
}

void CubeCamera::MoveX(float delta) {
    pos += gm::Vector3(1, 0, 0) * delta;
    ConstructMatrices();
}

void CubeCamera::MoveY(float delta) {
    pos += gm::Vector3(0, 1, 0) * delta;
    ConstructMatrices();
}

void CubeCamera::MoveZ(float delta) {
    pos += gm::Vector3(0, 0, 1) * delta;
    ConstructMatrices();
}

void CubeCamera::ConstructMatrices() {
    auto proj = gm::Perspective(gm::Radians(90.0f), 1.0f, near, far);
    mat[0] = proj * gm::LookAt(pos, pos + gm::Vector3(1, 0, 0),
        gm::Vector3(0, -1, 0));
    mat[1] = proj * gm::LookAt(pos, pos + gm::Vector3(-1, 0, 0),
        gm::Vector3(0, -1, 0));
    mat[2] = proj * gm::LookAt(pos, pos + gm::Vector3(0, 1, 0),
        gm::Vector3(0, 0, 1));
    mat[3] = proj * gm::LookAt(pos, pos + gm::Vector3(0, -1, 0),
        gm::Vector3(0, 0, -1));
    mat[4] = proj * gm::LookAt(pos, pos + gm::Vector3(0, 0, 1),
        gm::Vector3(0, -1, 0));
    mat[5] = proj * gm::LookAt(pos, pos + gm::Vector3(0, 0, -1),
        gm::Vector3(0, -1, 0));
}

}