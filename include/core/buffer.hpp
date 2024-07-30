#pragma once

#include <glad/glad.h>
#include <stdexcept>

class Buffer {
 public:

  // glGenBuffers(1, &id_)
  Buffer(GLenum target) : target_(target) { glGenBuffers(1, &id_); }
  ~Buffer() { glDeleteBuffers(1, &id_); }

  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;

  inline void Bind() const { glBindBuffer(target_, id_); }
  inline void Unbind() const { glBindBuffer(target_, 0); }

  inline void SetData(GLsizeiptr size, const void* data, GLenum usage) {
    glBufferData(target_, size, data, usage);
  }
  inline void SetSubData(GLintptr offset, GLsizeiptr size, const void* data) {
    glBufferSubData(target_, offset, size, data);
  }

  GLuint GetID() const { return id_; }

 private:
  GLuint id_;
  GLenum target_;
};



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
