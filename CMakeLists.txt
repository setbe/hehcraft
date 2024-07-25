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

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Os -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -Os -s")

# Paths to external libraries
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/external/glfw")
set(GLM_DIR "${CMAKE_SOURCE_DIR}/external/glm")

# Include directories
include_directories(${GLM_DIR})
include_directories(${CMAKE_SOURCE_DIR}/external/tiny_obj_loader)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Find packages
# find_package(OpenGL REQUIRED)
find_package(Vulkan REQUIRED)

# Manually specify the paths to GLFW if not found by find_package
if (NOT TARGET glfw)
    # add_subdirectory(${GLFW_DIR})
    include_directories(${GLFW_DIR}/include)
    link_directories(${GLFW_DIR}/lib)
endif()

# Add executable
add_executable(hehcraft
    src/main.cpp
    src/window.cpp
    src/pipeline.cpp
    src/device.cpp
    src/swap_chain.cpp
    src/model.cpp
    src/renderer.cpp
    src/simple_render_system.cpp
    src/app.cpp
    src/camera.cpp
    src/keyboard_controller.cpp
    src/game_object.cpp
    src/buffer.cpp
    src/descriptors.cpp
)

# Link libraries
target_link_libraries(hehcraft
    glfw
    ${OPENGL_LIBRARIES}
    Vulkan::Vulkan
)

if (WIN32)
    target_link_libraries(hehcraft opengl32)
endif()

if (UNIX)
    target_link_libraries(hehcraft GL X11 Xxf86vm Xrandr Xi pthread dl)
endif()



# Find the glslangValidator executable
find_program(GLSL_VALIDATOR glslangValidator HINTS
  "$ENV{VULKAN_SDK}/Bin"
  "/usr/bin"
  "/usr/local/bin"
)

# Ensure the GLSL_VALIDATOR was found
if (NOT GLSL_VALIDATOR)
  message(FATAL_ERROR "glslangValidator not found. Please install or set VULKAN_SDK environment variable.")
endif()

# Get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
  "${CMAKE_SOURCE_DIR}/shaders/*.frag"
  "${CMAKE_SOURCE_DIR}/shaders/*.vert"
)

# Set the output directory for the SPIR-V files
set(SPIRV_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")

# Ensure the SPIRV_OUTPUT_DIR exists
file(MAKE_DIRECTORY ${SPIRV_OUTPUT_DIR})

# Create a list to hold the SPIR-V files
set(SPIRV_BINARY_FILES)

# Compile each GLSL file to SPIR-V
foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${SPIRV_OUTPUT_DIR}/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL}
    COMMENT "Compiling ${GLSL} to SPIR-V"
  )
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach()

# Create a custom target to compile the shaders
add_custom_target(
  Shaders
  DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(hehcraft Shaders)

add_custom_target(CopyModels ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/models
    ${CMAKE_BINARY_DIR}/models
)

add_dependencies(hehcraft CopyModels)