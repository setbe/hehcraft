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
#include <vector>

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
    : 
    width_{width}, 
    height_{height}, 
    window_name_{window_name}, 
    window_{nullptr}, 

    camera_(glm::vec3(0.0f, 0.0f, 3.0f), 
        -90.0f,   // yaw
        0.0f,     // pitch
        70.0f,    // fov
        (float)width_ / (float)height_,  // aspect
        0.1f,       // z_near
        100.0f)    // z_far
{
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
  //glEnable(GL_CULL_FACE);
  //glCullFace(GL_BACK);
  //glFrontFace(GL_CCW);
  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, Window::FramebufferResizeCallback);
  glfwSetKeyCallback(window_, Window::KeyCallback);
  glfwSetMouseButtonCallback(window_, Window::MouseButtonCallback);
  glfwSetCursorPosCallback(window_, Window::CursorPositionCallback);
  glfwSetScrollCallback(window_, Window::ScrollCallback);
  PrintOpenGLInfo();
}

struct Vertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
};

Vertex vertices[] {
    // positions          // texture coords // normals
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{0.5f, -0.5f, -0.5f},  {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{0.5f, 0.5f, -0.5f},   {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
    {{-0.5f, 0.5f, -0.5f},  {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},

    {{-0.5f, -0.5f, 0.5f},  {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f, -0.5f, 0.5f},   {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f, 0.5f, 0.5f},    {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f},   {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},

    {{-0.5f, 0.5f, 0.5f},   {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f},  {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f},  {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},

    {{0.5f, 0.5f, 0.5f},    {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f},   {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f},   {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},

    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f},  {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f},   {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f},  {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},

    {{-0.5f, 0.5f, -0.5f},  {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f},   {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f},    {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f},   {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
};

unsigned int indices[] = {
    0, 1, 2, 2, 3, 0,    // front face
    4, 5, 6, 6, 7, 4,    // back face
    8, 9, 10, 10, 11, 8, // left face
    12, 13, 14, 14, 15, 12, // right face
    16, 17, 18, 18, 19, 16, // bottom face
    20, 21, 22, 22, 23, 20  // top face
};

void Window::Run() {
  double last_time_ = glfwGetTime();
  double current_time = 0.0;
  double delta_time = 0.0f;
  int nb_frames_ = 0;
  double last_fps_update_time = 0.0;

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

  // Texture attribute
  vertex_array.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  vertex_array.EnableVertexAttribArray(1);

  // Normal attribute
  vertex_array.VertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
  vertex_array.EnableVertexAttribArray(2);

  GrassTexture side(Texture::Face::kFront);

  glm::vec3 light_pos(1.2f, 1.0f, 2.0f);

  shader.Use();
  glActiveTexture(GL_TEXTURE0);
  side.Bind();
  shader.SetInt("texture_diffuse1", 0);

  while (!glfwWindowShouldClose(window_)) {
    current_time = glfwGetTime();
    delta_time = static_cast<double>(current_time - last_time_);
    last_time_ = current_time;
    nb_frames_++;

    // Update the window title every second
    if (current_time - last_fps_update_time >= 1.0) {
      double fps = nb_frames_ / (current_time - last_fps_update_time);
      std::string new_title = "Hehcraft - FPS: " + std::to_string(static_cast<int>(fps));
      glfwSetWindowTitle(window_, new_title.c_str());
      last_fps_update_time = current_time;
      nb_frames_ = 0;
    }
    
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera_.Update(delta_time, show_cursor_);

    shader.Use();
    glm::mat4 view = camera_.LookAt();
    glm::mat4 projection = camera_.GetProjectionMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    shader.SetMat4("model", model);
    shader.SetMat4("view", view);
    shader.SetMat4("projection", projection);
    shader.SetVec3("lightPos", light_pos);
    shader.SetVec3("viewPos", camera_.GetPos());
    shader.SetVec3("lightColor", glm::vec3(0.0f, 0.0f, 1.0f));

    glActiveTexture(GL_TEXTURE0);
    side.Bind();

    vertex_array.Bind();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

void Window::HandleInput() {
  if (keyboard_.IsKeyPressed(Keyboard::Key::kEscape))
    glfwSetWindowShouldClose(window_, GLFW_TRUE);

  if (keyboard_.IsKeyPressed(Keyboard::Key::kF12)) {
    wireframe_mode_ = !wireframe_mode_;
    if (wireframe_mode_)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  if (keyboard_.IsKeyPressed(Keyboard::Key::kLeftAlt)) {
    show_cursor_ = !show_cursor_;
    if (show_cursor_)
      glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
      glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
}

void Window::FramebufferResizeCallback(GLFWwindow* glfw_window, int width, int height) {
  glViewport(0, 0, width, height);
  auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
  window->framebuffer_resized_ = true;
  window->width_ = width;
  window->height_ = height;
  window->camera_.SetAspect((float)width / (float)height);
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  auto window_ptr = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  window_ptr->keyboard_.HandleKey(key, scancode, action, mods);
  window_ptr->HandleInput();
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  auto window_ptr = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  window_ptr->mouse_.HandleMouseButton(button, action, mods);
}

void Window::CursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
  auto window_ptr = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  window_ptr->mouse_.HandleCursorPosition(xpos, ypos);
}

void Window::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  auto window_ptr = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  window_ptr->mouse_.HandleScroll(xoffset, yoffset);
}

void Window::Cleanup() {
  if (window_) {
    glfwDestroyWindow(window_);
    window_ = nullptr;
  }
  glfwTerminate();
}

}  // namespace heh
