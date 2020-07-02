#pragma once

#include "Window.h"

namespace pepcy::renderer {

class InputManager {
  public:
    void BindWindow(const Window *p);
    bool IsValid() const;

    int GetKeyState(int key) const;
    int GetMouseState(int btn) const;
    double GetScroll() const;

    std::pair<double, double> GetCursorPosition() const;
    std::pair<double, double> GetCursorDelta() const;

  private:
    const Window *p_window = nullptr;
    mutable double last_x, last_y;
};

}