#pragma once

// libs
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb/stb_image.h>

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

    virtual std::string GetFaceName(Face face) const {
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

    unsigned int GetID() const {
        return id_;
    }

private:
    void InitializeTexture() {
        glCreateTextures(GL_TEXTURE_2D, 1, &id_);
        int width, height, nr_channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(GetPath().c_str(), &width, &height, &nr_channels, 0);
        if (!data) {
            throw std::runtime_error("Failed to load texture: " + GetPath());
        }

        glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureStorage2D(id_, 1, GL_RGBA8, width, height);
        glTextureSubImage2D(id_, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateTextureMipmap(id_);

        stbi_image_free(data);
    }

    std::string block_name_;
    std::string texture_name_;
    unsigned int id_;
};

class TextureManager {
public:
    static std::shared_ptr<Texture> Create(const std::string &block_name, Texture::Face face) {
        auto texture = std::make_shared<Texture>(block_name, face);
        std::string key = texture->GetTextureName();
        if (textures_.find(key) == textures_.end()) {
            textures_[key] = texture;
        }
        return textures_[key];
    }

    static std::shared_ptr<Texture> GetTexture(const std::string &texture_name) {
        if (textures_.find(texture_name) == textures_.end()) {
            throw std::runtime_error("Texture not found: " + texture_name);
        }
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

std::unordered_map<std::string, std::shared_ptr<Texture>> TextureManager::textures_;

}  // namespace heh