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

int main(int argc, char** argv) 
{  
  try {
    heh::Window window(800, 600, "Hello, World!");
    window.Run();
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}