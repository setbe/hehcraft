cmake_minimum_required(VERSION 3.10)
project(hehcraft)

# Set policy CMP0072 to use GLVND by default when available
if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()

set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)


if (UNIX)
  set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os -s")
  set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -Os -s")

  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -flto -funroll-loops")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -march=native -flto -funroll-loops")

  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -g3 -march=native -fno-omit-frame-pointer")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Og -g3 -march=native -fno-omit-frame-pointer")
endif()


# Paths to external libraries
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/external/glfw")
set(GLM_DIR "${CMAKE_SOURCE_DIR}/external/glm")

# Include directories
include_directories(${GLM_DIR})
include_directories(${CMAKE_SOURCE_DIR}/external)
include_directories(${CMAKE_SOURCE_DIR}/external/glad)
include_directories(${CMAKE_SOURCE_DIR}/external/tiny_obj_loader)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Find packages
find_package(OpenGL REQUIRED)

# Manually specify the paths to GLFW if not found by find_package
if (NOT TARGET glfw)
  # add_subdirectory(${GLFW_DIR})
  include_directories(${GLFW_DIR}/include)
  link_directories(${GLFW_DIR}/lib)
endif()

set(CORE_SOURCES
  src/core/window.cpp
  src/core/keys_n_mouse.cpp
  src/core/shader.cpp
)

set(WORLD_SOURCES
  src/world/world.cpp
  src/world/chunk.cpp
  src/world/block.cpp
)

set(UTILS_SOURCES
  src/utils/image_writer.cpp
  src/utils/toml_extended.cpp
)

set(SOURCE_FILES
  src/main.cpp
  external/glad/glad/glad.c
)

set(HEADER_FILES
  include/core/window.hpp
  include/core/buffer.hpp
  include/core/camera.hpp
  include/core/keys_n_mouse.hpp
  include/core/shader.hpp

  include/world/world.hpp
  include/world/chunk.hpp
  include/world/block.hpp

  include/utils/image_writer.hpp
  include/utils/toml_extended.hpp
)

add_executable(hehcraft
  ${SOURCE_FILES}
  ${CORE_SOURCES}
  ${WORLD_SOURCES}
  ${UTILS_SOURCES}

  ${HEADER_FILES}
)

# Source groups for Visual Studio filters
source_group("Source Files\\Core" FILES ${CORE_SOURCES})
source_group("Source Files\\World" FILES ${WORLD_SOURCES})
source_group("Source Files\\Utils" FILES ${UTILS_SOURCES})
source_group("Header Files" FILES ${HEADER_FILES})

if (WIN32)
  file(GLOB LIBS "${CMAKE_SOURCE_DIR}/libs/*.lib")
  target_link_libraries(hehcraft opengl32 ${LIBS})
endif()

if (UNIX)
  target_link_libraries(hehcraft ${OPENGL_LIBRARIES} glfw GL X11 Xxf86vm Xrandr Xi pthread dl)
endif()

# Get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
  "${CMAKE_SOURCE_DIR}/shaders/*.frag"
  "${CMAKE_SOURCE_DIR}/shaders/*.vert"
)

source_group("Assets\\Shaders" FILES ${GLSL_SOURCE_FILES})

# Get all texture files in textures directory
file(GLOB_RECURSE TEXTURE_FILES
  "${CMAKE_SOURCE_DIR}/textures/*"
)

# Set the output directory for the compiled shader and texture files
set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")
set(TEXTURE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/textures")

# Ensure the SHADER_OUTPUT_DIR and TEXTURE_OUTPUT_DIR exist
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})
file(MAKE_DIRECTORY ${TEXTURE_OUTPUT_DIR})

# Copy each shader file to the output directory
file(COPY ${GLSL_SOURCE_FILES} DESTINATION ${SHADER_OUTPUT_DIR})

# Copy the entire textures directory to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/textures DESTINATION ${CMAKE_BINARY_DIR})

# Create custom target to ensure shaders and textures are copied
add_custom_target(CopyAssets ALL DEPENDS ${SHADER_OUTPUT_DIR} ${TEXTURE_OUTPUT_DIR})

add_dependencies(hehcraft CopyAssets)