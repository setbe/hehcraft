#include "world/chunk.hpp"


#include "simplexnoise/src/SimplexNoise.h"

// std
#include <vector>
#include <limits>
#include <array>
#include <type_traits>
#include <cstdint>
#include <random>

#include <filesystem>

#include <iostream>

namespace heh {
  enum class BlockFace : uint32_t
  {
    Top,
    Side,
    Bottom,
    Right,
    Left,
    Back
  };

  static int To1DArrayIndex(int x, int y, int z)
  {
    return (x * kChunkDepth) + (y * kChunkHeight) + z;
  }

  static bool IsBlockOnChunkEdge(int x, int y, int z)
  {
    return x == 0 || x == kChunkWidth  - 1 ||
           y == 0 || y == kChunkHeight - 1 ||
           z == 0 || z == kChunkDepth  - 1;
  }

  static const Block& GetBlock(Block* blocks_data, int x, int y, int z)
  {
    int index = To1DArrayIndex(x, y, z);
    return 
      x >= 16  || x < 0 || 
      z >= 16  || z < 0 || 
      y >= 256 || y < 0  ?
      block_map::kNullBlock :
      blocks_data[index];
  }

  template<typename T, uint8_t N_Vec, uint8_t N_Array>
  struct VecOrder
  {
    std::array<glm::vec<N_Vec, T>, N_Array> vec;
    std::array<uint8_t, N_Array> order;
  };

  static void LoadBlock(
    ChunkRenderData& render_data,
    const VecOrder<float, 3, 8>& verts,
    const VecOrder<float, 2, 4>& uv,
    const glm::vec3& normal,
    int vertex_cursor,
    int element_index_cursor,
    int element_cursor);
  

  void Chunk::Generate(int32_t seed, int chunk_x, int chunk_z)
  {
    const int world_chunk_x = chunk_x * 16;
    const int world_chunk_z = chunk_z * 16;

    chunk_position = { chunk_x, chunk_z };

    
    blocks_data.clear();

    
    blocks_data.resize(kChunkWidth * kChunkHeight * kChunkDepth);
    const SimplexNoise generator;

    for (int y = 0; y < kChunkHeight; ++y)
    {
      for (int x = 0; x < kChunkDepth; ++x)
      {
        for (int z = 0; z < kChunkWidth; ++z)
        {
          const int array1d = To1DArrayIndex(x, y, z);
          const float increment_size = 1000.0f;

          float max_height_float = generator.fractal(10, (x + world_chunk_x) / increment_size, (z + world_chunk_z) / increment_size) * 30.0f + 70.0f;
          int16_t max_height = (int16_t)max_height_float;


          if (y < max_height - 5)
            blocks_data[array1d].id = 3;
          else if (y < max_height)
            blocks_data[array1d].id = 1;
          else if (y == max_height)
            blocks_data[array1d].id = 2;
        } // for y
      } // for z
    } // for x
  }

  void Chunk::BuildMesh()
  {
    data.vertices.clear();
    data.elements.clear();
    data.vertices.resize(kChunkWidth * kChunkHeight * kChunkDepth * 24);
    data.elements.resize(kChunkWidth * kChunkHeight * kChunkDepth * 36);

    const int world_chunk_x = chunk_position.x * 16;
    const int world_chunk_z = chunk_position.y * 16;

    int vertex_cursor = 0;
    int element_cursor = 0;
    int element_index_cursor = 0;

    for (int y = 0; y < kChunkHeight; ++y) 
    {
      for (int x = 0; x < kChunkDepth; ++x)
      {
        for (int z = 0; z < kChunkWidth; ++z)
        {
          const Block& block = GetBlock(blocks_data.data(), x, y, z);
          const int block_id = block.id;

          if (block.id == block_map::kNullBlock.id)
            continue;

          const int array_expansion = To1DArrayIndex(x, y, z);

          const BlockFormat& block_format = block_map::GetBlock(block.id);

          const TextureFormat& tex_top = block_map::texture_formats[block_format.top];
          const TextureFormat& tex_side = block_map::texture_formats[block_format.side];
          const TextureFormat& tex_bottom = block_map::texture_formats[block_format.bottom];

          // verts begin

          //
          std::array<glm::vec3, 8> verts;
          verts[0] = glm::vec3(
            (float)x - 0.5f + world_chunk_x,
            (float)y + 0.5f,
            (float)z + 0.5f + world_chunk_z
          );
          verts[1] = verts[0] + glm::vec3(1.f, 0.f, 0.f);
          verts[2] = verts[1] - glm::vec3(0.f, 0.f, 1.f);
          verts[3] = verts[0] - glm::vec3(0.f, 0.f, 1.f);

          verts[4] = verts[0] - glm::vec3(0.f, 1.f, 0.f);
          verts[5] = verts[1] - glm::vec3(0.f, 1.f, 0.f);
          verts[6] = verts[2] - glm::vec3(0.f, 1.f, 0.f);
          verts[7] = verts[3] - glm::vec3(0.f, 1.f, 0.f);

          // verts end


          using VertsOrder = VecOrder<float, 3, 8>;
          using UvOrder = VecOrder<float, 2, 4>;
           
          // Top face
          const int top_block_id = GetBlock(blocks_data.data(), x, y + 1, z).id;
          const BlockFormat& top_block = block_map::GetBlock(top_block_id);
          if (!top_block_id || top_block.is_transparent)
          {
            LoadBlock(
              data,
              VertsOrder{ verts, { 0, 1, 2, 3 } },
              UvOrder{ tex_top.uvs, { 0, 1, 2, 3 } },
              { 0.f, 1.f, 0.f },
              vertex_cursor,
              element_index_cursor,
              element_cursor
            );
            vertex_cursor += 4;
            element_index_cursor += 6;
            element_cursor += 4;
          }

          // Right face
          const int right_block_id = GetBlock(blocks_data.data(), x, y, z + 1).id;
          const BlockFormat& right_block = block_map::GetBlock(right_block_id);
          if (!right_block_id || right_block.is_transparent) {
            LoadBlock(
              data,
              VertsOrder{ verts, { 0, 4, 5, 1 } },
              UvOrder{ tex_side.uvs, { 0, 1, 2, 3 } },
              { 1.f, 0.f, 0.f },
              vertex_cursor,
              element_index_cursor,
              element_cursor
            );
            vertex_cursor += 4;
            element_index_cursor += 6;
            element_cursor += 4;
          }

          // Forward face
          const int forward_block_id = GetBlock(blocks_data.data(), x + 1, y, z).id;
          const BlockFormat& forward_block = block_map::GetBlock(forward_block_id);
          if (!forward_block_id || forward_block.is_transparent) {
            LoadBlock(
              data,
              VertsOrder{ verts, { 1, 5, 6, 2 } },
              UvOrder{ tex_side.uvs, { 3, 2, 1, 0 } },
              { 0.f, 0.f, 1.f },
              vertex_cursor,
              element_index_cursor,
              element_cursor
            );
            vertex_cursor += 4;
            element_index_cursor += 6;
            element_cursor += 4;
          }

          // Left face
          const int left_block_id = GetBlock(blocks_data.data(), x, y, z - 1).id;
          const BlockFormat& left_block = block_map::GetBlock(left_block_id);
          if (!left_block_id || left_block.is_transparent) {
            LoadBlock(
              data,
              VertsOrder{ verts, { 2, 6, 7, 3 } },
              UvOrder{ tex_side.uvs, { 0, 1, 2, 3 } },
              { -1.f, 0.f, 0.f },
              vertex_cursor,
              element_index_cursor,
              element_cursor
            );
            vertex_cursor += 4;
            element_index_cursor += 6;
            element_cursor += 4;
          }

          // Back face
          const int back_block_id = GetBlock(blocks_data.data(), x - 1, y, z).id;
          const BlockFormat& back_block = block_map::GetBlock(back_block_id);
          if (!back_block_id || back_block.is_transparent) {
            LoadBlock(
              data,
              VertsOrder{ verts, { 3, 7, 4, 0 } },
              UvOrder{ tex_side.uvs, { 3, 2, 1, 0 } },
              { 0.f, 0.f, -1.f },
              vertex_cursor,
              element_index_cursor,
              element_cursor
            );
            vertex_cursor += 4;
            element_index_cursor += 6;
            element_cursor += 4;
          }

          // Bottom face
          const int bottom_block_id = GetBlock(blocks_data.data(), x, y - 1, z).id;
          const BlockFormat& bottom_block = block_map::GetBlock(bottom_block_id);
          if (!bottom_block_id || bottom_block.is_transparent)
          {
            // Bottom face
            LoadBlock(
              data,
              VertsOrder{ verts, { 7, 6, 5, 4 } },
              UvOrder{ tex_bottom.uvs, { 1, 0, 3, 2 } },
              { 0.f, -1.f, 0.f },
              vertex_cursor,
              element_index_cursor,
              element_cursor
            );
            vertex_cursor += 4;
            element_index_cursor += 6;
            element_cursor += 4;
          }

        } // for z
      } // for y
    } // for x

    std::cout << "Vertex size (before): " << data.vertices.size() << std::endl;
    std::cout << "vertex cursor: " << vertex_cursor << std::endl;
    std::cout << "Element size (before): " << data.elements.size() << std::endl;
    std::cout << "element cursor: " << element_cursor << std::endl;
    std::cout << "element index: " << element_index_cursor << std::endl;
    
    data.vertices.resize(vertex_cursor);
    data.elements.resize(element_cursor * 2);

    std::cout << "Vertex size (after): " << data.vertices.size() << std::endl;
    std::cout << "Element size (after): " << data.elements.size() << std::endl;

    std::cout << "------------------------------------\n";

    

    // Grab calculated data into a struct
    data.vertex_size_bytes = data.vertices.size() * sizeof(Vertex);
    data.element_size_bytes = data.elements.size() * sizeof(int32_t);
    data.num_elements = static_cast<uint32_t>(data.elements.size());
  }


  static void LoadBlock(
    ChunkRenderData& render_data,
    const VecOrder<float, 3, 8>& verts,
    const VecOrder<float, 2, 4>& uv,
    const glm::vec3& normal,
    int vertex_cursor,
    int element_index_cursor,
    int element_cursor)
  {
    for (int i = 0; i < 4; ++i)
    {
      render_data.vertices.data()[vertex_cursor + i].position = verts.vec[verts.order[i]];
      render_data.vertices.data()[vertex_cursor + i].tex_coords = uv.vec[uv.order[i]];
      render_data.vertices.data()[vertex_cursor + i].normal = normal;
    }

    // 0 1 2
    // 0 2 3
    render_data.elements.data()[element_index_cursor + 0] = element_cursor + 0;
    render_data.elements.data()[element_index_cursor + 1] = element_cursor + 1;
    render_data.elements.data()[element_index_cursor + 2] = element_cursor + 2;

    render_data.elements.data()[element_index_cursor + 3] = element_cursor + 0;
    render_data.elements.data()[element_index_cursor + 4] = element_cursor + 2;
    render_data.elements.data()[element_index_cursor + 5] = element_cursor + 3;
  }


  void Chunk::UploadToGpu()
  {
    vao_.Bind();  // VAO begin
    {
      // VBO
      vbo_.BindAndSetData(data.vertex_size_bytes, data.vertices.data(), GL_STATIC_DRAW);

      // EBO
      ebo_.BindAndSetData(data.element_size_bytes, data.elements.data(), GL_STATIC_DRAW);

      //
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(heh::Vertex), (void*)0);
      glEnableVertexAttribArray(0);

      // texture coords
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(heh::Vertex), (void*)(offsetof(heh::Vertex, tex_coords)));
      glEnableVertexAttribArray(1);

      // normals
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(heh::Vertex), (void*)(offsetof(heh::Vertex, normal)));
      glEnableVertexAttribArray(2);

      //vbo_.Unbind();
    }
    //vao_.Unbind(); // VAO end
  }

  void Chunk::ClearCpuData()
  {
    data.vertices.clear();
    data.vertices.shrink_to_fit();
    data.elements.clear();
    data.elements.shrink_to_fit();
  }

  void Chunk::Render()
  {
    vao_.Bind();
    glDrawElements(GL_TRIANGLES, data.num_elements, GL_UNSIGNED_INT, nullptr);
  }

  static std::string GetFormattedChunkFilename(const std::string& world_filename, const glm::ivec2& chunk_position)
  {
    std::stringstream ss;
    ss << world_filename << "/chunk_" 
       << chunk_position.x << "_" << chunk_position.y << ".chunk";
    return ss.str();
  }

  void Chunk::Serialize(const std::string& world_filename) {
    std::filesystem::create_directories(world_filename);
    std::ofstream file(
        GetFormattedChunkFilename(world_filename, chunk_position), 
        std::ios::out | std::ios::binary);

    file.write(reinterpret_cast<const char*>(&chunk_position), sizeof(glm::ivec2));
    file.write(reinterpret_cast<const char*>(blocks_data.data()), blocks_data.size() * sizeof(Block));
  }

  void Chunk::Deserialize(const std::string& world_filename, uint32_t x, uint32_t y) {
    std::ifstream file(
        GetFormattedChunkFilename(world_filename, {x, y}),
        std::ios::in | std::ios::binary);

    file.read(reinterpret_cast<char*>(&chunk_position), sizeof(glm::ivec2));
    blocks_data.resize(kChunkWidth * kChunkHeight * kChunkDepth);
    file.read(reinterpret_cast<char*>(blocks_data.data()), blocks_data.size() * sizeof(Block));
    blocks_data.shrink_to_fit();
  }

}  // namespace heh