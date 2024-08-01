#include "core/texture.hpp"

// libs
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb/stb_image.h>

// std
#include <stdexcept>

namespace heh {

Texture::Texture(Face face, const std::string &full_path) {
  glCreateTextures(GL_TEXTURE_2D, 1, &id_);
  int width, height, nr_channels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load(full_path.c_str(), &width, &height, &nr_channels, 0);
  if (!data)
    throw std::runtime_error("Failed to load texture: " + full_path);

  glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTextureStorage2D(id_, 1, GL_RGBA8, width, height);
  glTextureSubImage2D(id_, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateTextureMipmap(id_);

  stbi_image_free(data);
}

std::string Texture::GetFullPath(Face face, const std::string &tex_name) const {
  switch (face) {
    case Face::kFront:
      return GetBasePath(tex_name) + "front.png";
    case Face::kBack:
      return GetBasePath(tex_name) + "back.png";
    case Face::kLeft:
      return GetBasePath(tex_name) + "left.png";
    case Face::kRight:
      return GetBasePath(tex_name) + "right.png";
    case Face::kTop:
      return GetBasePath(tex_name) + "top.png";
    case Face::kBottom:
      return GetBasePath(tex_name) + "bottom.png";
    default:
      throw std::invalid_argument("Unknown Face value");
  }
}


}  // namespace heh