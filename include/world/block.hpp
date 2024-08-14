#pragma once

// libs
#include <toml11/toml.hpp>
#include <glm/glm.hpp>

#include "utils/image_writer.hpp"

// std
#include <unordered_map>
#include <string>
#include <vector>
#include <array>

namespace heh {

  struct Block {
    int16_t id;
    int8_t light_level;
    int8_t rotation;
    int32_t padding;
  };

  struct BlockFormat {
    std::string side;
    std::string top;
    std::string bottom;
    bool is_transparent;
  };

  struct TextureFormat {
    std::string name;
    std::array<glm::vec2, 4> uvs;
  };

  namespace block_map {
    extern Block kNullBlock;

    extern std::unordered_map<int, std::string> id_to_name;
    extern std::vector<BlockFormat> block_formats;
    extern std::unordered_map<std::string, TextureFormat> texture_formats;

    void LoadBlocks();
    const BlockFormat& GetBlock(const std::string& name);
    const BlockFormat& GetBlock(int id);

  } // namespace block_map  

} // namespace heh