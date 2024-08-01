#pragma once

#include "core/keys_n_mouse.hpp"

// libs
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace heh {

/**
 * @class Camera
 * @brief Represents a camera in a 3D scene.
 */
class Camera {
 public:
  /**
   * @brief Constructs a Camera object.
   * @param pos The position of the camera.
   * @param yaw The yaw angle of the camera.
   * @param pitch The pitch angle of the camera.
   * @param fov The field of view of the camera.
   * @param aspect The aspect ratio of the camera.
   * @param z_near The near clipping plane of the camera.
   * @param z_far The far clipping plane of the camera.
   */
  Camera(const glm::vec3 &pos, float yaw, float pitch, float fov, float aspect, float z_near, float z_far)
      : pos_(pos), yaw_(yaw), pitch_(pitch), fov_(fov), aspect_(aspect), z_near_(z_near), z_far_(z_far) {
    UpdateCameraVectors();
  }

  /**
   * @brief Destructs the Camera object.
   */
  ~Camera() {}

  /**
   * @brief Deleted copy constructor.
   */
  Camera(const Camera&) = delete;

  /**
   * @brief Deleted copy assignment operator.
   */
  Camera& operator=(const Camera&) = delete;

  /**
   * @brief Updates the camera's position and orientation based on user input.
   * @param delta_time The time elapsed since the last update.
   * @param cursor_enabled Whether the cursor is enabled or not.
   */
  void Update(float delta_time, bool cursor_enabled) {
    if (cursor_enabled)
      return;

    float camera_speed = 2.5f * delta_time;
    if (Keyboard::IsKeyHeld(Keyboard::Key::kLeftControl))
      camera_speed *= 3.0f;

    glm::vec3 horizontal_movement = glm::normalize(glm::vec3(front_.x, 0.0f, front_.z));

    if (Keyboard::IsKeyHeld(Keyboard::Key::kW))
      pos_ += camera_speed * horizontal_movement;
    if (Keyboard::IsKeyHeld(Keyboard::Key::kS))
      pos_ -= camera_speed * horizontal_movement;
    if (Keyboard::IsKeyHeld(Keyboard::Key::kA))
      pos_ -= glm::normalize(glm::cross(front_, up_)) * camera_speed;
    if (Keyboard::IsKeyHeld(Keyboard::Key::kD))
      pos_ += glm::normalize(glm::cross(front_, up_)) * camera_speed;
    if (Keyboard::IsKeyHeld(Keyboard::Key::kSpace))
      pos_ += camera_speed * glm::vec3(0.0f, 1.0f, 0.0f);
    if (Keyboard::IsKeyHeld(Keyboard::Key::kLeftShift))
      pos_ -= camera_speed * glm::vec3(0.0f, 1.0f, 0.0f);

    const float sensitivity = 0.1f;
    yaw_ += Mouse::GetDX() * sensitivity;
    pitch_ -= Mouse::GetDY() * sensitivity;

    if (pitch_ > 89.0f) pitch_ = 89.0f;
    if (pitch_ < -89.0f) pitch_ = -89.0f;

    UpdateCameraVectors();
    Mouse::ResetDeltas();
  }

  /**
   * @brief Calculates the view matrix for the camera.
   * @return The view matrix.
   */
  glm::mat4 LookAt() const {
    return glm::lookAt(pos_, pos_ + front_, up_);
  }

  /**
   * @brief Calculates the projection matrix for the camera.
   * @return The projection matrix.
   */
  glm::mat4 GetProjectionMatrix() const {
    return glm::perspective(glm::radians(fov_), aspect_, z_near_, z_far_);
  }

  /**
   * @brief Gets the position of the camera.
   * @return The position of the camera.
   */
  const glm::vec3& GetPos() const { return pos_; }

  /**
   * @brief Sets the position of the camera.
   * @param pos The new position of the camera.
   */
  void SetPos(const glm::vec3 &pos) { pos_ = pos; }

  /**
   * @brief Gets the field of view of the camera.
   * @return The field of view of the camera.
   */
  float GetFov() const { return fov_; }

  /**
   * @brief Sets the field of view of the camera.
   * @param fov The new field of view of the camera.
   */
  void SetFov(float fov) { fov_ = fov; }

  /**
   * @brief Gets the aspect ratio of the camera.
   * @return The aspect ratio of the camera.
   */
  float GetAspect() const { return aspect_; }

  /**
   * @brief Sets the aspect ratio of the camera.
   * @param aspect The new aspect ratio of the camera.
   */
  void SetAspect(float aspect) { aspect_ = aspect; }

  /**
   * @brief Gets the near clipping plane of the camera.
   * @return The near clipping plane of the camera.
   */
  float GetZNear() const { return z_near_; }

  /**
   * @brief Sets the near clipping plane of the camera.
   * @param z_near The new near clipping plane of the camera.
   */
  void SetZNear(float z_near) { z_near_ = z_near; }

  /**
   * @brief Gets the far clipping plane of the camera.
   * @return The far clipping plane of the camera.
   */
  float GetZFar() const { return z_far_; }

  /**
   * @brief Sets the far clipping plane of the camera.
   * @param z_far The new far clipping plane of the camera.
   */
  void SetZFar(float z_far) { z_far_ = z_far; }

 private:
  /**
   * @brief Updates the camera's front, right, and up vectors based on its yaw and pitch angles.
   */
  void UpdateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front);
    right_ = glm::normalize(glm::cross(front_, glm::vec3(0.0f, 1.0f, 0.0f)));
    up_ = glm::normalize(glm::cross(right_, front_));
  }

  glm::vec3 pos_; ///< The position of the camera.
  glm::vec3 front_; ///< The front vector of the camera.
  glm::vec3 up_; ///< The up vector of the camera.
  glm::vec3 right_; ///< The right vector of the camera.
  float yaw_; ///< The yaw angle of the camera.
  float pitch_; ///< The pitch angle of the camera.
  float fov_; ///< The field of view of the camera.
  float aspect_; ///< The aspect ratio of the camera.
  float z_near_; ///< The near clipping plane of the camera.
  float z_far_; ///< The far clipping plane of the camera.
};


}  // namespace heh