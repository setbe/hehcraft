#include "camera.hpp"

#include <cassert>
#include <limits>

namespace heh
{

void Camera::SetOrthographicProjection(
  float left, float right, 
  float top, float bottom, 
  float near, float far) 
{
  proj_matrix_ = glm::mat4{1.0f};
  proj_matrix_[0][0] = 2.f / (right - left);
  proj_matrix_[1][1] = 2.f / (bottom - top);
  proj_matrix_[2][2] = 1.f / (far - near);
  proj_matrix_[3][0] = -(right + left) / (right - left);
  proj_matrix_[3][1] = -(bottom + top) / (bottom - top);
  proj_matrix_[3][2] = -near / (far - near);
}
 
void Camera::SetPerspectiveProjection(
  float fovy, 
  float aspect, 
  float near, float far) 
{
  assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
  const float tan_half_fovy = tan(fovy / 2.f);
  proj_matrix_ = glm::mat4{0.0f};
  proj_matrix_[0][0] = 1.f / (aspect * tan_half_fovy);
  proj_matrix_[1][1] = 1.f / (tan_half_fovy);
  proj_matrix_[2][2] = far / (far - near);
  proj_matrix_[2][3] = 1.f;
  proj_matrix_[3][2] = -(far * near) / (far - near);
}

void Camera::SetViewDirection(
  const glm::vec3& position, 
  const glm::vec3& direction, 
  const glm::vec3& up) 
{
  const glm::vec3 w{glm::normalize(direction)};
  const glm::vec3 u{glm::normalize(glm::cross(w, up))};
  const glm::vec3 v{glm::cross(w, u)};

  view_matrix_ = glm::mat4{1.f};
  view_matrix_[0][0] = u.x;
  view_matrix_[1][0] = u.y;
  view_matrix_[2][0] = u.z;
  view_matrix_[0][1] = v.x;
  view_matrix_[1][1] = v.y;
  view_matrix_[2][1] = v.z;
  view_matrix_[0][2] = w.x;
  view_matrix_[1][2] = w.y;
  view_matrix_[2][2] = w.z;
  view_matrix_[3][0] = -glm::dot(u, position);
  view_matrix_[3][1] = -glm::dot(v, position);
  view_matrix_[3][2] = -glm::dot(w, position);
}

void Camera::SetViewTarget(
  const glm::vec3& position, 
  const glm::vec3& target, 
  const glm::vec3& up) 
{
  SetViewDirection(position, target - position, up);
}

void Camera::SetViewYXZ(
  const glm::vec3& position, 
  const glm::vec3& rotation) 
{
  const float c3 = glm::cos(rotation.z);
  const float s3 = glm::sin(rotation.z);
  const float c2 = glm::cos(rotation.x);
  const float s2 = glm::sin(rotation.x);
  const float c1 = glm::cos(rotation.y);
  const float s1 = glm::sin(rotation.y);
  const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
  const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
  const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
  view_matrix_ = glm::mat4{1.f};
  view_matrix_[0][0] = u.x;
  view_matrix_[1][0] = u.y;
  view_matrix_[2][0] = u.z;
  view_matrix_[0][1] = v.x;
  view_matrix_[1][1] = v.y;
  view_matrix_[2][1] = v.z;
  view_matrix_[0][2] = w.x;
  view_matrix_[1][2] = w.y;
  view_matrix_[2][2] = w.z;
  view_matrix_[3][0] = -glm::dot(u, position);
  view_matrix_[3][1] = -glm::dot(v, position);
  view_matrix_[3][2] = -glm::dot(w, position);
}


} // namespace heh