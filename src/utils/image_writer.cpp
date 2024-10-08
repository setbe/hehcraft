#include "utils/image_writer.hpp"

#include <stb/stb_image_write.h>
#include <stb/stb_image.h>

#include <vector>
#include <string>
#include <filesystem>
#include <cmath>
#include <cassert>
#include <glad/glad.h>


namespace heh {

void ImageWriter::CreateAtlas(const std::string& tex_path, const std::string& output_file) {
 
  std::vector<ImageLocation> image_locations;

  int num_files = 0;
  for (const auto& image : std::filesystem::directory_iterator(tex_path)) {
    num_files++;
  }

  int single_texture_size = 16;
  int total_size = std::sqrt(num_files) * single_texture_size;
  out_size_ = static_cast<int>(std::pow(2, std::ceil(std::log2(total_size))));  // next power of 2

  int current_x = 0;
  int current_y = 0;
  int line_height = 0;
  std::vector<Pixel> pixels_list(out_size_ * out_size_);

  //stbi_set_flip_vertically_on_load(true);
  for (const auto& image : std::filesystem::directory_iterator(tex_path)) {
    int w, h, channels;
    unsigned char* raw_pixels = stbi_load(image.path().string().c_str(), &w, &h, &channels, 4);
    assert(channels == 4 && "Need 4 channels");

    if (current_x + w > out_size_) {
      current_x = 0;
      current_y += line_height;
      line_height = h;
    }

    /* 
    Save image locations:
      - x, y : the position of the texture in the atlas
      - width, height : the size of the texture
      - name : the name of the texture
    */
    image_locations.push_back(
      {current_x, current_y,            // x, y 
      w, h,                             // width, height
      image.path().stem().string()} // name
    );

    for (int x = 0; x < w; x++) {
      for (int y = 0; y < h; y++) {
        int local_x = current_x + x;
        int local_y = current_y + y;

        Pixel& current_pixel = pixels_list[local_y * out_size_ + local_x];
        unsigned char* current_raw_pixel = raw_pixels + (y * w + x) * 4;

        current_pixel.r = current_raw_pixel[0];
        current_pixel.g = current_raw_pixel[1];
        current_pixel.b = current_raw_pixel[2];
        current_pixel.a = current_raw_pixel[3];
      }
    }
    current_x += w;
    line_height = std::max(line_height, h);
    stbi_image_free(raw_pixels);
  }

  stbi_write_png(output_file.c_str(), out_size_, out_size_, 4, pixels_list.data(), out_size_ * sizeof(Pixel));

  config::TextureConfig texture_config;
  config::BlockConfig block_config;
  for (const auto &location : image_locations)
  {
    texture_config.name = location.name;

    texture_config.uvs[0] =
    { (location.x + location.width) / static_cast<float>(out_size_),
      location.y / static_cast<float>(out_size_) };
    texture_config.uvs[1] =
    { (location.x + location.width) / static_cast<float>(out_size_),
     (location.y + location.height) / static_cast<float>(out_size_) };
    texture_config.uvs[2] =
    { location.x / static_cast<float>(out_size_),
    (location.y + location.height) / static_cast<float>(out_size_) };
    texture_config.uvs[3] =
    { location.x / static_cast<float>(out_size_),
     location.y / static_cast<float>(out_size_) };
    

    config::file.textures[texture_config.name] = texture_config;
  }
  config::SaveTexturesConfig();
  

  GenerateGLTexture(pixels_list);
}

void ImageWriter::GenerateGLTexture(const std::vector<Pixel>& pixels_list) {
  glGenTextures(1, &atlas_texture_id_);
  glBindTexture(GL_TEXTURE_2D, atlas_texture_id_);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, out_size_, out_size_, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels_list.data());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);
}

 
} // namespace heh