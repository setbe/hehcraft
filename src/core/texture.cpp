#include "core/texture.hpp"

// libs
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb/stb_image.h>

// std
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <memory>

namespace heh {
  std::unordered_map<std::string, std::shared_ptr<Texture>> TextureManager::textures_;

  void Texture::InitializeTexture() {
    int width, height, nr_channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(GetPath().c_str(), &width, &height, &nr_channels, 0);
    if (!data)
      throw std::runtime_error("Failed to load texture: " + GetPath());

    glCreateTextures(GL_TEXTURE_2D, 1, &id_);
    glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureStorage2D(id_, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(id_, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateTextureMipmap(id_);

    stbi_image_free(data);
  }

  std::string Texture::GetFaceName(Face face) const {
    switch (face) {
    case Face::kFront:  return "front";
    case Face::kBack:   return "back";
    case Face::kLeft:   return "left";
    case Face::kRight:  return "right";
    case Face::kTop:    return "top";
    case Face::kBottom: return "bottom";
    default:            return "unknown";
    }
  }

}  // namespace heh