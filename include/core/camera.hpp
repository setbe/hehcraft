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
 struct Data {
  bool show_cursor{true};     ///< Whether the cursor is visible or not.
  float delta_time{0.0f};     ///< The time elapsed since the last update.
  float sensitivity{1.0f};    ///< The sensitivity of the camera.
  float aspect_ratio{1.0f};   ///< The aspect ratio of the camera.
  float fov{80.0f};           ///< The field of view of the camera.
  glm::mat4 view{1.0f};       ///< The view matrix of the camera.
  glm::mat4 projection{1.0f}; ///< The projection matrix of the camera.
 };
  /**
   * @brief Constructs a Camera object.
   * @param pos The position of the camera.
   * @param yaw The yaw angle of the camera.
   * @param pitch The pitch angle of the camera.
   * @param z_near The near clipping plane of the camera.
   * @param z_far The far clipping plane of the camera.
   */
  Camera(Camera::Data &data, const glm::vec3 &pos, float yaw, float pitch, float z_near, float z_far)
      : data_(data), pos_(pos), yaw_(yaw), pitch_(pitch), z_near_(z_near), z_far_(z_far) {
    UpdateCameraVectors();
  }

  ~Camera() {}

  Camera(const Camera&) = delete;
  Camera& operator=(const Camera&) = delete;

  /**
   * @brief Updates the camera's position and orientation based on user input.
   */
  void HandleKeys() {
    float camera_speed = 5.0f * data_.delta_time;
    
    if (Keyboard::IsKeyHeld(Keyboard::Key::kLeftControl))
      camera_speed *= 4.0f;

    glm::vec3 horizontal_movement = glm::normalize(glm::vec3(front_.x, 0.0f, front_.z));

    if (Keyboard::IsKeyHeld(Keyboard::Key::kW)) {
      pos_ += camera_speed * horizontal_movement;
      view_needs_update_ = true;
    }
    if (Keyboard::IsKeyHeld(Keyboard::Key::kS)) {
      pos_ -= camera_speed * horizontal_movement;
      view_needs_update_ = true;
    }
    if (Keyboard::IsKeyHeld(Keyboard::Key::kA)) {
      pos_ -= glm::normalize(glm::cross(front_, up_)) * camera_speed;
      view_needs_update_ = true;
    }
    if (Keyboard::IsKeyHeld(Keyboard::Key::kD)) {
      pos_ += glm::normalize(glm::cross(front_, up_)) * camera_speed;
      view_needs_update_ = true;
    }
    if (Keyboard::IsKeyHeld(Keyboard::Key::kSpace)) {
      pos_ += camera_speed * glm::vec3(0.0f, 1.0f, 0.0f);
      view_needs_update_ = true;
    }
    if (Keyboard::IsKeyHeld(Keyboard::Key::kLeftShift)) {
      pos_ -= camera_speed * glm::vec3(0.0f, 1.0f, 0.0f);
      view_needs_update_ = true;
    }
  }

  void HandleMousePosition(double xpos, double ypos) {
    if (data_.show_cursor)
      return; // Don't handle mouse input if the cursor is visible.

    yaw_ += Mouse::GetDX() * data_.sensitivity;
    pitch_ -= Mouse::GetDY() * data_.sensitivity;

    if (pitch_ > 89.0f) pitch_ = 89.0f;
    if (pitch_ < -89.0f) pitch_ = -89.0f;

    UpdateCameraVectors();
    Mouse::ResetDeltas();

    view_needs_update_ = true;
  }

  glm::vec3 GetRay(double xpos, double ypos, int screen_width, int screen_height) {
    float x = (2.0f * xpos) / screen_width - 1.0f;
    float y = 1.0f - (2.0f * ypos) / screen_height;
    float z = 1.0f;

    glm::vec3 ray_nds = glm::vec3(x, y, z);
    glm::vec4 ray_clip = glm::vec4(ray_nds, 1.0f);

    glm::vec4 ray_eye = glm::inverse(data_.projection) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

    glm::vec3 ray_wor = glm::vec3(glm::inverse(data_.view) * ray_eye);
    ray_wor = glm::normalize(ray_wor);

    return ray_wor;
  }

  bool RayIntersectsBlock(const glm::vec3 &ray, const glm::vec3 &camera_pos, const glm::vec3 &block_pos) {
    glm::vec3 block_to_camera = camera_pos - block_pos;
    float distance = glm::dot(block_to_camera, ray);
    glm::vec3 point = block_pos + distance * ray;

    return glm::distance(point, camera_pos) < 5.0f;
  }


  /**
   * @brief Calculates the view matrix for the camera if it needs updating.
   */
  void LookAt() {
    if (!view_needs_update_)
      return;

    data_.view = glm::lookAt(pos_, pos_ + front_, up_);
    view_needs_update_ = false;
  }

  /**
   * @brief Calculates the projection matrix for the camera if it needs updating.
   */
  void ProjectionMatrix() {
    if (!projection_needs_update_)
      return;

    data_.projection = glm::perspective(glm::radians(data_.fov), data_.aspect_ratio, z_near_, z_far_);
    projection_needs_update_ = false;
  }

  float GetAspectRatio() const { return data_.aspect_ratio; }

  /**
   * @brief Sets the aspect ratio of the camera.
   * @param aspect_ratio The new aspect ratio of the camera.
   */
  void SetAspectRatio(float aspect_ratio) {
    data_.aspect_ratio = aspect_ratio;
    projection_needs_update_ = true;
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
  void SetPos(const glm::vec3 &pos) { 
    pos_ = pos; 
    view_needs_update_ = true;
  }

  /**
   * @brief Gets the field of view of the camera.
   * @return The field of view of the camera.
   */
  float GetFov() const { return data_.fov; }

  /**
   * @brief Sets the field of view of the camera.
   * @param fov The new field of view of the camera.
   */
  void SetFov(float fov) { 
    data_.fov = fov; 
    projection_needs_update_ = true;
  }

  /**
   * @brief Gets the near clipping plane of the camera.
   * @return The near clipping plane of the camera.
   */
  float GetZNear() const { return z_near_; }

  /**
   * @brief Sets the near clipping plane of the camera.
   * @param z_near The new near clipping plane of the camera.
   */
  void SetZNear(float z_near) { 
    z_near_ = z_near; 
    projection_needs_update_ = true;
  }

  /**
   * @brief Gets the far clipping plane of the camera.
   * @return The far clipping plane of the camera.
   */
  float GetZFar() const { return z_far_; }

  /**
   * @brief Sets the far clipping plane of the camera.
   * @param z_far The new far clipping plane of the camera.
   */
  void SetZFar(float z_far) { 
    z_far_ = z_far; 
    projection_needs_update_ = true;
  }

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

  Camera::Data &data_;  ///< Data for the camera object.
  glm::vec3 pos_; ///< The position of the camera.
  glm::vec3 front_; ///< The front vector of the camera.
  glm::vec3 up_; ///< The up vector of the camera.
  glm::vec3 right_; ///< The right vector of the camera.
  float yaw_; ///< The yaw angle of the camera.
  float pitch_; ///< The pitch angle of the camera.
  float z_near_; ///< The near clipping plane of the camera.
  float z_far_; ///< The far clipping plane of the camera.
  bool view_needs_update_ = true; ///< Flag to indicate if the view matrix needs updating.
  bool projection_needs_update_ = true; ///< Flag to indicate if the projection matrix needs updating.
};


}  // namespace heh