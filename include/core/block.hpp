#pragma once

#include "core/texture.hpp"

// libs
#include <glad/glad.h>

// std
#include <string>

namespace heh {

class GrassTexture : public Texture {
 public:
  GrassTexture(Face face, const std::string &tex_name = "grass") 
    : Texture(face, GetFullPath(face, tex_name)) {}

  ~GrassTexture() {}

  GrassTexture(const GrassTexture&) = delete;
  GrassTexture& operator=(const GrassTexture&) = delete;

  std::string GetFullPath(Face face, const std::string &tex_name) const override;
};

}  // namespace heh