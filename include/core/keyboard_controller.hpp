#pragma once

#include "core/game_object.hpp"
#include "core/window.hpp"

namespace heh {

class KeyboardController {
 public:
  struct KeyMap {
    int move_left = GLFW_KEY_A;
    int move_right = GLFW_KEY_D;
    int move_forward = GLFW_KEY_W;
    int move_backward = GLFW_KEY_S;
    int move_up = GLFW_KEY_SPACE;
    int move_down = GLFW_KEY_LEFT_SHIFT;
    int show_cursor = GLFW_KEY_LEFT_ALT;
  };  // struct KeyMap

  void MoveInPlaneXZ(
      GLFWwindow *window, 
      GameObject &game_object, 
      float delta_time);

  void Look(
      GLFWwindow *window, 
      GameObject &game_object, 
      float delta_time);

  void UpdateCursorState(GLFWwindow *window);

  KeyMap key{};
  float movement_speed{3.f};
  float look_speed{0.1f};
  bool first_mouse{true};
  float last_x{400.0f};
  float last_y{300.0f};
  float yaw{0.0f};
  float pitch{0.0f};
  bool cursor_visible{false};

};  // class KeyboardController

}  // namespace heh