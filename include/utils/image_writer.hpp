#pragma once

#include "utils/toml_extended.hpp"

// libs
#include <stb/stb_image_write.h>
#include <stb/stb_image.h>

#include <glad/glad.h>

// std
#include <vector>
#include <string>
#include <filesystem>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <glad/glad.h>

namespace heh {

  struct Pixel {
    unsigned char r, g, b, a;
  };

  struct ImageLocation {
    int x, y, width, height;
    std::string name;
  };

  class ImageWriter {
  public:
    ImageWriter() : atlas_texture_id_(0), out_size_(0) {}

    ~ImageWriter() {
      glDeleteTextures(1, &atlas_texture_id_);
    }

    void CreateAtlas(const std::string& tex_path, const std::string& output_file);

    void BindAtlas() const {
      glBindTexture(GL_TEXTURE_2D, atlas_texture_id_);
    }

    uint16_t GetAtlasSize() const {
      return out_size_;
    }

  private:
    GLuint atlas_texture_id_;
    uint16_t out_size_;

    void GenerateGLTexture(const std::vector<Pixel>& pixels_list);
  };

} // namespace heh