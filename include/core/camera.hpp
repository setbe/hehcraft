#pragma once

#include "core/keys_n_mouse.hpp"

// libs
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace heh {

class Camera {
 public:
  Camera(const glm::vec3 &pos, float yaw, float pitch, float fov, float aspect, float z_near, float z_far)
      : pos_(pos), yaw_(yaw), pitch_(pitch), fov_(fov), aspect_(aspect), z_near_(z_near), z_far_(z_far) {
    UpdateCameraVectors();
  }

  ~Camera() {}

  Camera(const Camera&) = delete;
  Camera& operator=(const Camera&) = delete;

  void Update(float delta_time, bool cursor_enabled) {
    if (cursor_enabled)
      return;

    float camera_speed = 2.5f * delta_time;
    if (Keyboard::IsKeyHeld(Keyboard::Key::kLeftControl))
      camera_speed *= 3.0f;

    if (Keyboard::IsKeyHeld(Keyboard::Key::kW))
      pos_ += camera_speed * front_;
    if (Keyboard::IsKeyHeld(Keyboard::Key::kS))
      pos_ -= camera_speed * front_;
    if (Keyboard::IsKeyHeld(Keyboard::Key::kA))
      pos_ -= glm::normalize(glm::cross(front_, up_)) * camera_speed;
    if (Keyboard::IsKeyHeld(Keyboard::Key::kD))
      pos_ += glm::normalize(glm::cross(front_, up_)) * camera_speed;
    if (Keyboard::IsKeyHeld(Keyboard::Key::kSpace))
      pos_ += camera_speed * up_;
    if (Keyboard::IsKeyHeld(Keyboard::Key::kLeftShift))
      pos_ -= camera_speed * up_;

    const float sensitivity = 0.1f;
    yaw_ += Mouse::GetDX() * sensitivity;
    pitch_ -= Mouse::GetDY() * sensitivity;

    if (pitch_ > 89.0f) pitch_ = 89.0f;
    if (pitch_ < -89.0f) pitch_ = -89.0f;

    UpdateCameraVectors();
    Mouse::ResetDeltas();
  }

  glm::mat4 LookAt() const {
    return glm::lookAt(pos_, pos_ + front_, up_);
  }

  glm::mat4 GetProjectionMatrix() const {
    return glm::perspective(glm::radians(fov_), aspect_, z_near_, z_far_);
  }

  const glm::vec3& GetPos() const { return pos_; }
  void SetPos(const glm::vec3 &pos) { pos_ = pos; }

  float GetFov() const { return fov_; }
  void SetFov(float fov) { fov_ = fov; }

  float GetAspect() const { return aspect_; }
  void SetAspect(float aspect) { aspect_ = aspect; }

  float GetZNear() const { return z_near_; }
  void SetZNear(float z_near) { z_near_ = z_near; }

  float GetZFar() const { return z_far_; }
  void SetZFar(float z_far) { z_far_ = z_far; }

 private:
  void UpdateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front);
    right_ = glm::normalize(glm::cross(front_, glm::vec3(0.0f, 1.0f, 0.0f)));
    up_ = glm::normalize(glm::cross(right_, front_));
  }

  

  glm::vec3 pos_;
  glm::vec3 front_;
  glm::vec3 up_;
  glm::vec3 right_;
  float yaw_;
  float pitch_;
  float fov_;
  float aspect_;
  float z_near_;
  float z_far_;
};


}  // namespace heh