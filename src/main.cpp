#include "core/window.hpp"

#include <iostream>

int main() {

  
  try {
    heh::Window window(800, 600, "Hello, World!");
    window.Run();
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}