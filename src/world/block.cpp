#include "world/block.hpp"

// std
#include <unordered_map>
#include <string>
#include <vector>

namespace heh {
  namespace block_map {
    std::unordered_map<int, std::string> id_to_name;
    std::vector<BlockFormat> block_formats;
    std::unordered_map<std::string, TextureFormat> texture_formats;

    void LoadBlocks()
    {

      block_formats.push_back({"", "", "", true}); // Null block
      for (const auto& block_config : config::file.blocks)
      {
        // block_config.first = name of the block
        BlockFormat block;
        int id = config::file.blocks[block_config.first].id;
        block.side = config::file.blocks[block_config.first].side;
        block.top = config::file.blocks[block_config.first].top;
        block.bottom = config::file.blocks[block_config.first].bottom;

        id_to_name[id] = block_config.first;
        block_formats.push_back(block);
      }

      for (const auto& texture_config : config::file.textures)
      {
        // texture_config.first = name of the texture
        TextureFormat texture;
        texture.name = config::file.textures[texture_config.first].name;
        for (size_t i = 0; i < 4; ++i)
        {
          texture.uvs[i] = config::file.textures[texture_config.first].uvs[i];
        }
        texture_formats[texture.name] = texture;
      }
    }
    const BlockFormat& GetBlock(const std::string& name)
    {
      return block_formats[config::file.blocks[name].id];
    }

    const BlockFormat& GetBlock(int id)
    {
      return block_formats[id];
    }
  } // namespace block_map

} // namespace heh