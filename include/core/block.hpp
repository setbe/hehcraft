#pragma once

#include "core/texture.hpp"

// libs
#include <glad/glad.h>

// std
#include <string>

namespace heh {

/**
 * @brief The GrassTexture class represents a texture for grass blocks.
 * 
 * This class inherits from the Texture class and provides functionality
 * specific to grass textures. It overrides the GetFullPath() method to
 * return the full path of the grass texture based on the face and texture
 * name provided.
 */
class GrassTexture : public Texture {
 public:
  /**
   * @brief Constructs a GrassTexture object.
   * 
   * This constructor initializes a GrassTexture object with the specified
   * face and texture name. If no texture name is provided, the default
   * texture name is set to "grass".
   * 
   * @param face The face of the texture.
   * @param tex_name The name of the texture (default: "grass").
   */
  GrassTexture(Face face, const std::string &tex_name = "grass") 
    : Texture(face, GetFullPath(face, tex_name)) {}

  /**
   * @brief Destroys the GrassTexture object.
   * 
   * This destructor is responsible for cleaning up any resources
   * associated with the GrassTexture object.
   */
  ~GrassTexture() {}

  /**
   * @brief Deleted copy constructor.
   * 
   * The copy constructor for GrassTexture is deleted to prevent
   * copying of GrassTexture objects.
   */
  GrassTexture(const GrassTexture&) = delete;

  /**
   * @brief Deleted copy assignment operator.
   * 
   * The copy assignment operator for GrassTexture is deleted to
   * prevent assignment of GrassTexture objects.
   */
  GrassTexture& operator=(const GrassTexture&) = delete;

  /**
   * @brief Returns the full path of the grass texture.
   * 
   * This method overrides the GetFullPath() method of the base
   * Texture class. It returns the full path of the grass texture
   * based on the face and texture name provided.
   * 
   * @param face The face of the texture.
   * @param tex_name The name of the texture.
   * @return The full path of the grass texture.
   */
  std::string GetFullPath(Face face, const std::string &tex_name) const override;
};

class CobblestoneTexture : public Texture {
 public:
  CobblestoneTexture(Face face = Face::kFront, const std::string &tex_name = "cobblestone")
    : Texture(face, GetFullPath(face, tex_name)) {}

  ~CobblestoneTexture() {}

  CobblestoneTexture(const CobblestoneTexture&) = delete;
  CobblestoneTexture& operator=(const CobblestoneTexture&) = delete;

  std::string GetFullPath(Face face, const std::string &tex_name) const override;
}; // class CobblestoneTexture

}  // namespace heh