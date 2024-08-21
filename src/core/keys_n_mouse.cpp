#include "core/keys_n_mouse.hpp"

namespace heh {

std::bitset<GLFW_KEY_LAST + 1> Keyboard::keys_ = 0;
std::bitset<GLFW_KEY_LAST + 1> Keyboard::keys_released_ = 0;
std::bitset<GLFW_KEY_LAST + 1> Keyboard::keys_held_ = 0;

bool Mouse::buttons_[GLFW_MOUSE_BUTTON_LAST + 1] = {false};
bool Mouse::buttons_released_[GLFW_MOUSE_BUTTON_LAST + 1] = {false};
bool Mouse::buttons_held_[GLFW_MOUSE_BUTTON_LAST + 1] = {false};
double Mouse::x_ = 0.0;
double Mouse::y_ = 0.0;
double Mouse::dx_ = 0.0;
double Mouse::dy_ = 0.0;
double Mouse::scroll_x_ = 0.0;
double Mouse::scroll_y_ = 0.0;

void Mouse::HandleMouseButton(int button, int action, int mods) noexcept {
  buttons_[button] = action;  // GLFW_PRESS or GLFW_RELEASE
}

void Mouse::HandleCursorPosition(double xpos, double ypos) noexcept {
  dx_ = xpos - x_;  // change in x-coordinate
  dy_ = ypos - y_;  // change in y-coordinate
  x_ = xpos;  // new x-coordinate
  y_ = ypos;  // new y-coordinate
}

void Mouse::HandleScroll(double xoffset, double yoffset) noexcept {
  scroll_x_ = xoffset;  // value in range [-1, 1]
  scroll_y_ = yoffset;  // value in range [-1, 1]
}

}  // namespace heh