#pragma once

#include "core/buffer.hpp"
#include "block.hpp"

// libs
#include <glad/glad.h>
#include <glm/glm.hpp>

// std
#include <cstdint>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>



namespace heh {

  constexpr const uint16_t kAtlasSize = 64;
  constexpr const uint16_t kTextureSize = 16;

  static constexpr uint32_t kChunkWidth = 16;
  static constexpr uint32_t kChunkDepth = 16;
  static constexpr uint32_t kChunkHeight = 256;

  struct Vertex
  {
    glm::vec3 position;
    glm::vec2 tex_coords;
    glm::vec3 normal;
  };

  struct ChunkRenderData
  {
    std::vector<Vertex> vertices;
    std::vector<int32_t> elements;

    size_t vertex_size_bytes;
    size_t element_size_bytes;
    uint32_t num_elements;
  };

  struct Chunk
  {
    ChunkRenderData data;
    std::vector<Block> blocks_data;
    glm::ivec2 chunk_position;

    void Generate(int32_t seed, int chunk_x, int chunk_z);
    void BuildMesh();
    void UploadToGpu();
    void ClearCpuData();
    void Render();

    void Serialize(const std::string& world_filename);
    void Deserialize(const std::string& world_filename, uint32_t x, uint32_t y);

  private:
    Buffer vbo_{ GL_ARRAY_BUFFER };
    Buffer ebo_{ GL_ELEMENT_ARRAY_BUFFER };
    VertexArray vao_{};
  };

}  // namespace heh