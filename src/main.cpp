#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG

#include "core/window.hpp"

#include <iostream>


int main(int argc, char** argv);

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


#include "utils/image_writer.hpp" 

int main(int argc, char** argv) 
{  
  try {
    int width = heh::config::file.window.width;
    int height = heh::config::file.window.height;
    std::string window_name = heh::config::file.window.window_name;

    heh::Window window(width, height, window_name);
    window.Run();
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}