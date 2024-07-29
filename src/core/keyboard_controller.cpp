
#define GLM_ENABLE_EXPERIMENTAL
#include "core/keyboard_controller.hpp"
#include <glm/gtx/euler_angles.hpp>

namespace heh {

void KeyboardController::MoveInPlaneXZ(
    GLFWwindow *window, 
    GameObject &game_object, 
    float delta_time) 
{
  float yaw = game_object.transform.rotation.y;
  const glm::vec3 forward_dir{sin(yaw), 0.f, cos(yaw)};
  const glm::vec3 right_dir{forward_dir.z, 0.f, -forward_dir.x};
  const glm::vec3 up_dir{0.f, 1.f, 0.f};

  glm::vec3 move_dir{0.f};
  if (glfwGetKey(window, key.move_forward) == GLFW_PRESS) move_dir += forward_dir;
  if (glfwGetKey(window, key.move_backward) == GLFW_PRESS) move_dir -= forward_dir;
  if (glfwGetKey(window, key.move_right) == GLFW_PRESS) move_dir += right_dir;
  if (glfwGetKey(window, key.move_left) == GLFW_PRESS) move_dir -= right_dir;
  if (glfwGetKey(window, key.move_up) == GLFW_PRESS) move_dir -= up_dir;    // Move up
  if (glfwGetKey(window, key.move_down) == GLFW_PRESS) move_dir += up_dir;  // Move down

  if (glm::dot(move_dir, move_dir) > std::numeric_limits<float>::epsilon())
    game_object.transform.translation += movement_speed * delta_time * glm::normalize(move_dir);
}

void KeyboardController::Look(
    GLFWwindow *window, 
    GameObject &game_object, 
    float delta_time) 
{
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  if (first_mouse) {
    last_x = xpos;
    last_y = ypos;
    first_mouse = false;
  }

  float xoffset = xpos - last_x;
  float yoffset = last_y - ypos; // Inverted since y-coordinates range from bottom to top
  last_x = xpos;
  last_y = ypos;

  float sensitivity = look_speed / 100.f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  // Make sure that when pitch is out of bounds, screen doesn't get flipped
  if (pitch > 89.0f) pitch = 89.0f;
  if (pitch < -89.0f) pitch = -89.0f;

  game_object.transform.rotation = glm::vec3{pitch, yaw, 0.0f};
}

void KeyboardController::UpdateCursorState(GLFWwindow *window) {
  if (glfwGetKey(window, key.show_cursor) == GLFW_PRESS) {
    if (!cursor_visible) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      cursor_visible = true;
      first_mouse = true; // Reset to avoid large jump when cursor is hidden again
    }
  } else {
    if (cursor_visible) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      cursor_visible = false;
    }
  }
}

} // namespace heh
