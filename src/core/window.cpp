#include "core/window.hpp"
#include "core/buffer.hpp"
#include "core/shader.hpp"
#include "core/block.hpp"
#include "core/texture.hpp"

// libs
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// std
#include <stdexcept>
#include <iostream>

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
}

Window::Window(int width, int height, const std::string &window_name)
    : 
    width_{width}, 
    height_{height}, 
    window_name_{window_name}, 
    window_{nullptr}, 
    camera_(
        camera_data_, 
        glm::vec3(0.0f, 0.0f, 3.0f), 
        -90.0f,   // yaw
        0.0f,     // pitch
        0.1f,       // z_near
        100.0f)    // z_far
{
  camera_.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
  camera_data_.sensitivity = 0.1f;
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
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    glfwTerminate();
    throw std::runtime_error("Failed to initialize GLAD");
  }
  PrintOpenGLInfo();

  glfwSwapInterval(0); // Disable VSync
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

  TextureManager::LoadAllTextures();
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
  Shader shader("shaders/simple.vert", "shaders/simple.frag");
  
  Buffer element_buffer(GL_ELEMENT_ARRAY_BUFFER);
  Buffer vertex_buffer(GL_ARRAY_BUFFER);
  VertexArray vertex_array;
  
  vertex_buffer.SetData(sizeof(vertices), vertices, GL_STATIC_DRAW);
  element_buffer.SetData(sizeof(indices), indices, GL_STATIC_DRAW);

  vertex_array.EnableVertexAttrib(0);
  vertex_array.VertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
  vertex_array.VertexAttribBinding(0, 0);
  vertex_array.VertexBuffer(0, vertex_buffer.GetID(), 0, 8 * sizeof(float));

  vertex_array.EnableVertexAttrib(1);
  vertex_array.VertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
  vertex_array.VertexAttribBinding(1, 0);
  vertex_array.VertexBuffer(0, vertex_buffer.GetID(), 0, 8 * sizeof(float));

  vertex_array.EnableVertexAttrib(2);
  vertex_array.VertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float));
  vertex_array.VertexAttribBinding(2, 0);
  vertex_array.VertexBuffer(0, vertex_buffer.GetID(), 0, 8 * sizeof(float));

  vertex_array.ElementBuffer(element_buffer.GetID());

  glm::vec3 light_pos(1.2f, 1.0f, 2.0f);

  Block cobblestone("cobblestone");
  Block grass("grass");
  grass.Translate(glm::vec3(2.0f, 0.0f, 0.0f));

  shader.Use();
  shader.SetInt("texture_diffuse1", 0);

  while (!glfwWindowShouldClose(window_)) {
    CalculateDeltaTime();
    CalculateFPS();

    camera_.HandleKeys();
    camera_.LookAt();
    camera_.ProjectionMatrix();
    
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Use();
    shader.SetMat4("view", camera_data_.view);
    shader.SetMat4("projection", camera_data_.projection);
    shader.SetVec3("lightPos", light_pos);
    shader.SetVec3("viewPos", camera_.GetPos());
    shader.SetVec3("lightColor", glm::vec3(1.f, 1.f, 1.f));
    shader.SetVec3("dirLightDirection", glm::vec3(1.2f, 2.0f, 1.3f));
    shader.SetVec3("dirLightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    cobblestone.Draw(shader);

    vertex_array.Bind();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    
    grass.Draw(shader);

    vertex_array.Bind();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

void Window::HandleKeys() {  
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
    camera_data_.show_cursor = !camera_data_.show_cursor;
    if (camera_data_.show_cursor)
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
  current_time_ = glfwGetTime();
  camera_data_.delta_time = static_cast<double>(current_time_ - last_time_);
  last_time_ = current_time_;
  nb_frames_++;
}

void Window::CalculateFPS() {
  // Update the window title every second
  if (current_time_ - last_fps_update_time_ >= 1.0) {
    double fps = nb_frames_ / (current_time_ - last_fps_update_time_);
    std::string new_title = "Hehcraft - FPS: " + std::to_string(static_cast<int>(fps));
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
