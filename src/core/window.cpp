#include "core/window.hpp"
#include "core/buffer.hpp"
#include "core/shader.hpp"
#include "core/block.hpp"

// Standard libraries
#include <stdexcept>
#include <iostream>

namespace heh {

void UpdateWindowTitleWithFPS(GLFWwindow* window, const std::string& baseTitle, double& lastTime, int& nbFrames) {
  double currentTime = glfwGetTime();
  nbFrames++;
  if (currentTime - lastTime >= 1.0) { // If last time is more than 1 sec ago
      std::stringstream ss;
      ss << baseTitle << " [" << nbFrames << " FPS]";
      glfwSetWindowTitle(window, ss.str().c_str());
      nbFrames = 0;
      lastTime += 1.0;
  }
}

void InitializeGlfw() {
  static bool glfw_initialized = false;

  if (glfw_initialized)
    return;
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }
  glfw_initialized = true;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
}

Window::Window(int width, int height, const std::string &window_name)
    : width_{width}, height_{height}, window_name_{window_name}, window_{nullptr} {
  InitWindow();
}

Window::~Window() {
  Cleanup();
}

void Window::InitWindow() {
  InitializeGlfw();

  window_ = glfwCreateWindow(width_, height_, window_name_.c_str(), nullptr, nullptr);
  if (!window_) {
    throw std::runtime_error("Failed to create GLFW window");
  }

  glfwMakeContextCurrent(window_);
  glfwSwapInterval(0); // Disable VSync
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    glfwTerminate();
    throw std::runtime_error("Failed to initialize GLAD");
  }

  glViewport(0, 0, width_, height_);
  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, FramebufferResizeCallback);
  glfwSetKeyCallback(window_, KeyCallback);
}

void Window::Run() {
  float vertices[] = {
  // positions // colors // texture coords
  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
  -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
  -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
  };
  unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3 // second triangle
  };

  Shader shader("shaders/simple.vert", "shaders/simple.frag");
  
  Buffer element_buffer(GL_ELEMENT_ARRAY_BUFFER);
  Buffer vertex_buffer(GL_ARRAY_BUFFER);
  VertexArray vertex_array;
  vertex_array.Bind();  // VAO

  vertex_buffer.Bind(); // VBO
  vertex_buffer.SetData(sizeof(vertices), vertices, GL_STATIC_DRAW);

  element_buffer.Bind(); // EBO
  element_buffer.SetData(sizeof(indices), indices, GL_STATIC_DRAW);

  // Position attribute
  vertex_array.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  vertex_array.EnableVertexAttribArray(0);

  // Color attribute
  vertex_array.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  vertex_array.EnableVertexAttribArray(1);

  // Texture attribute
  vertex_array.VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  vertex_array.EnableVertexAttribArray(2);

  GrassTexture grass(GrassTexture::Face::kFront);

  double last_time_ = glfwGetTime();
  int nb_frames_ = 0;

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
  while (!glfwWindowShouldClose(window_)) {
    UpdateWindowTitleWithFPS(window_, window_name_, last_time_, nb_frames_);
    
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    grass.Bind();
    shader.Use();
    vertex_array.Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

void Window::Cleanup() {
  if (window_) {
    glfwDestroyWindow(window_);
    window_ = nullptr;
  }
  glfwTerminate();
}

void Window::FramebufferResizeCallback(GLFWwindow* glfw_window, int width, int height) {
  glViewport(0, 0, width, height);
  auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
  window->framebuffer_resized_ = true;
  window->width_ = width;
  window->height_ = height;
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

}  // namespace heh
