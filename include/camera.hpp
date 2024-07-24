#pragma once

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace lve
{
class Camera
{
  glm::mat4 projMatrix{1.f};
  glm::mat4 viewMatrix{1.f};

public:
  void setOrthographicProjection(
    float left, float right, 
    float bottom, float top, 
    float near, float far);
  
  void setPerspectiveProjection(
    float fovy, float aspect, 
    float near, float far);

  void setViewDirection(
    const glm::vec3& cameraPosition, 
    const glm::vec3& lookAtPosition, 
    const glm::vec3& up = {0.f, -1.f, 0.f});

  void setViewTarget(
    const glm::vec3& cameraPosition, 
    const glm::vec3& targetPosition, 
    const glm::vec3& up = {0.f, -1.f, 0.f});

  void setViewYXZ(const glm::vec3& cameraPosition, const glm::vec3& rotation);

  const glm::mat4& getProjection() const { return projMatrix; }
  const glm::mat4& getView() const { return viewMatrix; }
    
}; // class Camera

} // namespace lve
