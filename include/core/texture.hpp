#pragma once

// libs
#include <glad/glad.h>

// std
#include <string>
#include <stdexcept>
#include <memory>
#include <unordered_map>

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

    Texture(const std::string &block_name, Face face)
        : id_(0),
          block_name_(block_name),
          texture_name_(block_name + "_" + GetFaceName(face)) {
        InitializeTexture();
    }

    virtual ~Texture() {
        glDeleteTextures(1, &id_);
    }

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    std::string GetBaseDirPath() const { return "textures/" + block_name_ + "/"; }
    std::string GetTextureName() const { return texture_name_; }
    std::string GetBlockName() const { return block_name_; }

    std::string GetPath(const std::string &image_format = ".png") const {
        return GetBaseDirPath() + GetTextureName() + image_format;
    }

    virtual std::string GetFaceName(Face face) const;

    unsigned int GetID() const {
        return id_;
    }

private:
  void InitializeTexture();

    std::string block_name_;
    std::string texture_name_;
    unsigned int id_;
};

class TextureManager {
public:
  static std::shared_ptr<Texture> Create(const std::string& block_name, Texture::Face face) {
    auto texture = std::make_shared<Texture>(block_name, face);
    std::string key = texture->GetTextureName();
    if (textures_.find(key) == textures_.end())
      textures_[key] = texture;

    return textures_[key];
  }

  static std::shared_ptr<Texture> GetTexture(const std::string& texture_name) {
    if (textures_.find(texture_name) == textures_.end())
      throw std::runtime_error("Texture not found: " + texture_name);

    return textures_[texture_name];
  }

  static void LoadAllTextures() {
    using Face = Texture::Face;

    Create("grass", Face::kFront);
    Create("grass", Face::kTop);
    Create("grass", Face::kBottom);

    Create("cobblestone", Face::kFront);
  }

private:
  static std::unordered_map<std::string, std::shared_ptr<Texture>> textures_;
};

}  // namespace heh