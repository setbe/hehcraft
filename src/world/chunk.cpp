#include "world/chunk.hpp"


#include "simplexnoise/src/SimplexNoise.h"

// std
#include <array>
#include <vector>
#include <cstdint>
#include <random>

#include <filesystem>
#include <fstream>
#include <sstream>

#include <iostream>

namespace heh {


  /*template<typename T, uint8_t N_Vec, uint8_t N_Array>
  struct VecOrder
  {
    std::array<glm::vec<N_Vec, T>, N_Array> vec;
    std::array<uint8_t, N_Array> order;
  };

  using Position3Order8 = VecOrder<float, 3, 8>;
  using UvOrder4 = VecOrder<float, 2, 4>;*/

  static struct alignas(4) VertexOrder {
    uint8_t order;
    uint8_t uv;
    uint8_t normal;
    uint8_t padding; // padding
  };

  static const std::array<VertexOrder, 24> kVertexOrder{ {
      // top
      {0, 0, 0, 0}, {1, 1, 1, 0}, {2, 2, 0, 0}, {3, 3, 0, 0},
      // front
      {1, 3, 0, 0}, {5, 2, 0, 0}, {6, 1, 1, 0}, {2, 0, 0, 0},
      // bottom
      {7, 7, 0, 0}, {6, 6, -1, 0}, {5, 5, 0, 0}, {4, 4, 0, 0},
      // right
      {0, 0, 1, 0}, {4, 1, 0, 0}, {5, 2, 0, 0}, {1, 3, 0, 0},
      // left
      {2, 0, -1, 0}, {6, 1, 0, 0}, {7, 2, 0, 0}, {3, 3, 0, 0},
      // back
      {3, 3, 0, 0}, {7, 2, 0, 0}, {4, 1, -1, 0}, {0, 0, 0, 0}
  } };

  /*
  static const std::array<uint8_t, 24> kOrder {
    0, 1, 2, 3, // top
    1, 5, 6, 2, // front
    7, 6, 5, 4, // bottom
    0, 4, 5, 1, // right
    2, 6, 7, 3, // left
    3, 7, 4, 0  // back
  };

  static const std::array<uint8_t, 24> kUvOrder {
    0, 1, 2, 3, // top
    3, 2, 1, 0, // front
    7, 6, 5, 4, // bottom
    0, 1, 2, 3, // right
    0, 1, 2, 3, // left
    3, 2, 1, 0, // back
  };

  static const std::array<uint8_t, 18> kFaceNormals{
    0, 1, 0,  // top
    0, 0, 1,  // front
    0, -1, 0, // bottom
    1, 0, 0,  // right
    -1, 0, 0, // left
    0, 0, -1  // back
  };
  */

  
  static constexpr glm::ivec3 GetFaceNormal(Block::Face face) {
    switch (face)
    {
      case heh::Block::Face::Top: return { 0, 1, 0 };
      case heh::Block::Face::Front: return { 0, 0, 1 };
      case heh::Block::Face::Bottom: return { 0, -1, 0 };
      case heh::Block::Face::Right: return { 1, 0, 0 };
      case heh::Block::Face::Left: return { -1, 0, 0 };
      case heh::Block::Face::Back: return { 0, 0, -1 };

      case heh::Block::Face::AllCulled:
        throw std::runtime_error("Cannot get normal from culled face");
      case heh::Block::Face::OnlyTopVisible:
        throw std::runtime_error("Cannot get normal from only top visible face");
      default:
        throw std::runtime_error("Unknown face");
    }
  }

  static void BuildBlockFace(
    ChunkRenderData& render_data,
    const std::array<glm::vec3, 8>& verts,
    const TextureFormat& texture_format,
    Block::Face face,
    int vertex_cursor,
    int element_index_cursor);
  

  void Chunk::Generate(int32_t seed, int chunk_x, int chunk_z) noexcept
  {
    const int world_chunk_x = chunk_x * kChunkWidth;
    const int world_chunk_z = chunk_z * kChunkDepth;
    chunk_position = { chunk_x, chunk_z };

    blocks_data.clear();    
    blocks_data.resize(kChunkWidth * kChunkHeight * kChunkDepth);

    const SimplexNoise generator(
      .005f, // frequency
      80.f,  // amplitude
      2.f,   // lacunarity
      .5f);  // persistence
    

    for (int y = 0; y < kChunkHeight; ++y)
    {
      for (int x = 0; x < kChunkDepth; ++x)
      {
        for (int z = 0; z < kChunkWidth; ++z)
        {
          const int array1d = Block::To1DArrayIndex(x, y, z);

          float height_float = generator.fractal(5, (x + world_chunk_x), (z + world_chunk_z)) * 80.f + 70.f;
          int16_t height = (int16_t)height_float;


          if (y < height - 5)
            blocks_data[array1d].id = 3; // stone
          else if (y < height)
            blocks_data[array1d].id = 1; // grass
          else if (y == height)
            blocks_data[array1d].id = 2; // dirt
        } // for y
      } // for z
    } // for x
  }

  void Chunk::BuildMesh() noexcept
  {
    data.vertices.clear();
    data.elements.clear();

    // TODO: Optimize this
    data.vertices.resize(kChunkWidth * kChunkHeight * kChunkDepth * 24);
    data.elements.resize(kChunkWidth * kChunkHeight * kChunkDepth * 36);

    std::array<glm::vec3, 8> verts;

    const int world_chunk_x = chunk_position.x * 16;
    const int world_chunk_z = chunk_position.y * 16;

    int vertex_cursor = 0;
    int element_index_cursor = 0;

    for (int y = 0; y < kChunkHeight; ++y) 
    {
      for (int x = 0; x < kChunkDepth; ++x)
      {
        for (int z = 0; z < kChunkWidth; ++z)
        {
          const Block& block = Block::At(blocks_data.data(), x, y, z);
          const int block_id = block.id;

          if (block.id == block_map::kNullBlock.id)
            continue;

          const int array_expansion = Block::To1DArrayIndex(x, y, z);

          const BlockFormat& block_format = block_map::GetBlock(block.id);

          const TextureFormat& tex_top = block_map::texture_formats[block_format.top];
          const TextureFormat& tex_side = block_map::texture_formats[block_format.side];
          const TextureFormat& tex_bottom = block_map::texture_formats[block_format.bottom];

          // verts begin
          // position
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
          
           
          // Top face
          const int top_block_id = Block::At(blocks_data.data(), x, y + 1, z).id;
          const BlockFormat& top_block = block_map::GetBlock(top_block_id);
          if (!top_block_id || top_block.is_transparent)
          {
            BuildBlockFace(
              data, verts, tex_top,
              Block::Face::Top,
              vertex_cursor,
              element_index_cursor
            );
            vertex_cursor += 4;
            element_index_cursor += 6;
          }

          // Right face
          const int right_block_id = Block::At(blocks_data.data(), x, y, z + 1).id;
          const BlockFormat& right_block = block_map::GetBlock(right_block_id);
          if (!right_block_id || right_block.is_transparent) {
            BuildBlockFace(
              data, verts, tex_side,
              Block::Face::Right,
              vertex_cursor,
              element_index_cursor
            );
            vertex_cursor += 4;
            element_index_cursor += 6;
          }

          // Forward face
          const int forward_block_id = Block::At(blocks_data.data(), x + 1, y, z).id;
          const BlockFormat& forward_block = block_map::GetBlock(forward_block_id);
          if (!forward_block_id || forward_block.is_transparent) {
            BuildBlockFace(
              data, verts, tex_side,
              Block::Face::Front,
              vertex_cursor,
              element_index_cursor
            );
            vertex_cursor += 4;
            element_index_cursor += 6;
          }

          // Left face
          const int left_block_id = Block::At(blocks_data.data(), x, y, z - 1).id;
          const BlockFormat& left_block = block_map::GetBlock(left_block_id);
          if (!left_block_id || left_block.is_transparent) {
            BuildBlockFace(
              data, verts, tex_side,
              Block::Face::Left,
              vertex_cursor,
              element_index_cursor
            );
            vertex_cursor += 4;
            element_index_cursor += 6;
          }

          // Back face
          const int back_block_id = Block::At(blocks_data.data(), x - 1, y, z).id;
          const BlockFormat& back_block = block_map::GetBlock(back_block_id);
          if (!back_block_id || back_block.is_transparent) {
            BuildBlockFace( 
              data, verts, tex_side,
              Block::Face::Back,
              vertex_cursor,
              element_index_cursor
            );
            vertex_cursor += 4;
            element_index_cursor += 6;
          }

          // Bottom face
          const int bottom_block_id = Block::At(blocks_data.data(), x, y - 1, z).id;
          const BlockFormat& bottom_block = block_map::GetBlock(bottom_block_id);
          if (!bottom_block_id || bottom_block.is_transparent)
          {
            // Bottom face
            BuildBlockFace(
              data, verts, tex_bottom,
              Block::Face::Bottom,
              vertex_cursor,
              element_index_cursor
            );
            vertex_cursor += 4;
            element_index_cursor += 6;
          }

        } // for z
      } // for y
    } // for x

    std::cout << "Vertex size (before): " << data.vertices.size() << std::endl;
    std::cout << "vertex cursor: " << vertex_cursor << std::endl;
    std::cout << "Element size (before): " << data.elements.size() << std::endl;
    std::cout << "element index: " << element_index_cursor << std::endl;
    
    data.vertices.resize(vertex_cursor);
    data.elements.resize(vertex_cursor * 2);

    std::cout << "Vertex size (after): " << data.vertices.size() << std::endl;
    std::cout << "Element size (after): " << data.elements.size() << std::endl;

    std::cout << "------------------------------------\n";

    

    // Grab calculated data into a struct
    data.vertex_size_bytes = data.vertices.size() * sizeof(decltype(data.vertices)::value_type);
    data.element_size_bytes = data.elements.size() * sizeof(decltype(data.elements)::value_type);
    data.num_elements = static_cast<uint32_t>(data.elements.size());

    UploadToGpu();
    ClearCpuData();
  }


  static void BuildBlockFace(
    ChunkRenderData& render_data,
    const std::array<glm::vec3, 8>& verts,
    const TextureFormat& texture_format,
    Block::Face face,
    int vertex_cursor,
    int element_index_cursor)
  {
    uint8_t face_index = static_cast<uint8_t>(face);
    for (int i = 0; i < 4; ++i)
    {
      int index = face_index * 4 + i;

      render_data.vertices.data()[vertex_cursor + i].position = verts[kVertexOrder[index].order];
      render_data.vertices.data()[vertex_cursor + i].tex_coords = texture_format.uvs[kVertexOrder[index].uv];
      render_data.vertices.data()[vertex_cursor + i].normal = GetFaceNormal(face);
    }

    // 0 1 2
    // 0 2 3
    render_data.elements.data()[element_index_cursor + 0] = vertex_cursor + 0;
    render_data.elements.data()[element_index_cursor + 1] = vertex_cursor + 1;
    render_data.elements.data()[element_index_cursor + 2] = vertex_cursor + 2;
                                                            
    render_data.elements.data()[element_index_cursor + 3] = vertex_cursor + 0;
    render_data.elements.data()[element_index_cursor + 4] = vertex_cursor + 2;
    render_data.elements.data()[element_index_cursor + 5] = vertex_cursor + 3;
  }





  // Manupalating buffers and Rendering
  // ---------------------------------

  void Chunk::UploadToGpu() noexcept
  {
    // Upload to GPU
    vao_.Bind();

    // VBO
    vbo_.Bind();
    vbo_.SetData(data.vertex_size_bytes, data.vertices.data(), GL_STATIC_DRAW);

    // EBO
    ebo_.Bind();
    ebo_.SetData(data.element_size_bytes, data.elements.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(heh::Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(heh::Vertex), (void*)(offsetof(heh::Vertex, tex_coords)));
    glEnableVertexAttribArray(1);

    // normals
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(heh::Vertex), (void*)(offsetof(heh::Vertex, normal)));
    glEnableVertexAttribArray(2);
  }

  void Chunk::ClearCpuData() noexcept
  {
    data.vertices.clear();
    data.vertices.shrink_to_fit();
    data.elements.clear();
    data.elements.shrink_to_fit();
  }

  void Chunk::Render() noexcept
  {
    vao_.Bind();
    glDrawElements(GL_TRIANGLES, data.num_elements, GL_UNSIGNED_INT, nullptr);
  }



  // Serialization and Deserialization
  // ---------------------------------

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