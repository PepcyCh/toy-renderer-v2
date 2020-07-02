#include "InputManager.h"

static double scroll_yoffset = 0;

static void GlfwScrollCallback(GLFWwindow *win, double xoffset, double yoffset) {
    scroll_yoffset = yoffset;
}

namespace pepcy::renderer {

void InputManager::BindWindow(const Window *p) {
    p_window = p;
    glfwSetScrollCallback(p_window->window, GlfwScrollCallback);
}

bool InputManager::IsValid() const {
    return p_window != nullptr;
}

int InputManager::GetKeyState(int key) const {
    return IsValid() ? glfwGetKey(p_window->window, key) : -1;
}

int InputManager::GetMouseState(int btn) const {
    return IsValid() ? glfwGetMouseButton(p_window->window, btn) : -1;
}

double InputManager::GetScroll() const {
    double ret = scroll_yoffset;
    scroll_yoffset = 0;
    return ret;
}

std::pair<double, double> InputManager::GetCursorPosition() const {
    if (!IsValid())
        return std::make_pair(0.0 / 0.0, 0.0 / 0.0);
    double x, y;
    glfwGetCursorPos(p_window->window, &x, &y);
    last_x = x;
    last_y = y;
    return std::make_pair(x, y);
}

std::pair<double, double> InputManager::GetCursorDelta() const {
    if (!IsValid())
        return std::make_pair(0.0 / 0.0, 0.0 / 0.0);
    double x, y;
    glfwGetCursorPos(p_window->window, &x, &y);
    double deltaX = x - last_x, delatY = y - last_y;
    last_x = x;
    last_y = y;
    return std::make_pair(deltaX, delatY);
}

}