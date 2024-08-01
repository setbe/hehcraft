#pragma once

#include "core/camera.hpp"
#include "core/keys_n_mouse.hpp"

// libs
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// std
#include <string>

namespace heh {

/**
 * @brief The Window class represents a window in the application.
 * This class provides functionality for creating and managing a window using GLFW.
 * It also handles input events such as keyboard and mouse events.
 */
class Window {
 public:
  /**
   * @brief Constructs a Window object with the specified width, height, and window name.
   * @param width The width of the window.
   * @param height The height of the window.
   * @param window_name The name of the window.
   */
  Window(int width, int height, const std::string &window_name);

  /**
   * @brief Destructor for the Window object.
   */
  ~Window();

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  /**
   * @brief Checks if the window was resized.
   * @return true if the window was resized, false otherwise.
   */
  bool WasResized() const { return framebuffer_resized_; }

  /**
   * @brief Resets the resized flag.
   * This function should be called after handling the window resize event.
   * It resets the resized flag to false.
   */
  void ResetResizedFlag() { framebuffer_resized_ = false; }

  /**
   * @brief Runs the main loop of the window.
   * It handles input events and updates the window accordingly.
   */
  void Run();

  /**
   * @brief Gets the GLFW window handle.
   * @return The GLFWwindow pointer representing the window.
   */
  GLFWwindow* GetGLFWwindow() const { return window_; }

 private:
  /**
   * @brief Handles keyboard input events.
   */
  void HandleKeys();

  void InitWindow();
  void Cleanup();

  void CalculateDeltaTime();
  void CalculateFPS();
  

  int width_;  /**< The width of the window.  */
  int height_; /**< The height of the window. */

  std::string window_name_; /**< The name of the window. */
  GLFWwindow* window_;      /**< The GLFW window handle. */

  Camera::Data camera_data_; /**< Data for the camera object.                           */ 
  Camera camera_;     /**< The camera object for handling view and projection matrices. */
  Keyboard keyboard_; /**< The keyboard object for handling keyboard input.             */
  Mouse mouse_;       /**< The mouse object for handling mouse input.                   */

  bool framebuffer_resized_ = false; /**< Flag indicating if the window was resized.    */
  bool wireframe_mode_ = false;      /**< Flag indicating if wireframe mode is enabled. */

  double last_time_ = 0.0;
  double current_time_ = 0.0;
  int nb_frames_ = 0;
  double last_fps_update_time_ = 0.0;

private:
  /**
   * @brief Callback function for handling framebuffer resize events.
   * @param glfw_window The GLFW window handle.
   * @param width The new width of the framebuffer.
   * @param height The new height of the framebuffer.
   */
  static void FramebufferResizeCallback(GLFWwindow* glfw_window, int width, int height);

  /**
   * @brief Callback function for handling keyboard events.
   * @param window The GLFW window handle.
   * @param key The keyboard key that was pressed or released.
   * @param scancode The system-specific scancode of the key.
   * @param action The action that was performed (GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT).
   * @param mods Bit field describing which modifier keys were held down.
   */
  static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

  /**
   * @brief Callback function for handling mouse button events.
   * @param window The GLFW window handle.
   * @param button The mouse button that was pressed or released.
   * @param action The action that was performed (GLFW_PRESS, GLFW_RELEASE).
   * @param mods Bit field describing which modifier keys were held down.
   */
  static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);  

  /**
   * @brief Callback function for handling cursor position events.
   * @param window The GLFW window handle.
   * @param xpos The new x-coordinate of the cursor.
   * @param ypos The new y-coordinate of the cursor.
   */
  static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

  /**
   * @brief Callback function for handling scroll events.
   * @param window The GLFW window handle.
   * @param xoffset The scroll offset along the x-axis.
   * @param yoffset The scroll offset along the y-axis.
   */
  static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

}  // namespace heh
