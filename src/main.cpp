#include <fstream>

#include "defines.h"
#include "BasicShape.h"
#include "InputManager.h"
#include "OpenGLViewer.h"
#include "RayTraceViewer.h"
#include "OBJLoader.h"
#include "OBJSaver.h"

using namespace pepcy;
using namespace pepcy::renderer;

void MoveCamera(const InputManager &input, Camera *cam,
        float delta, const gm::Vector3 &focus) {
    static bool focused = false;

    if (input.GetKeyState(GLFW_KEY_W) == GLFW_PRESS) {
        if (input.GetKeyState(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            if (focused) {
                cam->OrbitV(-delta, focus);
            }
        } else {
            cam->MoveForward(delta * 2.0f);
        }
    } else if (input.GetKeyState(GLFW_KEY_S) == GLFW_PRESS) {
        if (input.GetKeyState(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            if (focused) {
                cam->OrbitV(delta, focus);
            }
        } else {
            cam->MoveForward(-delta * 2.0f);
        }
    }
    if (input.GetKeyState(GLFW_KEY_A) == GLFW_PRESS) {
        if (input.GetKeyState(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            if (focused) {
                cam->OrbitH(-delta, focus);
            }
        } else {
            cam->MoveRight(-delta * 2.0f);
            focused = false;
        }
    } else if (input.GetKeyState(GLFW_KEY_D) == GLFW_PRESS) {
        if (input.GetKeyState(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            if (focused) {
                cam->OrbitH(delta, focus);
            }
        } else {
            cam->MoveRight(delta * 2.0f);
            focused = false;
        }
    }
    if (input.GetKeyState(GLFW_KEY_Q) == GLFW_PRESS) {
        cam->MoveUp(delta * 2.0f);
        focused = false;
    } else if (input.GetKeyState(GLFW_KEY_E) == GLFW_PRESS) {
        cam->MoveUp(-delta * 2.0f);
        focused = false;
    }
    if (input.GetKeyState(GLFW_KEY_F) == GLFW_PRESS) {
        cam->LookAt(focus);
        focused = true;
    }

    const auto &[delta_x, delta_y] = input.GetCursorDelta();
    if (input.GetMouseState(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        cam->Rotate(delta_x * 0.005, delta_y * 0.005);
        focused = false;
    }

    double scroll = input.GetScroll();
    if (scroll > 0) {
        cam->ZoomIn(scroll * delta);
    } else if (scroll < 0) {
        cam->ZoomOut(-scroll * delta);
    }
}

struct ImGuiFileBrowser {
    ImGuiFileBrowser() {
        std::fill(path_cstr, path_cstr + 256, 0);
    }
    ImGuiFileBrowser(const std::string &title, const std::string &prefix) :
            title(title), prefix(prefix) {
        std::fill(path_cstr, path_cstr + 256, 0);
    }

    void SetPrefix(const std::string &pre) {
        prefix = pre;
    }
    void SetTitle(const std::string &t) {
        title = t;
    }

    bool Display() {
        if (show) {
            bool ret = false;
            ImGui::Begin(title.c_str());
            ImGui::Text(prefix.c_str());
            ImGui::InputText("", path_cstr, IM_ARRAYSIZE(path_cstr));
            if (ImGui::Button("OK")) {
                std::ifstream fin(prefix + path_cstr);
                if (!check || fin) {
                    path = prefix + path_cstr;
                    ret = true;
                    show = false;
                    std::fill(path_cstr, path_cstr + 256, 0);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                show = false;
            }
            ImGui::End();
            return ret;
        } else {
            return false;
        }
    }
    
    std::string prefix, title, path;
    char path_cstr[256];
    bool show = false, check = true;
};

int main() {
    WindowConfig config;
    config.width = 2400;
    config.height = 1350;
    config.title = "toy-renderer";
    Window win(config);
    if (!win.IsValid()) {
        return -1;
    }

    InputManager input;
    input.BindWindow(&win);

    float cam_aspect = double(config.width) / config.height;
    Camera cam(gm::Vector3(3, 5, 2), gm::Vector3(0, 0, 0),
        gm::Vector3(0, 1, 0), gm::Radians(90.0f), cam_aspect);
    auto pcam = &cam;

    OBJLoader loader;
    auto scene = loader.ReadFile(scene_path + "cube_texture/cube_texture.obj");
    // auto scene = loader.ReadFile(scene_path + "cup.obj");
    auto pscene = &scene;
    OBJSaver saver;

    OpenGLViewerConfig viewer_config;
    viewer_config.cam = pcam;
    viewer_config.width = config.width;
    viewer_config.height = config.height;
    viewer_config.is_deferred = false;
    viewer_config.light_model = OpenGLViewerConfig::LightModel::Normal;
    viewer_config.scene = pscene;
    viewer_config.skybox_path = scene_path + "skybox/";
    viewer_config.skybox_suffix = ".jpg";
    viewer_config.skybox = true;
    opengl_viewer.SetConfig(viewer_config);

    RayTraceViewerConfig raytrace_config;
    raytrace_config.width = config.width / 4;
    raytrace_config.height = config.height / 4;
    raytrace_config.cam = pcam;
    raytrace_config.scene = pscene;
    raytrace_viewer.SetConfig(raytrace_config);

    ImGuiIO &io = ImGui::GetIO();
    ImFontConfig font_config;
    font_config.SizePixels = 24.0f;
    io.Fonts->AddFontDefault(&font_config);

    std::unordered_map<std::string, ImGuiFileBrowser> tex_browsers;
    tex_browsers["albedo"] = ImGuiFileBrowser("File Browser - albedo", scene_path);
    tex_browsers["ambient"] = ImGuiFileBrowser("File Browser - ambient", scene_path);
    tex_browsers["specular"] = ImGuiFileBrowser("File Browser - specular", scene_path);
    tex_browsers["normal"] = ImGuiFileBrowser("File Browser - normal", scene_path);
    ImGuiFileBrowser load_browser("File Browser - load obj", scene_path);
    ImGuiFileBrowser save_browser("File Browser - save obj", scene_path);
    save_browser.check = false;
    ImGuiFileBrowser skybox_browser("File Browser - skybox", scene_path);
    skybox_browser.check = false;

    win.main_loop = [&]() {
        opengl_viewer.Draw();

        const auto &[width, height] = win.GetGeometry();
        viewer_config.width = width;
        viewer_config.height = height;
        pcam->SetAspect(double(width) / height);
        auto cam_lookat = gm::Vector3(0.0f, 0.0f, 0.0f);

        {
            ImGui::Begin("OpenGL Renderer");

            double fps = win.GetFPS();
            ImGui::Text("FPS: %.2f", fps);
            ImGui::Separator();

            static int light_model = 0;
            ImGui::RadioButton("Normal", &light_model, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Phong", &light_model, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Blinn-Phong", &light_model, 2);
            static bool deferred = false;
            ImGui::Checkbox("deferred rendering", &deferred);

            ImGui::End();

            if (light_model == 0) {
                viewer_config.light_model = OpenGLViewerConfig::LightModel::Normal;
            } else if (light_model == 1) {
                viewer_config.light_model = OpenGLViewerConfig::LightModel::Phong;
            } else {
                viewer_config.light_model = OpenGLViewerConfig::LightModel::BlinnPhong;
            }

            viewer_config.is_deferred = deferred;
        }
        {
            ImGui::Begin("Scene");

            static auto it_dir_light = pscene->GetDirLights().begin();
            static auto it_point_light = pscene->GetPointLights().begin();
            static auto it_spot_light = pscene->GetSpotLights().begin();
            static auto it_mesh = pscene->GetMeshes().begin();

            // load file browser
            if (ImGui::Button("load obj")) {
                load_browser.show = true;
            }
            if (load_browser.Display()) {
                auto new_scene = loader.ReadFile(load_browser.path);
                pscene->Clear();
                *pscene = new_scene;
                viewer_config.scene = pscene;
                raytrace_config.scene = pscene;
                it_dir_light = pscene->GetDirLights().begin();
                it_point_light = pscene->GetPointLights().begin();
                it_spot_light = pscene->GetSpotLights().begin();
                it_mesh = pscene->GetMeshes().begin();
                opengl_viewer.Clear();
            }

            // save file browser
            ImGui::SameLine();
            if (ImGui::Button("save obj")) {
                save_browser.show = true;
            }
            if (save_browser.Display()) {
                int len = scene_path.size();
                auto filename = save_browser.path.substr(len, save_browser.path.size());
                saver.SaveScene(scene_path, filename, *pscene);
            }

            // screen shot
            if (ImGui::Button("screen shot")) {
                opengl_viewer.ScreenShot();
            }
            // ray trace
            ImGui::SameLine();
            if (ImGui::Button("ray trace")) {
                raytrace_viewer.Draw();
            }

            // skybox
            ImGui::Separator();
            ImGui::Checkbox("skybox", &viewer_config.skybox);
            ImGui::SameLine();
            if (ImGui::Button("load skybox")) {
                skybox_browser.show = true;
            }
            if (skybox_browser.Display()) {
                std::string name(skybox_browser.path);
                int pos = name.rfind('.');
                if (pos != -1) {
                    viewer_config.skybox_path = name.substr(0, pos);
                    viewer_config.skybox_suffix = name.substr(pos + 1, name.size());
                }
            }
            ImGui::Separator();

            // scene tree
            ImGuiTabBarFlags flag = ImGuiTabBarFlags_Reorderable;
            flag |= ImGuiTabBarFlags_TabListPopupButton;
            flag |= ImGuiTabBarFlags_AutoSelectNewTabs;
            flag |= ImGuiTabBarFlags_FittingPolicyScroll;
            flag |= ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;
            const char *names[] = { "Dir Lights", "Point Lights", "Spot Lights", "Models" };
            static bool opened[] = { true, true, true, true };
            bool input_prev = false; // TODO
            bool input_next = false; // TODO
            if (ImGui::BeginTabBar("Scene", flag)) {
                if (opened[0] && ImGui::BeginTabItem(names[0], &opened[0])) {
                    ImGui::Text("Directional Lights");
                    auto &dir_lights = pscene->GetDirLights();
                    auto &it = it_dir_light;
                    if (it == dir_lights.end()) {
                        ;
                    } else if (ImGui::Button("delete")) {
                        it = dir_lights.erase(it);
                        if (it == dir_lights.end() && it != dir_lights.begin()) {
                            --it;
                        }
                    } else {
                        float col[] = { it->color.r, it->color.g, it->color.b };
                        ImGui::ColorEdit3("color", col);
                        float dir[] = { it->dir[0], it->dir[1], it->dir[2] };
                        ImGui::DragFloat3("direction", dir, 0.05f);
                        ImGui::Checkbox("shadow", &it->shadow);
                        ImGui::SameLine();
                        ImGui::Checkbox("real-time", &it->realtime);
                        it->color = gm::Color(col[0], col[1], col[2]);
                        it->dir = gm::Normalize(gm::Vector3(dir[0], dir[1], dir[2]));
                        if (ImGui::Button("generate shadow map")) {
                            opengl_viewer.GenerateDepthBuffer(*it);
                        }
                    }
                    ImGui::Separator();
                    if (it != dir_lights.begin() && (ImGui::Button("prev") || input_prev)) {
                        --it;
                    }
                    if (it != dir_lights.end()) {
                        ++it;
                        if (it == dir_lights.end() || (!ImGui::Button("next") && !input_next)) {
                            --it;
                        }
                    }
                    ImGui::Separator();
                    if (ImGui::Button("add")) {
                        dir_lights.emplace_back(gm::Color(1, 1, 1), gm::Vector3(0, -1, 0));
                        it = dir_lights.end();
                        --it;
                    }
                    ImGui::EndTabItem();
                }
                if (opened[1] && ImGui::BeginTabItem(names[1], &opened[1])) {
                    ImGui::Text("Point Lights");
                    auto &point_lights = pscene->GetPointLights();
                    auto &it = it_point_light;
                    if (it == point_lights.end()) {
                        ;
                    } else if (ImGui::Button("delete")) {
                        it = point_lights.erase(it);
                        if (it == point_lights.end() && it != point_lights.begin()) {
                            --it;
                        }
                    } else {
                        float col[] = { it->color.r, it->color.g, it->color.b };
                        ImGui::ColorEdit3("color", col);
                        float pos[] = { it->pos[0], it->pos[1], it->pos[2] };
                        ImGui::DragFloat3("position", pos, 0.05);
                        ImGui::Checkbox("shadow", &it->shadow);
                        ImGui::SameLine();
                        ImGui::Checkbox("real-time", &it->realtime);
                        float k[] = { it->kq, it->kl, it->kc };
                        ImGui::DragFloat3("coefficients", k);
                        it->color = gm::Color(col[0], col[1], col[2]);
                        it->pos = gm::Vector3(pos[0], pos[1], pos[2]);
                        it->kq = k[0];
                        it->kl = k[1];
                        it->kc = k[2];
                        if (ImGui::Button("generate shadow map")) {
                            opengl_viewer.GenerateDepthBuffer(*it);
                        }
                    }
                    ImGui::Separator();
                    if (it != point_lights.begin() && (ImGui::Button("prev") || input_prev)) {
                        --it;
                    }
                    if (it != point_lights.end()) {
                        ++it;
                        if (it == point_lights.end() || (!ImGui::Button("next") && !input_next)) {
                            --it;
                        }
                    }
                    ImGui::Separator();
                    if (ImGui::Button("add")) {
                        point_lights.emplace_back(gm::Color(1, 1, 1), gm::Vector3(0, 1, 0));
                        it = point_lights.end();
                        --it;
                    }
                    ImGui::EndTabItem();
                }
                if (opened[2] && ImGui::BeginTabItem(names[2], &opened[2])) {
                    ImGui::Text("Spot Lights");
                    auto &spot_lights = pscene->GetSpotLights();
                    auto &it = it_spot_light;
                    if (it == spot_lights.end()) {
                        ;
                    } else if (ImGui::Button("delete")) {
                        it = spot_lights.erase(it);
                        if (it == spot_lights.end() && it != spot_lights.begin()) {
                            --it;
                        }
                    } else {
                        float col[] = { it->color.r, it->color.g, it->color.b };
                        ImGui::ColorEdit3("color", col);
                        float pos[] = { it->pos[0], it->pos[1], it->pos[2] };
                        ImGui::DragFloat3("position", pos , 0.05f);
                        float dir[] = { it->dir[0], it->dir[1], it->dir[2] };
                        ImGui::DragFloat3("direction", dir, 0.05f);
                        ImGui::Checkbox("shadow", &it->shadow);
                        ImGui::SameLine();
                        ImGui::Checkbox("real-time", &it->realtime);
                        float k[] = { it->kq, it->kl, it->kc };
                        ImGui::DragFloat3("coefficients", k);
                        float cutoff[] = { gm::Degree(it->cutoff), gm::Degree(it->outer_cutoff) };
                        ImGui::DragFloat2("cutoff", cutoff);
                        it->color = gm::Color(col[0], col[1], col[2]);
                        it->pos = gm::Vector3(pos[0], pos[1], pos[2]);
                        it->dir = gm::Normalize(gm::Vector3(dir[0], dir[1], dir[2]));
                        it->kq = k[0];
                        it->kl = k[1];
                        it->kc = k[2];
                        it->cutoff = gm::Radians(std::clamp(cutoff[0], 0.0f, 180.0f));
                        it->outer_cutoff = gm::Radians(std::clamp(cutoff[1], cutoff[0], 180.0f));
                        if (ImGui::Button("generate shadow map")) {
                            opengl_viewer.GenerateDepthBuffer(*it);
                        }
                    }
                    ImGui::Separator();
                    if (it != spot_lights.begin() && (ImGui::Button("prev") || input_prev)) {
                        --it;
                    }
                    if (it != spot_lights.end()) {
                        ++it;
                        if (it == spot_lights.end() || (!ImGui::Button("next") && input_next)) {
                            --it;
                        }
                    }
                    ImGui::Separator();
                    if (ImGui::Button("add")) {
                        spot_lights.emplace_back(gm::Color(1, 1, 1),
                            gm::Vector3(0, 1, 0), gm::Vector3(0, -1, 0));
                        it = spot_lights.end();
                        --it;
                    }
                    ImGui::EndTabItem();
                }
                if (opened[3] && ImGui::BeginTabItem(names[3], &opened[3])) {
                    ImGui::Text("Models");
                    auto &meshes = pscene->GetMeshes();
                    auto &it = it_mesh;
                    if (it == meshes.end()) {
                        ;
                    } else if (ImGui::Button("delete")) {
                        it = meshes.erase(it);
                        if (it == meshes.end() && it != meshes.begin()) {
                            --it;
                        }
                    } else {
                        ImGui::Separator();
                        ImGui::Text("Geometry");
                        const auto &cen = (*it)->GetCentroid();
                        cam_lookat = cen;
                        auto trans = (*it)->GetModel();
                        auto t_before = gm::Translate(-cen[0], -cen[1], -cen[2]);

                        float pos[] = { cen[0], cen[1], cen[2] };
                        ImGui::DragFloat3("position", pos, 0.05f);
                        auto t_translate = gm::Translate(pos[0], pos[1], pos[2]);
                        float scale[] = { 1.0f, 1.0f, 1.0f };
                        ImGui::DragFloat3("scale", scale, 0.05f);
                        auto t_scale = gm::Scale(scale[0], scale[1], scale[2]);
                        float rotate[] = { 0.0f, 0.0f, 0.0f };
                        ImGui::DragFloat3("rotate", rotate, 0.05f);
                        auto t_rotate = gm::RotateZ(rotate[2]) * 
                            gm::RotateY(rotate[1]) * gm::RotateX(rotate[0]);

                        auto t_model = t_translate * t_rotate * t_scale * t_before;
                        trans = gm::Transform(t_model) * trans;
                        (*it)->SetModel(trans);

                        ImGui::Separator();
                        ImGui::Text("Materials");
                        auto &mat = (*it)->GetMaterial();
                        int n_tex = 0;
                        for (auto &[name, tex] : mat.GetTextures()) {
                            if (name == "emissive") {
                                continue;
                            } else if (name == "exponent") {
                                ImGui::Separator();
                                float exp = 1.0f;
                                if (tex.IsColor()) {
                                    exp = tex.GetColor().r;
                                }
                                ImGui::DragFloat("exponent", &exp, 0.02f);
                                if (tex.IsColor()) {
                                    tex.SetColor(gm::Color(exp, exp, exp));
                                }
                                continue;
                            }
                            ++n_tex;
                            ImGui::Separator();
                            float col[] = { 0.0f, 0.0f, 0.0f };
                            auto color = tex.GetColor();
                            col[0] = color.r;
                            col[1] = color.g;
                            col[2] = color.b;
                            if (name != "normal") {
                                ImGui::ColorEdit3(name.c_str(), col);
                            } else {
                                ImGui::Text("normal");
                            }
                            
                            auto gui_str = "browse image##" + std::to_string(n_tex);
                            if (ImGui::Button(gui_str.c_str())) {
                                tex_browsers[name].show = true;
                            }
                            if (tex_browsers[name].Display()) {
                                tex.SetPath(tex_browsers[name].path);
                            }
                            bool is_img = !tex.IsColor();
                            gui_str = "is image##" + std::to_string(n_tex);
                            ImGui::Checkbox(gui_str.c_str(), &is_img);
                            if (is_img == tex.IsColor()) {
                                tex.Switch();
                            }
                            if (!is_img) {
                                tex.SetColor(gm::Color(col[0], col[1], col[2]));
                            }
                        }
                    }
                    ImGui::Separator();
                    if (it != meshes.begin() && (ImGui::Button("prev") || input_prev)) {
                        --it;
                    }
                    if (it != meshes.end()) {
                        ++it;
                        if (it == meshes.end() || (!ImGui::Button("next") && !input_next)) {
                            --it;
                        }
                    }
                    ImGui::Separator();
                    ImGui::Text("Add");
                    if (ImGui::Button("Cube")) {
                        auto sh = new Cube();
                        sh->SetMaterial(Material::BasicPhongMat());
                        pscene->AddMesh(sh);
                        it = meshes.end();
                        --it;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Sphere")) {
                        auto sh = new Sphere();
                        sh->SetMaterial(Material::BasicPhongMat());
                        pscene->AddMesh(sh);
                        it = meshes.end();
                        --it;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Plane")) {
                        auto sh = new Plane();
                        sh->SetMaterial(Material::BasicPhongMat());
                        pscene->AddMesh(sh);
                        it = meshes.end();
                        --it;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cylinder")) {
                        auto sh = new Cylinder();
                        sh->SetMaterial(Material::BasicPhongMat());
                        pscene->AddMesh(sh);
                        it = meshes.end();
                        --it;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Capsule")) {
                        auto sh = new Capsule();
                        sh->SetMaterial(Material::BasicPhongMat());
                        pscene->AddMesh(sh);
                        it = meshes.end();
                        --it;
                    }
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            ImGui::End();
        }

        for (const auto &light : pscene->GetDirLights()) {
            if (light.shadow && light.realtime) {
                opengl_viewer.GenerateDepthBuffer(light);
            }
        }
        for (const auto &light : pscene->GetPointLights()) {
            if (light.shadow && light.realtime) {
                opengl_viewer.GenerateDepthBuffer(light);
            }
        }
        for (const auto &light : pscene->GetSpotLights()) {
            if (light.shadow && light.realtime) {
                opengl_viewer.GenerateDepthBuffer(light);
            }
        }

        MoveCamera(input, pcam, win.GetDeltaTime(), cam_lookat);

        opengl_viewer.SetConfig(viewer_config);
        raytrace_viewer.SetConfig(raytrace_config);
    };

    win.MainLoop();

    pscene->Clear();

    return 0;
}