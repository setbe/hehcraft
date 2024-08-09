#pragma once

// libs
#include <toml11/toml.hpp>
#include <glm/glm.hpp>

#include "utils/image_writer.hpp"

// std
#include <unordered_map>
#include <string>
#include <vector>

namespace heh {
  
  struct BlockFormat {
    std::string side;
    std::string top;
    std::string bottom;
  };

  struct TextureFormat {
    std::string name;
    glm::vec2 uvs[4];
  };

  namespace block_map {
    extern std::unordered_map<int, std::string> id_to_name;
    extern std::vector<BlockFormat> block_formats;
    extern std::unordered_map<std::string, TextureFormat> texture_formats;

    void LoadBlocks();

  } // namespace block_map  

} // namespace heh