#include "core/keys_n_mouse.hpp"

namespace heh {

bool Keyboard::keys_[GLFW_KEY_LAST + 1] = {false};
bool Keyboard::keys_released_[GLFW_KEY_LAST + 1] = {false};
bool Keyboard::keys_held_[GLFW_KEY_LAST + 1] = {false};


bool Mouse::buttons_[GLFW_MOUSE_BUTTON_LAST + 1] = {false};
bool Mouse::buttons_released_[GLFW_MOUSE_BUTTON_LAST + 1] = {false};
bool Mouse::buttons_held_[GLFW_MOUSE_BUTTON_LAST + 1] = {false};
double Mouse::x_ = 0.0;
double Mouse::y_ = 0.0;
double Mouse::dx_ = 0.0;
double Mouse::dy_ = 0.0;
double Mouse::scroll_x_ = 0.0;
double Mouse::scroll_y_ = 0.0;

void Mouse::HandleMouseButton(int button, int action, int mods) {
  buttons_[button] = action;
}

void Mouse::HandleCursorPosition(double xpos, double ypos) {
  dx_ = xpos - x_;
  dy_ = ypos - y_;
  x_ = xpos;
  y_ = ypos;
}

void Mouse::HandleScroll(double xoffset, double yoffset) {
  scroll_x_ = xoffset;
  scroll_y_ = yoffset;
}

}  // namespace heh