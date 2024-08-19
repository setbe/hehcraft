#include "world/block.hpp"

// std
#include <unordered_map>
#include <string>
#include <vector>

namespace heh {
  namespace block_map {
    extern Block kNullBlock{ 0, 0, 0, 0 };

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
        block.is_transparent = config::file.blocks[block_config.first].is_transparent;

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


  inline bool Block::GetFace(Block::Face face) const noexcept {
    return !(culled_faces & (1 << static_cast<uint8_t>(face)));
  }

  void Block::SetFace(Block::Face face, bool visible) noexcept {
    if (visible) {
      // Clear the bit corresponding to the specified face to make it visible.
      culled_faces &= ~(1 << static_cast<uint8_t>(face));

      // If any face becomes visible, clear the "All faces were culled" bit.
      culled_faces &= ~(1 << static_cast<uint8_t>(Face::AllCulled));

      // If the top face is visible, but other faces are visible too, clear the "Only top face is visible" bit.
      if (face != Face::Top) {
        culled_faces &= ~(1 << static_cast<uint8_t>(Face::OnlyTopVisible));
      }
      else if ((culled_faces & 0x3E) != 0) { // 0x3E masks all but the top face.
        culled_faces &= ~(1 << static_cast<uint8_t>(Face::OnlyTopVisible));
      }
    }
    else {
      // Set the bit corresponding to the specified face to make it culled.
      culled_faces |= (1 << static_cast<uint8_t>(face));

      // Check if all faces are culled, and if so, set the "All faces were culled" bit.
      if ((culled_faces & 0x3F) == 0x3F) { // 0x3F corresponds to all 6 faces.
        culled_faces |= (1 << static_cast<uint8_t>(Face::AllCulled));
      }

      // If only the top face is not culled, set the "Only top face is visible" bit.
      if ((culled_faces & 0x3E) == 0x3E) { // 0x3E masks all but the top face.
        culled_faces |= (1 << static_cast<uint8_t>(Face::OnlyTopVisible));
      }
    }
  }
  
  int Block::To1DArrayIndex(int x, int y, int z)
  {
    return (x * kChunkDepth) + (y * kChunkHeight) + z;
  }

  const Block& Block::At(Block* blocks_data, int x, int y, int z)
  {
    int index = To1DArrayIndex(x, y, z);
    return
      x >= 16 || x < 0 ||
      z >= 16 || z < 0 ||
      y >= 256 || y < 0 ?
      block_map::kNullBlock :
      blocks_data[index];
  }

  bool Block::OnChunkEdge(int x, int y, int z)
  {
    return x == 0 || x == kChunkWidth - 1 ||
      y == 0 || y == kChunkHeight - 1 ||
      z == 0 || z == kChunkDepth - 1;
  }

} // namespace heh