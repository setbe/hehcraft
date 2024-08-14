#include "core/window.hpp"

// libs
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// std
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <vector>

using namespace glm;

namespace heh {

static void PrintOpenGLInfo() {
  const GLubyte* renderer = glGetString(GL_RENDERER);
  const GLubyte* vendor = glGetString(GL_VENDOR);
  const GLubyte* version = glGetString(GL_VERSION);
  const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  GLint major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  std::cout << "Renderer: " << renderer << std::endl;
  std::cout << "Vendor: " << vendor << std::endl;
  std::cout << "OpenGL Version: " << version << std::endl;
  std::cout << "GLSL Version: " << glslVersion << std::endl;
  std::cout << "OpenGL Version (integer): " << major << "." << minor << std::endl;
}

static void InitializeOnce() {
  static bool initialized = false;

  if (initialized)
    return;
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }
  initialized = true;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  glfwWindowHint(GLFW_DEPTH_BITS, 24);
}

Window::Window(int width, int height, const std::string &window_name)
    : 
    width_{width},
    height_{height},
    window_name_{window_name},
    window_{nullptr},
    camera_{
        -90.0f,   // yaw
        0.0f,     // pitch
        0.9f,     // z_near
        1000.0f }  // z_far
{
  if (width < 100) width = 100;
  if (height < 100) height = 100;

  camera_.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
  camera_.SetSensitivity(config::file.camera.sensitivity);
  camera_.SetFov(config::file.camera.fov);
  camera_.SetPos({0.0f, 70.0f, 3.0f});
  InitWindow();
}

Window::~Window() {
  Cleanup();
}

void Window::InitWindow() {
  InitializeOnce();

  window_ = glfwCreateWindow(width_, height_, window_name_.c_str(), nullptr, nullptr);
  if (!window_)
    throw std::runtime_error("Failed to create GLFW window");

  glfwMakeContextCurrent(window_);

  if (config::file.window.fullscreen) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwSetWindowMonitor(GetGLFWwindow(), monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    width_ = mode->width;
    height_ = mode->height;
  }

  
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    glfwTerminate();
    throw std::runtime_error("Failed to initialize GLAD");
  }
  PrintOpenGLInfo();

  glfwSwapInterval(0); // Disable VSync
  glViewport(0, 0, width_, height_);
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, Window::FramebufferResizeCallback);
  glfwSetKeyCallback(window_, Window::KeyCallback);
  glfwSetMouseButtonCallback(window_, Window::MouseButtonCallback);
  glfwSetCursorPosCallback(window_, Window::CursorPositionCallback);
  glfwSetScrollCallback(window_, Window::ScrollCallback);
  FramebufferResizeCallback(window_, width_, height_);
}

void Window::Run() {
  World world{camera_.GetData()};
  world.Init();

  while (!glfwWindowShouldClose(window_)) {
    CalculateDeltaTime();
    CalculateFPS();

    camera_.LookAt();
    camera_.ProjectionMatrix();
    camera_.HandleKeys();

    if (dark_background_mode_)
      glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    else
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    world.Render();

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

void Window::HandleKeys() {  
  if (keyboard_.IsKeyPressed(Keyboard::Key::kEscape))
    glfwSetWindowShouldClose(window_, GLFW_TRUE);

  // [F1] Toggle wireframe mode
  if (keyboard_.IsKeyPressed(Keyboard::Key::kF1)) {
    wireframe_mode_ = !wireframe_mode_;
    if (wireframe_mode_)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  // [F2] Toggle dark background mode
  if (keyboard_.IsKeyPressed(Keyboard::Key::kF2))
    dark_background_mode_ = !dark_background_mode_;

  // [F11] Toggle fullscreen mode
  if (keyboard_.IsKeyPressed(Keyboard::Key::kF11)) 
  {
    config::file.window.fullscreen = !config::file.window.fullscreen;
    
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    if (config::file.window.fullscreen)
    {
      glfwSetWindowMonitor(GetGLFWwindow(), monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
      FramebufferResizeCallback(GetGLFWwindow(), mode->width, mode->height);
    }
    else
    {
      int screen_width = mode->width;
      int screen_height = mode->height;

      // Calculate the position of the window
      int xpos = (screen_width - config::file.window.width) / 2;   // Center the X position
      int ypos = (screen_height - config::file.window.height) / 2; // Center the Y position

      glfwSetWindowMonitor(
        GetGLFWwindow(),
        nullptr,
        xpos, ypos,
        config::file.window.width,
        config::file.window.height,
        GLFW_DONT_CARE);
    }
  }

  // [Left Alt] Toggle cursor visibility
  if (keyboard_.IsKeyPressed(Keyboard::Key::kLeftAlt)) {
    camera_.SetShowCursor(!camera_.GetShowCursor());
    if (camera_.GetShowCursor())
      glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
      glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
}

void Window::HandleMouse(double xpos, double ypos) {
  glm::vec3 ray_direction = camera_.GetRay(xpos, ypos, width_, height_);
  glm::vec3 ray_origin = camera_.GetPos();

  glm::vec3 hit_pos, hit_normal;


}

void Window::FramebufferResizeCallback(GLFWwindow* glfw_window, int width, int height) {
  glViewport(0, 0, width, height);
  auto window_ptr = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
  window_ptr->framebuffer_resized_ = true;

  if (config::file.window.fullscreen) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwSetWindowMonitor(window_ptr->GetGLFWwindow(), monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
  }
  if (height == 0)
    height = 1;

  window_ptr->width_ = width;
  window_ptr->height_ = height;
  window_ptr->camera_.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  auto window_ptr = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  window_ptr->keyboard_.HandleKey(key, scancode, action, mods);

  // Handle key input for the window
  window_ptr->HandleKeys();
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  auto window_ptr = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  window_ptr->mouse_.HandleMouseButton(button, action, mods);
}

void Window::CursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
  auto window_ptr = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  window_ptr->mouse_.HandleCursorPosition(xpos, ypos);

  // Handle mouse input for the camera and the window
  window_ptr->camera_.HandleMousePosition(xpos, ypos);
  window_ptr->HandleMouse(xpos, ypos);
}

void Window::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  auto window_ptr = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  window_ptr->mouse_.HandleScroll(xoffset, yoffset);

  // Not used yet
}

void Window::CalculateDeltaTime() {
  // Set delta time in camera data
  current_time_ = glfwGetTime();
  camera_.SetDeltaTime(static_cast<double>(current_time_ - last_time_));
  last_time_ = current_time_;
  nb_frames_++;
}

void Window::CalculateFPS() {
  // Update the window title every second
  if (current_time_ - last_fps_update_time_ >= 1.0) {
    double fps = nb_frames_ / (current_time_ - last_fps_update_time_);
    std::string new_title = config::file.window.window_name + " - FPS: " + std::to_string(static_cast<int>(fps));
    glfwSetWindowTitle(window_, new_title.c_str());
    last_fps_update_time_ = current_time_;
    nb_frames_ = 0;
  }
}

void Window::Cleanup() {
  if (window_) {
    glfwDestroyWindow(window_);
    window_ = nullptr;
  }
  glfwTerminate();
}

}  // namespace heh
