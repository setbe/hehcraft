#include "core/keyboard_controller.hpp"

namespace heh
{
  void KeyboardController::MoveInPlaneXZ(
    GLFWwindow *window, 
    GameObject &game_object, 
    float      deltaTime)
  {
    glm::vec3 rotate{0};
    if (glfwGetKey(window, key.look_right) == GLFW_PRESS) rotate.y += 1.f;
    if (glfwGetKey(window, key.look_left) == GLFW_PRESS) rotate.y -= 1.f;
    if (glfwGetKey(window, key.look_up) == GLFW_PRESS) rotate.x += 1.f;
    if (glfwGetKey(window, key.look_down) == GLFW_PRESS) rotate.x -= 1.f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
      game_object.transform.rotation += look_speed * deltaTime * glm::normalize(rotate);

    game_object.transform.rotation.x = glm::clamp(game_object.transform.rotation.x, -1.5f, 1.5f);
    game_object.transform.rotation.y = glm::mod(game_object.transform.rotation.y, glm::two_pi<float>());

    float yaw = game_object.transform.rotation.y;
    const glm::vec3 forward_dir{sin(yaw), 0.f, cos(yaw)};
    const glm::vec3 right_dir{forward_dir.z, 0.f, -forward_dir.x};
    const glm::vec3 up_dir{0.f, -1.f, 0.f};

    glm::vec3 move_dir{0.f};
    if (glfwGetKey(window, key.move_forward) == GLFW_PRESS) move_dir += forward_dir;
    if (glfwGetKey(window, key.move_backward) == GLFW_PRESS) move_dir -= forward_dir;
    if (glfwGetKey(window, key.move_right) == GLFW_PRESS) move_dir += right_dir;
    if (glfwGetKey(window, key.move_left) == GLFW_PRESS) move_dir -= right_dir;
    if (glfwGetKey(window, key.move_up) == GLFW_PRESS) move_dir += up_dir;
    if (glfwGetKey(window, key.move_down) == GLFW_PRESS) move_dir -= up_dir;

    if (glm::dot(move_dir, move_dir) > std::numeric_limits<float>::epsilon())
      game_object.transform.translation += movement_speed * deltaTime * glm::normalize(move_dir);
  }

} // namespace heh