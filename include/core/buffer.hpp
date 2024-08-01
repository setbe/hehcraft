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
 * It encapsulates the creation and deletion of buffer objects, as well as setting
 * and updating buffer data.
 */
class Buffer {
 public:

  /**
   * @brief Constructs a Buffer object with the specified target.
   * @param target The target of the buffer object.
   *
   * This constructor creates a buffer object with the specified target using glCreateBuffers.
   */
  Buffer(GLenum target) : target_(target) { glCreateBuffers(1, &id_); }

  /**
   * @brief Destructor.
   *
   * This destructor deletes the buffer object using glDeleteBuffers.
   */
  ~Buffer() { glDeleteBuffers(1, &id_); }

  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;

  /**
   * @brief Sets the data of the buffer object.
   * @param size The size of the data in bytes.
   * @param data A pointer to the data.
   * @param usage The usage pattern of the data.
   *
   * This function sets the data of the buffer object using glNamedBufferData.
   */
  inline void SetData(GLsizeiptr size, const void* data, GLenum usage) {
    glNamedBufferData(id_, size, data, usage);
  }

  /**
   * @brief Updates a portion of the buffer object's data.
   * @param offset The offset in bytes from the beginning of the buffer object.
   * @param size The size of the data in bytes.
   * @param data A pointer to the data.
   *
   * This function updates a portion of the buffer object's data using glNamedBufferSubData.
   */
  inline void SetSubData(GLintptr offset, GLsizeiptr size, const void* data) {
    glNamedBufferSubData(id_, offset, size, data);
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
  // glCreateVertexArrays(1, &id_)
  VertexArray() { glCreateVertexArrays(1, &id_); }
  ~VertexArray() { glDeleteVertexArrays(1, &id_); }

  VertexArray(const VertexArray&) = delete;
  VertexArray& operator=(const VertexArray&) = delete;

  inline void EnableVertexAttrib(GLuint index) { glEnableVertexArrayAttrib(id_, index); }
  inline void VertexAttribFormat(GLuint index, GLint size, GLenum type, GLboolean normalized, GLuint relative_offset) {
    glVertexArrayAttribFormat(id_, index, size, type, normalized, relative_offset);
  }
  inline void VertexAttribBinding(GLuint attribindex, GLuint bindingindex) {
    glVertexArrayAttribBinding(id_, attribindex, bindingindex);
  }
  inline void VertexBuffer(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride) {
    glVertexArrayVertexBuffer(id_, bindingindex, buffer, offset, stride);
  }
  inline void ElementBuffer(GLuint buffer) { glVertexArrayElementBuffer(id_, buffer); }

  inline void Bind() const { glBindVertexArray(id_); }
  inline void Unbind() const { glBindVertexArray(0); }

  inline GLuint GetID() const { return id_; }

 private:
  GLuint id_;
};
