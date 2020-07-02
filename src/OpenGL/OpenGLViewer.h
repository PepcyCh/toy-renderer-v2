#pragma once

#include <stack>

#include "LightManager.h"
#include "Shader.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "VertexArrayManager.h"
#include "RenderTask.h"

namespace pepcy::renderer {

struct OpenGLViewerConfig {
    Scene *scene;
    Camera *cam;
    int width;
    int height;
    bool is_deferred;
    bool skybox;
    std::string skybox_path;
    std::string skybox_suffix;
    enum class LightModel {
        Normal,
        Phong,
        BlinnPhong
    } light_model;
};

class OpenGLViewer {
  public:
    OpenGLViewer();
    OpenGLViewer(const OpenGLViewerConfig &config);

    void Draw();
    void Reset();

    void Clear();
    OpenGLViewerConfig GetConfig() const;
    void SetConfig(const OpenGLViewerConfig &config);
    void Resize(int width, int height);

    Shader GetShader() const;

    void UseShader(const Shader &sh);
    void UnuseShader();

    void GenerateDepthBuffer(const DirectionalLight &light);
    void GenerateDepthBuffer(const PointLight &light);
    void GenerateDepthBuffer(const SpotLight &light);

    void SetTexture2D(const std::string &name, unsigned int id);
    void PopTexture2D(int count);
    void SetTextureCube(const std::string &name, unsigned int id);
    void PopTextureCube(int count);
    void ResetTexture();

    void ScreenShot();

    LightManager light_mgr;
    TextureManager tex_mgr;
    VertexArrayManager vao_mgr;

  private:
    void CreateShaders();
    void CreateFrameBuffers();
    void ConstructTasks();

    bool init = false;

    int n_tex_2d, n_tex_cube;
    int n_shot = 0;

    OpenGLViewerConfig config;
    ShaderManager shader_mgr;

    std::stack<Shader> shader_stk;
    std::unordered_map<std::string, FrameBuffer> framebuffers;
    std::unordered_map<std::string, Shader> shaders;
    std::vector<RenderTask *> tasks;

    friend class LightManager;
};

extern OpenGLViewer opengl_viewer;

}