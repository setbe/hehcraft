#pragma once

// libs
#include <stb/stb_image_write.h>
#include <stb/stb_image.h>

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

  class ImageWriter {
  public:
    ImageWriter() : atlas_texture_id_(0), out_width_(0), out_height_(0) {}

    ~ImageWriter() {
      glDeleteTextures(1, &atlas_texture_id_);
    }

    void CreateAtlas(const std::string& tex_path, const std::string& output_file);

    void BindAtlas() const {
      glBindTexture(GL_TEXTURE_2D, atlas_texture_id_);
    }

    uint32_t GetAtlasSize() const {
      return out_width_;
    }

  private:
    GLuint atlas_texture_id_;
    uint32_t out_width_;
    uint32_t out_height_;

    void GenerateGLTexture(const std::vector<Pixel>& pixels_list);
  };

} // namespace heh