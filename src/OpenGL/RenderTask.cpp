#include "RenderTask.h"

#include "glad/glad.h"
#include "OpenGLViewer.h"

namespace pepcy::renderer {

void SetCamera::Run(const std::any &a) const {
    auto sh = opengl_viewer.GetShader();
    sh.SetVec3("cam_pos", cam->GetPosition());
    sh.SetMat4("projection", cam->GetProjectionMatrix());
    sh.SetMat4("view", cam->GetViewMatrix());
}

void SetCubeCamera::Run(const std::any &a) const {
    auto sh = opengl_viewer.GetShader();
    sh.SetVec3("cam_pos", cam->GetPosition());
    for (int i = 0; i < 6; i++) {
        sh.SetMat4("cubecam_mat[" + std::to_string(i) + "]", cam->GetMatrix(i));
    }
}

void DrawMesh::Run(const std::any &a) const {
    const Shape *pmesh;
    if (auto p = std::any_cast<Shape *>(&a)) {
        pmesh = *p;
    } else {
        pmesh = default_mesh;
    }

    VertexArray vao = opengl_viewer.vao_mgr.GetVertexArray(pmesh);
    auto sh = opengl_viewer.GetShader();
    int n_tex = 0;
    for (const auto &[name, tex] : pmesh->GetMaterial().GetTextures()) {
        if (tex.IsColor()) {
            sh.SetInt(name + "_b", 0);
            auto col = tex.GetColor();
            sh.SetVec3(name + "_c", col);
        } else {
            sh.SetInt(name + "_b", 1);
            opengl_viewer.SetTexture2D(name + "_i", opengl_viewer.tex_mgr
                .LoadTexture(tex.GetPath(), tex.NeedGamma()));
            ++n_tex;
        }
    }
    vao.Draw();
    opengl_viewer.PopTexture2D(n_tex);
}

void DrawShape::Run(const std::any &a) const {
    const Shape *pshape;
    if (auto p = std::any_cast<Shape *>(&a)) {
        pshape = *p;
    } else {
        pshape = default_shape;
    }
    
    VertexArray vao = opengl_viewer.vao_mgr.GetVertexArray(pshape);
    vao.Draw();
}

void MeshLoop::Run(const std::any &a) const {
    const auto &meshes = scene->GetMeshes();
    for (const auto mesh : meshes) {
        auto sh = opengl_viewer.GetShader();
        sh.SetMat4("model", mesh->GetModel().GetMatrix());
        sh.SetMat4("model_TI", mesh->GetModel().GetITMatrix());
        sub_task->Run(mesh);
    }
}

void LightLoop::Run(const std::any &a) const {
    static const int MAX_LIGHT = 16;
    const auto &dir_lights = scene->GetDirLights();
    const auto &point_lights = scene->GetPointLights();
    const auto &spot_lights = scene->GetSpotLights();
    auto sh = opengl_viewer.GetShader();

    for (int k = 0; ; k += MAX_LIGHT) {
        int n_tex_2d = 0, n_tex_cube = 0;
        // TODO - depth test and blend

        int n_dir_lights = std::clamp(int(dir_lights.size() - k), 0, MAX_LIGHT);
        int n_point_lights = std::clamp(int(point_lights.size() - k), 0, MAX_LIGHT);
        int n_spot_lights = std::clamp(int(spot_lights.size() - k), 0, MAX_LIGHT);
        if (n_dir_lights == 0 && n_point_lights == 0 && n_spot_lights == 0 && k) {
            break;
        }

        sh.SetInt("n_dir_lights", n_dir_lights);
        for (int i = k; i < k + n_dir_lights; i++) {
            std::string prefix = "dir_lights[" + std::to_string(i - k) + "].";
            sh.SetInt(prefix + "shadow", dir_lights[i].shadow);
            sh.SetVec3(prefix + "color", dir_lights[i].color);
            sh.SetVec3(prefix + "dir", dir_lights[i].dir);
            sh.SetMat4(prefix + "mat", dir_lights[i].GetCamera().GetMatrix());
            if (dir_lights[i].shadow) {
                opengl_viewer.SetTexture2D(prefix + "depth", opengl_viewer
                    .light_mgr.GetDepthBuffer(dir_lights[i]).attachs["depth"]);
                ++n_tex_2d;
            }
        }
        
        sh.SetInt("n_point_lights", n_point_lights);
        for (int i = k; i < k + n_point_lights; i++) {
            std::string prefix = "point_lights[" + std::to_string(i - k) + "].";
            sh.SetInt(prefix + "shadow", point_lights[i].shadow);
            sh.SetVec3(prefix + "color", point_lights[i].color);
            sh.SetVec3(prefix + "pos", point_lights[i].pos);
            sh.SetFloat(prefix + "kq", point_lights[i].kq);
            sh.SetFloat(prefix + "kl", point_lights[i].kl);
            sh.SetFloat(prefix + "kc", point_lights[i].kc);
            if (point_lights[i].shadow) {
                opengl_viewer.SetTextureCube(prefix + "depth", opengl_viewer
                    .light_mgr.GetDepthBuffer(point_lights[i]).attachs["depth"]);
                ++n_tex_cube;
            }
        }

        sh.SetInt("n_spot_lights", n_spot_lights);
        for (int i = k; i < k + n_spot_lights; i++) {
            std::string prefix = "spot_lights[" + std::to_string(i - k) + "].";
            sh.SetInt(prefix + "shadow", spot_lights[i].shadow);
            sh.SetVec3(prefix + "color", spot_lights[i].color);
            sh.SetVec3(prefix + "pos", spot_lights[i].pos);
            sh.SetVec3(prefix + "direction", spot_lights[i].dir);
            sh.SetFloat(prefix + "kq", spot_lights[i].kq);
            sh.SetFloat(prefix + "kl", spot_lights[i].kl);
            sh.SetFloat(prefix + "kc", spot_lights[i].kc);
            sh.SetFloat(prefix + "cutoff", spot_lights[i].cutoff);
            sh.SetFloat(prefix + "outer_cutoff", spot_lights[i].outer_cutoff);
            sh.SetMat4(prefix + "mat", spot_lights[i].GetCamera().GetMatrix());
            if (spot_lights[i].shadow) {
                opengl_viewer.SetTexture2D(prefix + "depth", opengl_viewer
                    .light_mgr.GetDepthBuffer(spot_lights[i]).attachs["depth"]);
                ++n_tex_2d;
            }
        }

        sub_task->Run(a);

        opengl_viewer.PopTexture2D(n_tex_2d);
        opengl_viewer.PopTextureCube(n_tex_cube);
    }

    // TODO - gamma and tune mapping
}

void CustomTask::Run(const std::any &a) const {
    task();
}

}