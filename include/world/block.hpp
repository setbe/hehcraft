#pragma once

// libs
#include <toml11/toml.hpp>
#include <glm/glm.hpp>

// std
#include <unordered_map>
#include <string>
#include <vector>

namespace heh {
  struct BlockFormat {
    std::string side_texture;
    std::string top_texture;
    std::string bottom_texture;
  };

  struct TextureFormat {
    std::string name;
    glm::vec2 uvs[4];
  };

  namespace block_map {
    std::unordered_map<std::string, int> name_to_id_map;
    std::vector<BlockFormat> block_formats;
    std::vector<std::string, TextureFormat> texture_formats_map;

    const BlockFormat& GetBlockFormat(const std::string& name);
    void LoadBlock(const std::string& name);

  } // namespace block_map
    
} // namespace heh