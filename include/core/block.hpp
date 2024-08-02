#pragma once

#include "core/texture.hpp"
#include "core/shader.hpp"

// libs
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// std
#include <string>
#include <memory>

namespace heh {

class Block {
 public:
  Block(const std::string& block_name) 
    : texture_(texture_manager_.GetTexture(block_name + "_front"))
  {
    if (!texture_)
      throw std::runtime_error("Texture not found for block: " + block_name);
  }

  virtual ~Block() {}

  Block(const Block&) = delete;
  Block& operator=(const Block&) = delete;

  virtual void Draw(Shader &shader, GLuint unit = 0) const {
    shader.SetMat4("model", model_);
    glBindTextureUnit(unit, texture_->GetID());
  }
  
  virtual void Translate(const glm::vec3 &translation) { 
    model_ = glm::translate(model_, translation); 
  }

  virtual void Rotate(float angle, const glm::vec3 &axis) { 
    model_ = glm::rotate(model_, angle, axis); 
  }

  virtual glm::mat4 GetModelMatrix() const { 
    return model_; 
  }

 protected:
  glm::mat4 model_{1.0f};
  std::shared_ptr<Texture> texture_{nullptr};
  
  static TextureManager texture_manager_;
}; // class Block


}  // namespace heh