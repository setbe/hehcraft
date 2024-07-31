#pragma once

// libs
#include <glad/glad.h>
#include <glm/glm.hpp>

// std
#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>

namespace heh {

/**
 * @brief The Shader class represents a shader program in OpenGL.
 * 
 * This class provides functionality to load, compile, and link vertex and fragment shaders,
 * as well as set uniform values in the shader program.
 */
class Shader {
 public:
  /**
   * @brief Constructs a Shader object with the specified vertex and fragment shader file paths.
   * 
   * @param vertex_path The file path to the vertex shader source code.
   * @param fragment_path The file path to the fragment shader source code.
   * 
   * @throws std::runtime_error if the shader files fail to be read or if there are compilation or linking errors.
   */
  Shader(const std::string& vertex_path, const std::string& fragment_path);

  /**
   * @brief Destroys the Shader object and releases the associated OpenGL resources.
   */
  ~Shader();

  /**
   * @brief Activates the shader program for use.
   */
  void Use() const;

  /**
   * @brief Retrieves the ID of the shader program.
   * 
   * @return The ID of the shader program.
   */
  GLuint GetID() const;

  /**
   * @brief Sets a boolean uniform value in the shader program.
   * 
   * @param name The name of the boolean uniform.
   * @param value The boolean value to set.
   */
  void SetBool(const std::string& name, bool value) const;

  /**
   * @brief Sets an integer uniform value in the shader program.
   * 
   * @param name The name of the integer uniform.
   * @param value The integer value to set.
   */
  void SetInt(const std::string& name, int value) const;

  /**
   * @brief Sets a floating-point uniform value in the shader program.
   * 
   * @param name The name of the floating-point uniform.
   * @param value The floating-point value to set.
   */
  void SetFloat(const std::string& name, float value) const;

  /**
   * @brief Sets a 2D vector uniform value in the shader program.
   * 
   * @param name The name of the 2D vector uniform.
   * @param value The 2D vector value to set.
   */
  void SetVec2(const std::string& name, const glm::vec2& value) const;

  /**
   * @brief Sets a 3D vector uniform value in the shader program.
   * 
   * @param name The name of the 3D vector uniform.
   * @param value The 3D vector value to set.
   */
  void SetVec3(const std::string& name, const glm::vec3& value) const;

  /**
   * @brief Sets a 4D vector uniform value in the shader program.
   * 
   * @param name The name of the 4D vector uniform.
   * @param value The 4D vector value to set.
   */
  void SetVec4(const std::string& name, const glm::vec4& value) const;

  /**
   * @brief Sets a 4x4 matrix uniform value in the shader program.
   * 
   * @param name The name of the matrix uniform.
   * @param mat The 4x4 matrix value to set.
   */
  void SetMat4(const std::string& name, const glm::mat4& mat) const;

 private:
  GLuint id_; /**< The ID of the shader program. */
};  // class Shader

} // namespace heh
