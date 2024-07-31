#pragma once

// libs
#include <glad/glad.h>

// std
#include <stdexcept>

/**
 * @class Buffer
 * @brief Represents a buffer object in OpenGL.
 *
 * The Buffer class provides a convenient interface for managing buffer objects in OpenGL.
 * It encapsulates the creation, binding, and deletion of buffer objects, as well as setting
 * and updating buffer data.
 */
class Buffer {
 public:

  /**
   * @brief Constructs a Buffer object with the specified target.
   * @param target The target of the buffer object.
   *
   * This constructor creates a buffer object with the specified target using glGenBuffers.
   */
  Buffer(GLenum target) : target_(target) { glGenBuffers(1, &id_); }

  /**
   * @brief Destructor.
   *
   * This destructor deletes the buffer object using glDeleteBuffers.
   */
  ~Buffer() { glDeleteBuffers(1, &id_); }

  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;

  /**
   * @brief Binds the buffer object.
   *
   * This function binds the buffer object to the specified target using glBindBuffer.
   */
  inline void Bind() const { glBindBuffer(target_, id_); }

  /**
   * @brief Unbinds the buffer object.
   *
   * This function unbinds the buffer object by binding the target to 0 using glBindBuffer.
   */
  inline void Unbind() const { glBindBuffer(target_, 0); }

  /**
   * @brief Sets the data of the buffer object.
   * @param size The size of the data in bytes.
   * @param data A pointer to the data.
   * @param usage The usage pattern of the data.
   *
   * This function sets the data of the buffer object using glBufferData.
   */
  inline void SetData(GLsizeiptr size, const void* data, GLenum usage) {
    glBufferData(target_, size, data, usage);
  }

  /**
   * @brief Updates a portion of the buffer object's data.
   * @param offset The offset in bytes from the beginning of the buffer object.
   * @param size The size of the data in bytes.
   * @param data A pointer to the data.
   *
   * This function updates a portion of the buffer object's data using glBufferSubData.
   */
  inline void SetSubData(GLintptr offset, GLsizeiptr size, const void* data) {
    glBufferSubData(target_, offset, size, data);
  }

  /**
   * @brief Returns the ID of the buffer object.
   * @return The ID of the buffer object.
   *
   * This function returns the ID of the buffer object.
   */
  GLuint GetID() const { return id_; }

 private:
  GLuint id_; ///< The ID of the buffer object.
  GLenum target_; ///< The target of the buffer object.
};



/**
 * @class VertexArray
 * @brief Represents a vertex array object (VAO) in OpenGL.
 *
 * The VertexArray class encapsulates the functionality of a vertex array object (VAO) in OpenGL.
 * It provides methods to set up vertex attribute pointers, enable vertex attributes, bind and unbind the VAO,
 * and retrieve the ID of the VAO.
 */
class VertexArray {
 public:
  // glGenVertexArrays(1, &id_)
  VertexArray() { glGenVertexArrays(1, &id_); }
  ~VertexArray() { glDeleteVertexArrays(1, &id_); }

  VertexArray(const VertexArray&) = delete;
  VertexArray& operator=(const VertexArray&) = delete;

  inline void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) {
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
  }
  inline void EnableVertexAttribArray(GLuint index) { glEnableVertexAttribArray(index); }

  inline void Bind() const { glBindVertexArray(id_); }
  inline void Unbind() const { glBindVertexArray(0); }

  inline GLuint GetID() const { return id_; }

 private:
  GLuint id_;
};
