#include "core/window.hpp"

int main() {
  heh::Window window(800, 600, "Hello, World!");

  while (!window.ShouldClose()) {
    glfwPollEvents();
  }

  return 0;
}