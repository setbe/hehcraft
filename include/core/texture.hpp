#pragma once

// libs
#include <glad/glad.h>

// std
#include <string>
#include <stdexcept>

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
          texture_name_(block_name + "_" + GetFaceName(face)) 
    {
        InitializeTexture();
    }

    virtual ~Texture() {
        glDeleteTextures(1, &id_);
    }

    void Bind() const {
        glBindTexture(GL_TEXTURE_2D, id_);
    }

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    std::string GetBlockName() const { return block_name_; }

    std::string GetPath(const std::string &image_format = ".png") const {
        return "textures/" + block_name_ + "/" + texture_name_ + image_format;
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

}  // namespace heh