#pragma once

#include "core/camera.hpp"
#include "core/keys_n_mouse.hpp"

// libs
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// std
#include <string>

namespace heh {

class Window {
 public:
  Window(int width, int height, const std::string &window_name);
  ~Window();

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  bool WasResized() const { return framebuffer_resized_; }
  void ResetResizedFlag() { framebuffer_resized_ = false; }
  void Run();

  GLFWwindow* GetGLFWwindow() const { return window_; }

 private:
  static void FramebufferResizeCallback(GLFWwindow* glfw_window, int width, int height);
  static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);  
  static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
  static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
  void HandleInput();

  void InitWindow();
  void Cleanup();
  

  int width_;
  int height_;

  std::string window_name_;
  GLFWwindow* window_;

  Camera camera_;
  Keyboard keyboard_;
  Mouse mouse_;

  bool framebuffer_resized_ = false;
  bool wireframe_mode_ = false;
  bool show_cursor_ = false;
};

}  // namespace heh
