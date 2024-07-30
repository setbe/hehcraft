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

# set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os -s")
# set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -Os -s")

# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Oz -s")
# set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Oz -s")

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
  src/core/glad.c
)

set(SOURCE_FILES
  src/main.cpp
)

# Додайте виконуваний файл проекту
add_executable(hehcraft
  ${SOURCE_FILES}
  ${CORE_SOURCES}
  ${CORE_SYSTEMS_SOURCES}
)

# Link libraries
target_link_libraries(hehcraft
  glfw
  ${OPENGL_LIBRARIES}
)

if (WIN32)
  target_link_libraries(hehcraft opengl32)
endif()

if (UNIX)
  target_link_libraries(hehcraft GL X11 Xxf86vm Xrandr Xi pthread dl)
endif()

# Get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
  "${CMAKE_SOURCE_DIR}/shaders/*.frag"
  "${CMAKE_SOURCE_DIR}/shaders/*.vert"
)

# Set the output directory for the compiled shader files
set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")

# Ensure the SHADER_OUTPUT_DIR exists
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

# Create a list to hold the compiled shader files
set(COMPILED_SHADER_FILES)

# Copy each shader file to the output directory
foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(COMPILED_SHADER "${SHADER_OUTPUT_DIR}/${FILE_NAME}")
  add_custom_command(
    OUTPUT ${COMPILED_SHADER}
    COMMAND ${CMAKE_COMMAND} -E copy ${GLSL} ${COMPILED_SHADER}
    DEPENDS ${GLSL}
    COMMENT "Copying ${GLSL} to ${COMPILED_SHADER}"
  )
  list(APPEND COMPILED_SHADER_FILES ${COMPILED_SHADER})
endforeach()

# Create a custom target to copy the shaders
add_custom_target(
  Shaders
  DEPENDS ${COMPILED_SHADER_FILES}
)

add_dependencies(hehcraft Shaders)