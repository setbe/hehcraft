#pragma once

#include "game_object.hpp"
#include "window.hpp"

namespace lve
{
class KeyboardController
{
public:
  struct KeyMap
  {
    int moveLeft = GLFW_KEY_A;
    int moveRight = GLFW_KEY_D;
    int moveForward = GLFW_KEY_W;
    int moveBackward = GLFW_KEY_S;
    int moveUp = GLFW_KEY_E;
    int moveDown = GLFW_KEY_Q;
    int lookLeft = GLFW_KEY_LEFT;
    int lookRight = GLFW_KEY_RIGHT;
    int lookUp = GLFW_KEY_UP;
    int lookDown = GLFW_KEY_DOWN;
  }; // struct KeyMap

  void moveInPlaneXZ(
    GLFWwindow *window, 
    LveGameObject &gameObject, 
    float deltaTime);

  void look(
    GLFWwindow *window, 
    LveGameObject &gameObject);

  KeyMap key{};
  float movementSpeed{3.f};
  float lookSpeed{1.5f};

}; // class KeyboardController

} // namespace lve