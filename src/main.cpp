#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG

#define HEHCRAFT_DEBUG
// #undef HEHCRAFT_DEBUG

#include "core/window.hpp"

#include <iostream>


int main(int argc, char** argv);

/* Entry point for the application on Windows 
   (subsystem:windows) with no console.
   Set linker option as SUBSYSTEM:WINDOWS to work */
#ifdef _WIN32
#include <Windows.h>
#include <cstdlib>
int APIENTRY WinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine, int nCmdShow)
{
  return main(__argc, __argv);
}
#endif


int main(int argc, char** argv)
{
#ifdef HEHCRAFT_DEBUG
  try {
#endif

    if (!glfwInit()) {
      throw std::runtime_error("Failed to initialize GLFW");
    }

    // Set the OpenGL version to 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Set the window to be resizable
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 32); // 32-bit depth buffer

    // Initialize the configuration files
    heh::config::InitConfigFiles("config.toml", "blocks.toml", "textures.toml");
    // Load the blocks configuration
    heh::block_map::LoadBlocks();

    // Set up the width, height, and window name from the configuration file
    int width = heh::config::file.window.width;
    int height = heh::config::file.window.height;
    std::string window_name = heh::config::file.window.window_name;

    // Create a window
    heh::Window window(width, height, window_name);

    // Run the main loop
    window.Run();

    glfwTerminate();

#ifdef HEHCRAFT_DEBUG
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
#endif

  return EXIT_SUCCESS;
}
