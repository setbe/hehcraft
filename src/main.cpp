#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG

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
  try {
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

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}