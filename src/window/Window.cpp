#include "glad/glad.h"
#include "Window.h"

#include <iostream>

static void GLFWErrorCallback(int error, const char* description) {
    std::cout << "GLFW Error " << error << ", message: " <<
        description << std::endl;
}

namespace pepcy::renderer {

Window::Window(const WindowConfig &config) {
    glfwSetErrorCallback(GLFWErrorCallback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.gl_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.gl_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, config.is_core ?
        GLFW_OPENGL_CORE_PROFILE : GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    width = config.width;
    height = config.height;

    window = glfwCreateWindow(config.width, config.height,
        config.title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Fail to create GLFW Window" << std::endl;
        glfwTerminate();
    } else {
        glfwMakeContextCurrent(window);
        // glfwSwapInterval(1);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cout << "Fail to load GLAD" << std::endl;
            glfwTerminate();
            window = nullptr;
        } else {
            glViewport(0, 0, config.width, config.height);
            std::cout << "GL_VERSION: " << glGetString(GL_VERSION) <<  std::endl;
            std::cout << "GL_VENDOR: " << glGetString(GL_VENDOR) <<  std::endl;
            std::cout << "GL_RENDERER: " << glGetString(GL_RENDERER) <<  std::endl;

            last_time = curr_time = base_time = glfwGetTime();
            n_frame = 0;

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            (void)io;

            ImGui::StyleColorsDark();

            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 330 core");
        }
    }
}

Window::~Window() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::IsValid() const {
    return window != nullptr;
}

std::pair<int, int> Window::GetGeometry() {
    glfwGetFramebufferSize(window, &width, &height);
    return { width, height };
}

double Window::GetAspect() {
    glfwGetFramebufferSize(window, &width, &height);
    return double(width) / height;
}

double Window::GetDeltaTime() const {
    return curr_time - last_time;
}

double Window::GetFPS() const {
    return fps;
}

void Window::MainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        main_loop();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        last_time = curr_time;
        curr_time = glfwGetTime();

        ++n_frame;
        if (curr_time - base_time > 1) {
            fps = n_frame / (curr_time - base_time);
            n_frame = 0;
            base_time = curr_time;
        }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }
}

}