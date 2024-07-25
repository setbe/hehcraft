#pragma once

#include "game_object.hpp"
#include "window.hpp"

namespace heh {

class KeyboardController {
 public:
  struct KeyMap {
    int move_left = GLFW_KEY_A;
    int move_right = GLFW_KEY_D;
    int move_forward = GLFW_KEY_W;
    int move_backward = GLFW_KEY_S;
    int move_up = GLFW_KEY_E;
    int move_down = GLFW_KEY_Q;
    int look_left = GLFW_KEY_LEFT;
    int look_right = GLFW_KEY_RIGHT;
    int look_up = GLFW_KEY_UP;
    int look_down = GLFW_KEY_DOWN;
  };  // struct KeyMap

  void MoveInPlaneXZ(
      GLFWwindow *window, 
      GameObject &game_object, 
      float delta_time);

  void Look(
      GLFWwindow *window, 
      GameObject &game_object);

  KeyMap key{};
  float movement_speed{3.f};
  float look_speed{1.5f};

};  // class KeyboardController

}  // namespace heh