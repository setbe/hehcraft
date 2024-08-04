#include "core/texture.hpp"

// libs
#include <glad/glad.h>

#include <stb/stb_image.h>

// std
#include <string>
#include <stdexcept>

namespace heh {

  void Texture::InitializeTexture() {
    glGenTextures(1, &id_);
    int width, height, nr_channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(GetPath().c_str(), &width, &height, &nr_channels, 0);
    if (!data)
      throw std::runtime_error("Failed to load texture: " + GetPath());

    glBindTexture(GL_TEXTURE_2D, id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
  }

  std::string Texture::GetFaceName(Face face) const {
    switch (face) {
    case Face::kFront:
      return "front";
    case Face::kBack:
      return "back";
    case Face::kLeft:
      return "left";
    case Face::kRight:
      return "right";
    case Face::kTop:
      return "top";
    case Face::kBottom:
      return "bottom";
    default:
      throw std::invalid_argument("Unknown Face value");
    }
  }


}  // namespace heh