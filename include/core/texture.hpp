#pragma once

// libs
#include <glad/glad.h>

// std
#include <string>

namespace heh {

class Texture {
 public:
  enum class Face {
    kFront,
    kBack,
    kLeft,
    kRight,
    kTop,
    kBottom
  };
  
  Texture(Face face, const std::string &full_path);
  virtual ~Texture() { glDeleteTextures(1, &id_); }

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  void Bind() const { glBindTexture(GL_TEXTURE_2D, id_); }
  void Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

  unsigned int GetID() const { return id_; }
  std::string GetBasePath(const std::string &tex_name) const { return "textures/" + tex_name + "/"; }
  virtual std::string GetFullPath(Face face, const std::string &tex_name) const;

 private:
  unsigned int id_;
};


}  // namespace heh