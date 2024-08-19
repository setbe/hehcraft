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

  constexpr uint32_t kChunkWidth = 16;
  constexpr uint32_t kChunkDepth = 16;
  constexpr uint32_t kChunkHeight = 256;

  struct Block {
    // One most significant bit is used to indicate if the block is transparent
    uint16_t id;
    uint8_t light_level;
    uint8_t rotation;

    /*
      0000 0000
      |||| ||||
      |||| |||+---> Top face    (1 << 0)
      |||| ||+----> Front face  (1 << 1)
      |||| |+-----> Bottom face (1 << 2)
      |||| +------> Right face  (1 << 3)
      |||+--------> Left face   (1 << 4)
      ||+---------> Back face   (1 << 5)
      |+----------> All faces were culled        (1 << 6)
      +-----------> Only the top face is visible (1 << 7)
    */
    uint8_t culled_faces;
    uint16_t padding;

    static enum class Face : uint8_t
    {
      Top = 0,
      Front = 1,
      Bottom = 2,
      Right = 3,
      Left = 4,
      Back = 5,
      AllCulled = 6,
      OnlyTopVisible = 7
    };

    inline bool GetFace(Face face) const noexcept;
    void SetFace(Face face, bool visible) noexcept;

    inline bool IsTransparent() const noexcept {
      // MSB is checked using a bitmask. 
      return (id & 0x8000) != 0; // 0x8000 = 1000 0000 0000 0000 in binary
    }

    void CalculateLightLevel() noexcept;

    static int To1DArrayIndex(int x, int y, int z);
    static const Block& At(Block* blocks_data, int x, int y, int z);
    static bool OnChunkEdge(int x, int y, int z);
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