#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace heh {

class Window {
 public:
  Window(int width, int height, std::string window_name);
  ~Window();

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  bool ShouldClose() const { return glfwWindowShouldClose(window_); }
  VkExtent2D GetExtent() const { return {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_)}; }
  bool WasWindowResized() const { return framebuffer_resized_; }
  void ResetWindowResizedFlag() { framebuffer_resized_ = false; }

  void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
  GLFWwindow* GetGLFWwindow() const { return window_; }

 private:
  static void FramebufferResizeCallback(GLFWwindow* glfw_window, int width, int height);
  void InitWindow();

  int width_;
  int height_;
  bool framebuffer_resized_ = false;

  std::string window_name_;
  GLFWwindow* window_;
};

}  // namespace heh
