#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
 public:
  Shader(const std::string& vertex_path, const std::string& fragment_path) {
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertex_code;
    std::string fragment_code;
    std::ifstream v_shader_file;
    std::ifstream f_shader_file;

    // Ensure ifstream objects can throw exceptions:
    v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
      // Open files
      v_shader_file.open(vertex_path);
      f_shader_file.open(fragment_path);
      std::stringstream v_shader_stream, f_shader_stream;

      // Read file's buffer contents into streams
      v_shader_stream << v_shader_file.rdbuf();
      f_shader_stream << f_shader_file.rdbuf();

      // Close file handlers
      v_shader_file.close();
      f_shader_file.close();

      // Convert stream into string
      vertex_code = v_shader_stream.str();
      fragment_code = f_shader_stream.str();
    } catch (std::ifstream::failure& e) {
      throw std::runtime_error("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
    }

    const char* v_shader_code = vertex_code.c_str();
    const char* f_shader_code = fragment_code.c_str();

    // 2. Compile shaders
    GLuint vertex, fragment;
    GLint success;
    GLchar info_log[512];

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_shader_code, nullptr);
    glCompileShader(vertex);
    // Print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(vertex, 512, nullptr, info_log);
      throw std::runtime_error(std::string("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n") + info_log);
    }

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &f_shader_code, nullptr);
    glCompileShader(fragment);
    // Print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(fragment, 512, nullptr, info_log);
      throw std::runtime_error(std::string("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n") + info_log);
    }

    // Shader Program
    id_ = glCreateProgram();
    glAttachShader(id_, vertex);
    glAttachShader(id_, fragment);
    glLinkProgram(id_);
    // Print linking errors if any
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(id_, 512, nullptr, info_log);
      throw std::runtime_error(std::string("ERROR::SHADER::PROGRAM::LINKING_FAILED\n") + info_log);
    }

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }

  ~Shader() {
    glDeleteProgram(id_);
  }

  void Use() const {
    glUseProgram(id_);
  }

  GLuint GetID() const {
    return id_;
  }

  // Utility uniform functions
  void SetBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(id_, name.c_str()), (int)value);
  }

  void SetInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
  }

  void SetFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
  }

  void SetVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(id_, name.c_str()), 1, &value[0]);
  }

  void SetVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(id_, name.c_str()), 1, &value[0]);
  }

  void SetVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(id_, name.c_str()), 1, &value[0]);
  }

  void SetMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  }

 private:
  GLuint id_;
};
