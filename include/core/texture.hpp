#pragma once

// libs
#include <glad/glad.h>

// std
#include <string>

namespace heh {

/**
 * @class Texture
 * @brief Represents a texture in OpenGL.
 *
 * The Texture class provides a convenient interface for managing textures in OpenGL.
 * It encapsulates the creation, binding, and deletion of textures, as well as loading
 * texture data from files.
 */
class Texture {
 public:

  /**
   * @brief Represents the face of a cube map texture.
   * kFront: The front face of the cube map and so on.
   */
  enum class Face {
    kFront,
    kBack,
    kLeft,
    kRight,
    kTop,
    kBottom
  };
  
  /**
   * @brief Constructs a Texture object with the specified face and full path.
   * @param face The face of the texture.
   * @param full_path The full path to the texture file.
   *
   * This constructor creates a texture object with the specified face and full path.
   * It loads the texture data from the file and generates a texture object using glGenTextures.
   */
  Texture(Face face, const std::string &full_path);

  /**
   * @brief Destructor.
   *
   * This destructor deletes the texture object using glDeleteTextures.
   */
  virtual ~Texture() { glDeleteTextures(1, &id_); }

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  /**
   * @brief Gets the base path of the texture.
   * @param tex_name The name of the texture.
   * @return The base path of the texture.
   * Returned path looks like: "textures/tex_name/"
   */
  std::string GetBasePath(const std::string &tex_name) const { return "textures/" + tex_name + "/"; }

  /**
   * @brief Gets the full path of the texture.
   * @param face The face of the texture.
   * @param tex_name The name of the texture.
   * @return The full path of the texture.
   * Returned path looks like: "textures/tex_name/front.png"
   * 
   * @note This function should be implemented by derived classes.
   * Derived classes could override this function to provide custom texture paths.
   */
  virtual std::string GetFullPath(Face face, const std::string &tex_name) const;

  unsigned int GetID() const { return id_; }

 private:
  unsigned int id_; //< The ID of the texture object.
};


}  // namespace heh