#pragma once

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
  
  void InitWindow();
  void Cleanup();

  int width_;
  int height_;
  bool framebuffer_resized_ = false;

  std::string window_name_;
  GLFWwindow* window_;
};

}  // namespace heh
