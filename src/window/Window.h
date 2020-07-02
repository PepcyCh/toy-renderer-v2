#pragma once

#include <string>
#include <functional>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"

namespace pepcy::renderer {

// glfw window config
struct WindowConfig {
    // window size
    size_t width = 800;
    size_t height = 600;
    // window title
    std::string title;
    // OpenGL version
    int gl_major = 3;
    int gl_minor = 3;
    bool is_core = true;
};

class Window {
  public:
    explicit Window(const WindowConfig &config);
    ~Window();

    bool IsValid() const;
    void MainLoop();

    std::pair<int, int> GetGeometry();
    double GetAspect();
    double GetDeltaTime() const;
    double GetFPS() const;

    std::function<void()> main_loop;

  private:
    GLFWwindow *window;
    int width, height;
    double last_time, curr_time, base_time;
    int n_frame;
    double fps;

    friend class InputManager;
};

}