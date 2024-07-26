#include "core/window.hpp"

// Standard libraries
#include <stdexcept>

namespace heh {

void InitializeGlfw() {
  static bool glfw_initialized = false;

  if (!glfw_initialized) {
    if (!glfwInit())
      throw std::runtime_error("Failed to initialize GLFW");

    glfw_initialized = true;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  }
}

Window::Window(int width, int height, std::string window_name)
    : width_{width}, height_{height}, window_name_{std::move(window_name)} {
  InitWindow();
}

Window::~Window() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

void Window::InitWindow() {
  InitializeGlfw();
  
  window_ = glfwCreateWindow(width_, height_, window_name_.c_str(), nullptr, nullptr);
  if (!window_)
    throw std::runtime_error("Failed to create GLFW window");

  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, FramebufferResizeCallback);
}

void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
  if (glfwCreateWindowSurface(instance, window_, nullptr, surface) != VK_SUCCESS)
    throw std::runtime_error("Failed to create window surface");
}

void Window::FramebufferResizeCallback(GLFWwindow* glfw_window, int width, int height) {
  auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
  window->framebuffer_resized_ = true;
  window->width_ = width;
  window->height_ = height;
}

}  // namespace heh
