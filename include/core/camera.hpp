#pragma once

// Libraries
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace heh {

class Camera {
 public:
  void SetOrthographicProjection(
      float left, float right, 
      float bottom, float top, 
      float near, float far);

  void SetPerspectiveProjection(
      float fovy, float aspect, 
      float near, float far);

  void SetViewDirection(
      const glm::vec3& camera_position, 
      const glm::vec3& look_at_position, 
      const glm::vec3& up = {0.f, -1.f, 0.f});

  void SetViewTarget(
      const glm::vec3& camera_position, 
      const glm::vec3& target_position, 
      const glm::vec3& up = {0.f, -1.f, 0.f});

  void SetViewYXZ(const glm::vec3& camera_position, const glm::vec3& rotation);

  const glm::mat4& GetProjection() const { return proj_matrix_; }
  const glm::mat4& GetView() const { return view_matrix_; }
  const glm::mat4& GetInverseView() const { return inverse_view_matrix_; }
  const glm::vec3 GetPosition() const { return glm::vec3(inverse_view_matrix_[3]); }

 private:
  glm::mat4 proj_matrix_{1.f};
  glm::mat4 view_matrix_{1.f};
  glm::mat4 inverse_view_matrix_{1.f};
};

}  // namespace heh
