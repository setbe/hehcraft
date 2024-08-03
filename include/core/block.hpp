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
   struct Vertex {
     glm::vec3 Position;
     glm::vec2 TexCoords;
     glm::vec3 Normal;
   };

  Block(const std::string& block_name) 
  {
    using Face = Texture::Face;
    textures_[0] = texture_manager_.Create(block_name, Face::kFront);
    textures_[1] = texture_manager_.Create(block_name, Face::kBack);
    textures_[2] = texture_manager_.Create(block_name, Face::kLeft);
    textures_[3] = texture_manager_.Create(block_name, Face::kRight);
    textures_[4] = texture_manager_.Create(block_name, Face::kTop);
    textures_[5] = texture_manager_.Create(block_name, Face::kBottom);

    for (auto& texture : textures_) {
      if (!texture) {
        throw std::runtime_error("Texture not found for block: " + block_name);
      }
    }
  }

  virtual ~Block() {}

  Block(const Block&) = delete;
  Block& operator=(const Block&) = delete;

  virtual void Draw(Shader &shader, GLuint unit = 0) const {
    shader.SetMat4("model", model_);

    for (int i = 0; i < 6; ++i) {
      glBindTextureUnit(unit, textures_[i]->GetID());
      glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 6 * i), 0);
    }

    glBindVertexArray(0);
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

  
  //static GLuint indices[36];


 protected:
  glm::mat4 model_{1.0f};
  std::shared_ptr<Texture> textures_[6];
  
  static TextureManager texture_manager_;
}; // class Block


}  // namespace heh