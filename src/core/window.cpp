#include "core/window.hpp"
#include "core/buffer.hpp"
#include "core/shader.hpp"
#include "core/block.hpp"


// libs
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// std
#include <stdexcept>
#include <iostream>

namespace heh {

void PrintOpenGLInfo() {
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
  glEnable(GL_DEPTH_TEST);
  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, FramebufferResizeCallback);
  glfwSetKeyCallback(window_, KeyCallback);
  PrintOpenGLInfo();
}

void Window::Run() {
  glm::vec3 cubePositions[] = {
glm::vec3( 0.0f, 0.0f, -3.0f),
glm::vec3( 2.0f, 5.0f, -15.0f),
glm::vec3(-1.5f, -2.2f, -2.5f),
glm::vec3(-3.8f, -2.0f, -12.3f),
glm::vec3( 2.4f, -0.4f, -3.5f),
glm::vec3(-1.7f, 3.0f, -7.5f),
glm::vec3( 1.3f, -2.0f, -2.5f),
glm::vec3( 1.5f, 2.0f, -2.5f),
glm::vec3( 1.5f, 0.2f, -1.5f),
glm::vec3(-1.3f, 1.0f, -1.5f)
};

  float vertices[] = {
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
  };

  Shader shader("shaders/simple.vert", "shaders/simple.frag");
  
  Buffer element_buffer(GL_ELEMENT_ARRAY_BUFFER);
  Buffer vertex_buffer(GL_ARRAY_BUFFER);
  VertexArray vertex_array;
  vertex_array.Bind();  // VAO

  vertex_buffer.Bind(); // VBO
  vertex_buffer.SetData(sizeof(vertices), vertices, GL_STATIC_DRAW);

  //element_buffer.Bind(); // EBO
  //element_buffer.SetData(sizeof(indices), indices, GL_STATIC_DRAW);

  // Position attribute
  vertex_array.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  vertex_array.EnableVertexAttribArray(0);

  // Texture attribute
  vertex_array.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  vertex_array.EnableVertexAttribArray(1);

  GrassTexture grass(GrassTexture::Face::kFront);
  GrassTexture dirt(GrassTexture::Face::kBottom);

  shader.Use();
  shader.SetInt("texture1", 0);
  shader.SetInt("texture2", 1);

  double last_time_ = glfwGetTime();
  int nb_frames_ = 0;

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
  while (!glfwWindowShouldClose(window_)) {
    UpdateWindowTitleWithFPS(window_, window_name_, last_time_, nb_frames_);
    
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    grass.Bind();
    glActiveTexture(GL_TEXTURE1);
    dirt.Bind();
    
    shader.Use();
    for(unsigned int i = 0; i < 10; i++)
    {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * i;
      model = glm::rotate(model, glm::radians(angle),
      glm::vec3(1.0f, 0.3f, 0.5f));
      shader.SetMat4("model", model);
      shader.SetMat4("view", glm::mat4(1.0f));
      shader.SetMat4("projection", glm::perspective(glm::radians(90.0f), (float)width_ / (float)height_, 0.1f, 100.0f));
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    vertex_array.Bind();

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
