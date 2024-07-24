#include "keyboard_controller.hpp"

namespace lve
{
  void KeyboardController::moveInPlaneXZ(
    GLFWwindow *window, 
    LveGameObject &gameObject, 
    float deltaTime)
  {
    glm::vec3 rotate{0};
    if (glfwGetKey(window, key.lookRight) == GLFW_PRESS) rotate.y += 1.f;
    if (glfwGetKey(window, key.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
    if (glfwGetKey(window, key.lookUp) == GLFW_PRESS) rotate.x += 1.f;
    if (glfwGetKey(window, key.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
    {
      gameObject.transform.rotation += lookSpeed * deltaTime * glm::normalize(rotate);
    }

    gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
    gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

    float yaw = gameObject.transform.rotation.y;
    const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
    const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
    const glm::vec3 upDir{0.f, -1.f, 0.f};

    glm::vec3 moveDir{0.f};
    if (glfwGetKey(window, key.moveForward) == GLFW_PRESS) moveDir += forwardDir;
    if (glfwGetKey(window, key.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
    if (glfwGetKey(window, key.moveRight) == GLFW_PRESS) moveDir += rightDir;
    if (glfwGetKey(window, key.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
    if (glfwGetKey(window, key.moveUp) == GLFW_PRESS) moveDir += upDir;
    if (glfwGetKey(window, key.moveDown) == GLFW_PRESS) moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
    {
      gameObject.transform.translation += movementSpeed * deltaTime * glm::normalize(moveDir);
    }
  }

} // namespace lve