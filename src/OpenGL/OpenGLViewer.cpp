#include "OpenGLViewer.h"

#include "../defines.h"
#include "glad/glad.h"
#include "BasicShape.h"

#include "stb_image_write.h"

namespace pepcy::renderer {

OpenGLViewer opengl_viewer;

OpenGLViewer::OpenGLViewer() {}

OpenGLViewer::OpenGLViewer(const OpenGLViewerConfig &config) : config(config) {
    CreateShaders();
    CreateFrameBuffers();
    ConstructTasks();
    init = true;
}

OpenGLViewerConfig OpenGLViewer::GetConfig() const {
    return config;
}
void OpenGLViewer::SetConfig(const OpenGLViewerConfig &config) {
    if (!init) {
        CreateShaders();
    }
    bool resize = config.width != this->config.width ||
        config.height != this->config.height;
    this->config = config;
    if (!init || resize) {
        CreateFrameBuffers();
    }
    ConstructTasks();
    init = true;
}
void OpenGLViewer::Resize(int width, int height) {
    config.width = width;
    config.height = height;
    CreateFrameBuffers();
    if (!init) {
        CreateShaders();
        ConstructTasks();
    }
    init = true;
}

Shader OpenGLViewer::GetShader() const {
    return shader_stk.top();
}

void OpenGLViewer::UseShader(const Shader &sh) {
    shader_stk.push(sh);
    sh.Use();
}
void OpenGLViewer::UnuseShader() {
    shader_stk.pop();
    if (!shader_stk.empty()) {
        shader_stk.top().Use();
    } else {
        glUseProgram(0);
    }
}

void OpenGLViewer::Draw() {
    for (const auto &task : tasks) {
        task->Run();
    }
    Reset();
}

void OpenGLViewer::ConstructTasks() {
    for (auto &task : tasks) {
        delete task;
        task = nullptr;
    }
    tasks.clear();

    Shader sh = shaders["normal"];
    Shader sh_defer_pre = shaders["defer-pre"];
    Shader sh_defer_light = shaders["defer normal"];
    auto light_model = config.light_model;
    if (light_model == OpenGLViewerConfig::LightModel::Phong) {
        sh = shaders["phong"];
        sh_defer_light = shaders["defer phong"];
    } else if (light_model == OpenGLViewerConfig::LightModel::BlinnPhong) {
        sh = shaders["blinn"];
        sh_defer_light = shaders["defer blinn"];
    }
    Shader sh_screen = shaders["screen"];

    static const Shape *screen_quad = new ScreenQuad();
    static const Shape *skybox_cube = new Cube();
    if (config.is_deferred) {
        tasks = {
            new CustomTask(
                [sh_defer_pre, this]() {
                    ResetTexture();
                    glEnable(GL_DEPTH_TEST);
                    glEnable(GL_CULL_FACE);
                    UseShader(sh_defer_pre);
                    framebuffers["G-buffer"].Use();
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                        GL_STENCIL_BUFFER_BIT);
                }
            ),
            new SetCamera(config.cam),
            new MeshLoop(config.scene,
                new DrawMesh()
            ),
            new CustomTask(
                [sh_defer_light, this]() {
                    ResetTexture();
                    UnuseShader();
                    UseShader(sh_defer_light);

                    framebuffers["simple"].Use();
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                        GL_STENCIL_BUFFER_BIT);

                    for (const auto &[name, tex] :
                            framebuffers["G-buffer"].GetTextures()) {
                        SetTexture2D(name, tex);
                    }
                }
            ),
            new SetCamera(config.cam),
            new LightLoop(config.scene,
                new DrawShape(screen_quad)
            )
        };
    } else {
        tasks = {
            new CustomTask(
                [sh, this]() {
                    ResetTexture();
                    glEnable(GL_DEPTH_TEST);
                    glEnable(GL_CULL_FACE);
                    UseShader(sh);
                    framebuffers["simple"].Use();
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                        GL_STENCIL_BUFFER_BIT);
                }
            ),
            new SetCamera(config.cam),
            new MeshLoop(config.scene,
                new LightLoop(config.scene,
                    new DrawMesh()
                )
            )
        };
    }

    if (config.skybox && config.light_model != OpenGLViewerConfig::LightModel::Normal) {
        tasks.emplace_back(
            new CustomTask(
                [this]() {
                    ResetTexture();
                    UnuseShader();
                    UseShader(shaders.at("cubemap"));
                    glDepthFunc(GL_LEQUAL);
                    glDisable(GL_CULL_FACE);
                    SetTextureCube("cubemap", tex_mgr.LoadCubemap(
                        config.skybox_path, config.skybox_suffix, true));
                }
            )
        );
        tasks.emplace_back(
            new SetCamera(config.cam)
        ),
        tasks.emplace_back(
            new DrawShape(skybox_cube)
        );
        tasks.emplace_back(
            new CustomTask(
                []() { glDepthFunc(GL_LESS); }
            )
        );
    }

    tasks.emplace_back(
        new CustomTask(
            [sh_screen, this]() {
                ResetTexture();
                UnuseShader();
                UseShader(sh_screen);
                framebuffers["default"].Use();
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                    GL_STENCIL_BUFFER_BIT);

                for (const auto &[name, tex] :
                        framebuffers["simple"].GetTextures()) {
                    SetTexture2D(name, tex);
                }
            }
        )
    );
    tasks.emplace_back(
        new DrawShape(screen_quad)
    );
}

void OpenGLViewer::Reset() {
    while (!shader_stk.empty()) shader_stk.pop();
    glUseProgram(0);
    framebuffers["default"].Use();
    ResetTexture();
}

void OpenGLViewer::Clear() {
    light_mgr.Clear();
    vao_mgr.Clear();
}

void OpenGLViewer::GenerateDepthBuffer(const DirectionalLight &light) {
    FrameBuffer buf = light_mgr.GetDepthBuffer(light);
    light_mgr.GenerateDepthBuffer(light, buf);
}
void OpenGLViewer::GenerateDepthBuffer(const PointLight &light) {
    FrameBuffer buf = light_mgr.GetDepthBuffer(light);
    light_mgr.GenerateDepthBuffer(light, buf);
}
void OpenGLViewer::GenerateDepthBuffer(const SpotLight &light) {
    FrameBuffer buf = light_mgr.GetDepthBuffer(light);
    light_mgr.GenerateDepthBuffer(light, buf);
}

void OpenGLViewer::SetTexture2D(const std::string &name, unsigned int id) {
    glActiveTexture(GL_TEXTURE0 + n_tex_2d);
    glBindTexture(GL_TEXTURE_2D, id);
    GetShader().SetInt(name, n_tex_2d);
    ++n_tex_2d;
}
void OpenGLViewer::PopTexture2D(int count) {
    n_tex_2d -= count;
    for (int i = n_tex_2d; i < n_tex_2d + count; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glActiveTexture(GL_TEXTURE0);
}
void OpenGLViewer::SetTextureCube(const std::string &name, unsigned int id) {
    glActiveTexture(GL_TEXTURE0 + n_tex_cube);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    GetShader().SetInt(name, n_tex_cube);
    ++n_tex_cube;
}
void OpenGLViewer::PopTextureCube(int count) {
    n_tex_cube -= count;
    for (int i = n_tex_cube; i < n_tex_cube + count; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    glActiveTexture(GL_TEXTURE0);
}
void OpenGLViewer::ResetTexture() {
    for (int i = 0; i < n_tex_2d; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    for (int i = 0; i < n_tex_cube; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    glActiveTexture(GL_TEXTURE0);
    n_tex_2d = n_tex_cube = 0;
}

static void SaveTexture(const std::string &name, unsigned int tex_id,
        int w, int h, int n, GLenum format = GL_RGB) {
    unsigned char *data = new unsigned char[w * h * n];
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, data);

    for (int i = 0; i < h / 2; i++) {
        int a = i * w * n;
        int b = (h - i - 1) * w * n;
        for (int j = 0; j < w; j++, a += n, b += n) {
            for (int k = 0; k < n; k++) {
                std::swap(data[a + k], data[b + k]);
            }
        }
    }
    stbi_write_png(name.c_str(), w, h, n, data, w * n);

    delete[] data;
}

void OpenGLViewer::ScreenShot() {
    ++n_shot;
    std::string name = shot_path + "screen_shots_" + std::to_string(n_shot) + ".png";
    SaveTexture(name, framebuffers["simple"].attachs["color"], config.width,
        config.height, 3);
    /*
    if (config.is_deferred) {
        name = shot_path + "g_pos_" + std::to_string(n_shot) + ".png";
        SaveTexture(name, framebuffers["G-buffer"].attachs["g_pos"], config.width,
            config.height, 4);
        name = shot_path + "g_norm_" + std::to_string(n_shot) + ".png";
        SaveTexture(name, framebuffers["G-buffer"].attachs["g_normal"], config.width,
            config.height, 3);
        name = shot_path + "g_albedo_" + std::to_string(n_shot) + ".png";
        SaveTexture(name, framebuffers["G-buffer"].attachs["g_albedo"], config.width,
            config.height, 3);
        name = shot_path + "g_ambient_" + std::to_string(n_shot) + ".png";
        SaveTexture(name, framebuffers["G-buffer"].attachs["g_ambient"], config.width,
            config.height, 3);
        name = shot_path + "g_spec_" + std::to_string(n_shot) + ".png";
        SaveTexture(name, framebuffers["G-buffer"].attachs["g_spec"], config.width,
            config.height, 4);
    }
    */
}

void OpenGLViewer::CreateShaders() {
    // phong
    Shader sh_phong;
    sh_phong.Attach(shader_mgr.VertexShader(
        GLSLshader_path + "P3N3U2T3B3.vs"));
    sh_phong.Attach(shader_mgr.FragmentShader(
        GLSLshader_path + "P3N3U2T3B3_phong.fs"));
    bool ret_phong = sh_phong.Link();
    if (!ret_phong) {
        std::cout << "error on shader 'phong'" << std::endl;
    }
    shaders.insert(std::make_pair("phong", sh_phong));
    // blinn
    Shader sh_blinn;
    sh_blinn.Attach(shader_mgr.VertexShader(
        GLSLshader_path + "P3N3U2T3B3.vs"));
    sh_blinn.Attach(shader_mgr.FragmentShader(
        GLSLshader_path + "P3N3U2T3B3_blinn.fs"));
    bool ret_blinn = sh_blinn.Link();
    if (!ret_blinn) {
        std::cout << "error on shader 'blinn'" << std::endl;
    }
    shaders.insert(std::make_pair("blinn", sh_blinn));
    // cook-torrance
    // Shader sh_torrance;
    // sh_torrance.Attach(shader_mgr.VertexShader(
    //     GLSLshader_path + "P3N3U2T3B3.vs"));
    // sh_torrance.Attach(shader_mgr.FragmentShader(
    //     GLSLshader_path + "P3N3U2T3B3_torrance.fs"));
    // bool ret_torrance = sh_torrance.Link();
    // if (!ret_torrance) {
    //     std::cout << "error on shader 'cook-torrance'" << std::endl;
    // }
    // shaders.insert(std::make_pair("cook-torrance", sh_torrance));
    // normal
    Shader sh_normal;
    sh_normal.Attach(shader_mgr.VertexShader(
        GLSLshader_path + "P3N3U2T3B3.vs"));
    sh_normal.Attach(shader_mgr.FragmentShader(
        GLSLshader_path + "P3N3U2T3B3_normal.fs"));
    bool ret_normal = sh_normal.Link();
    if (!ret_normal) {
        std::cout << "error on shader 'normal'" << std::endl;
    }
    shaders.insert(std::make_pair("normal", sh_normal));
    // deferred pre-process
    Shader sh_defer_pre;
    sh_defer_pre.Attach(shader_mgr.VertexShader(
        GLSLshader_path + "P3N3U2T3B3.vs"));
    sh_defer_pre.Attach(shader_mgr.FragmentShader(
        GLSLshader_path + "P3N3U2T3B3_deferred.fs"));
    bool ret_defer_pre = sh_defer_pre.Link();
    if (!ret_defer_pre) {
        std::cout << "error on shader 'deferred pre-process'" << std::endl;
    }
    shaders.insert(std::make_pair("defer-pre", sh_defer_pre));
    // deferred phong
    Shader sh_defer_phong;
    sh_defer_phong.Attach(shader_mgr.VertexShader(
        GLSLshader_path + "screen.vs"));
    sh_defer_phong.Attach(shader_mgr.FragmentShader(
        GLSLshader_path + "deferred_phong.fs"));
    bool ret_defer_phong = sh_defer_phong.Link();
    if (!ret_defer_phong) {
        std::cout << "error on shader 'deferred phong'" << std::endl;
    }
    shaders.insert(std::make_pair("defer phong", sh_defer_phong));
    // deferred blinn
    Shader sh_defer_blinn;
    sh_defer_blinn.Attach(shader_mgr.VertexShader(
        GLSLshader_path + "screen.vs"));
    sh_defer_blinn.Attach(shader_mgr.FragmentShader(
        GLSLshader_path + "deferred_blinn.fs"));
    bool ret_defer_blinn = sh_defer_blinn.Link();
    if (!ret_defer_blinn) {
        std::cout << "error on shader 'deferred blinn'" << std::endl;
    }
    shaders.insert(std::make_pair("defer blinn", sh_defer_blinn));
    // deferred cook-torrance
    // Shader sh_defer_torrance;
    // sh_defer_torrance.Attach(shader_mgr.VertexShader(
    //     GLSLshader_path + "screen.vs"));
    // sh_defer_torrance.Attach(shader_mgr.FragmentShader(
    //     GLSLshader_path + "deferred_torrance.fs"));
    // bool ret_defer_torrance = sh_defer_torrance.Link();
    // if (!ret_defer_torrance) {
    //     std::cout << "error on shader 'deferred cook-torrance'" << std::endl;
    // }
    // shaders.insert(std::make_pair("defer torrance", sh_defer_torrance));
    // deferred normal
    Shader sh_defer_normal;
    sh_defer_normal.Attach(shader_mgr.VertexShader(
        GLSLshader_path + "screen.vs"));
    sh_defer_normal.Attach(shader_mgr.FragmentShader(
        GLSLshader_path + "deferred_normal.fs"));
    bool ret_defer_normal = sh_defer_normal.Link();
    if (!ret_defer_normal) {
        std::cout << "error on shader 'deferred normal'" << std::endl;
    }
    shaders.insert(std::make_pair("defer normal", sh_defer_normal));
    // screen
    Shader sh_screen;
    sh_screen.Attach(shader_mgr.VertexShader(GLSLshader_path + "screen.vs"));
    sh_screen.Attach(shader_mgr.FragmentShader(GLSLshader_path + "screen.fs"));
    bool ret_screen = sh_screen.Link();
    if (!ret_screen) {
        std::cout << "error on shader 'screen'" << std::endl;
    }
    shaders.insert(std::make_pair("screen", sh_screen));
    // cube map
    Shader sh_cubemap;
    sh_cubemap.Attach(shader_mgr.VertexShader(GLSLshader_path + "cubemap.vs"));
    sh_cubemap.Attach(shader_mgr.FragmentShader(GLSLshader_path + "cubemap.fs"));
    bool ret_cubemap = sh_cubemap.Link();
    if (!ret_cubemap) {
        std::cout << "error on shader 'cubemap'" << std::endl;
    }
    shaders.insert(std::make_pair("cubemap", sh_cubemap));
    // depth map
    Shader sh_depth;
    sh_depth.Attach(shader_mgr.VertexShader(GLSLshader_path + "P3.vs"));
    sh_depth.Attach(shader_mgr.FragmentShader(GLSLshader_path + "P3_depth.fs"));
    bool ret_depth = sh_depth.Link();
    if (!ret_depth) {
        std::cout << "error on shader 'depth'" << std::endl;
    }
    shaders.insert(std::make_pair("depth", sh_depth));
    // cube depth map
    Shader sh_cube_depth;
    sh_cube_depth.Attach(shader_mgr.VertexShader(
        GLSLshader_path + "P3_cube_depth.vs"));
    sh_cube_depth.Attach(shader_mgr.FragmentShader(
        GLSLshader_path + "P3_cube_depth.fs"));
    sh_cube_depth.Attach(shader_mgr.GeometryShader(
        GLSLshader_path + "P3_cube_depth.gs"));
    bool ret_cube_depth = sh_cube_depth.Link();
    if (!ret_cube_depth) {
        std::cout << "error on shader 'cube_depth'" << std::endl;
    }
    shaders.insert(std::make_pair("cube depth", sh_cube_depth));
}

void OpenGLViewer::CreateFrameBuffers() {
    // default
    FrameBuffer fb_default = FrameBuffer::DefaultBuffer(config.width, config.height);
    framebuffers["default"] = fb_default;
    // simple buffer
    FrameBuffer fb_simple = FrameBuffer::SimpleBuffer(config.width, config.height);
    framebuffers["simple"] = fb_simple;
    // G-buffers
    FrameBuffer fb_gbuffer = FrameBuffer::GBuffer(config.width, config.height);
    framebuffers["G-buffer"] = fb_gbuffer;
}

}