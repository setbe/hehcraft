#include "world/chunk.hpp"

// std
#include <vector>
#include <limits>
#include <array>
#include <type_traits>
#include <cstdint>
#include <random>

namespace heh {
  static int To1DArrayIndex(int x, int y, int z)
  {
    int index = x * (kChunkDepth * kChunkHeight) + (y + kChunkHeight * z);
    return index > kChunkWidth * kChunkHeight * kChunkDepth || index < 0 ?
      block_map::kNullBlock :
      index;
  }

  template<typename T, uint8_t N_Vec, uint8_t N = 4>
  struct VecOrder
  {
    std::array<glm::vec<N_Vec, T>, N> vec;
    std::array<uint8_t, N> order;
  };

  static void LoadBlock(
    Vertex* vert_data,
    const VecOrder<float, 3, 8>& verts,
    const VecOrder<float, 2, 4>& uv,
    const glm::vec3& normal,
    int vertex_cursor);
  

  void Chunk::Generate()
  {
    data = std::make_unique<ChunkRenderData>();
    data->vertices.resize(kChunkWidth * kChunkHeight * kChunkDepth * 24);
    data->elements.resize(kChunkWidth * kChunkHeight * kChunkDepth * 36);
    blocks_data.resize(kChunkWidth * kChunkHeight * kChunkDepth);

    int vertex_cursor = 0;
    int element_cursor = 0;
    int element_index_cursor = 0;

    for (int i = 0; i < kChunkWidth * kChunkHeight * kChunkDepth; ++i)
    {
      blocks_data[i] = 1;
    }

    for (int x = 0; x < kChunkWidth; ++x) 
    {
      for (int z = 0; z < kChunkDepth; ++z)
      {
        for (int y = 0; y < kChunkHeight; ++y)
        {
          const int array_expansion = To1DArrayIndex(x, y, z);


          // data->elements
          //const uint32_t element_index = array_expansion * 36;
          //const uint32_t element_offset = array_expansion * 24;
          if (element_cursor >= std::numeric_limits<uint32_t>::max() - 24)
            printf("Chunk Overflow\n");

          // 0 1 2
          // 0 2 3
          for (int i = 0; i < 6; ++i)
          {
            data->elements[element_index_cursor + (i * 6) + 0] = element_cursor + 0 + (i * 4);
            data->elements[element_index_cursor + (i * 6) + 1] = element_cursor + 1 + (i * 4);
            data->elements[element_index_cursor + (i * 6) + 2] = element_cursor + 2 + (i * 4);

            data->elements[element_index_cursor + (i * 6) + 3] = element_cursor + 0 + (i * 4);
            data->elements[element_index_cursor + (i * 6) + 4] = element_cursor + 2 + (i * 4);
            data->elements[element_index_cursor + (i * 6) + 5] = element_cursor + 3 + (i * 4);
          }

          element_cursor += 4 * 6;
          element_index_cursor += 36;


          // tex_coords


          //const int vertex_cursor = array_expansion * 24;
          //const int tex_coords_index = array_expansion * 24;


          const BlockFormat& block = block_map::GetBlock(blocks_data[array_expansion]);

          const TextureFormat& tex_top = block_map::texture_formats[block.top];
          const TextureFormat& tex_side = block_map::texture_formats[block.side];
          const TextureFormat& tex_bottom = block_map::texture_formats[block.bottom];

          // verts begin

          //
          std::array<glm::vec3, 8> verts;
          verts[0] = glm::vec3(
            (float)x - 0.5f,
            (float)y + 0.5f,
            (float)z + 0.5f
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
          /*const int top_index = To1DArrayIndex(x, y + 1, z);
          const BlockFormat& top_block = block_map::block_formats[blocks_data[top_index]];
          if (!top_index || top_block.is_transparent)
          {*/
          using VertsOrder = VecOrder<float, 3, 8>;
          using UvOrder = VecOrder<float, 2, 4>;

          LoadBlock(
            data->vertices.data(),
            VertsOrder{verts, { 0, 1, 2, 3 }},
            UvOrder{tex_top.uvs, { 0, 1, 2, 3 }},
            { 0.f, 1.f, 0.f },
            vertex_cursor
           );
          vertex_cursor += 4;
          //}

          // Right face
          LoadBlock(
            data->vertices.data(),
            VertsOrder{verts, { 0, 4, 5, 1 }},
            UvOrder{tex_side.uvs, { 0, 1, 2, 3 }},
            { 1.f, 0.f, 0.f },
            vertex_cursor
          );
          vertex_cursor += 4;

          // Forward face
          LoadBlock(
            data->vertices.data(),
            VertsOrder{verts, { 1, 5, 6, 2 }},
            UvOrder{tex_side.uvs, { 3, 2, 1, 0 }},
            { 0.f, 0.f, 1.f },
            vertex_cursor
          );
          vertex_cursor += 4;

          // Left face
          LoadBlock(
            data->vertices.data(),
            VertsOrder{verts, { 2, 6, 7, 3 }},
            UvOrder{tex_side.uvs, { 0, 1, 2, 3 }},
            { -1.f, 0.f, 0.f },
            vertex_cursor
          );
          vertex_cursor += 4;

          // Back face
          LoadBlock(
            data->vertices.data(),
            VertsOrder{verts, { 3, 7, 4, 0 }},
            UvOrder{tex_side.uvs, { 3, 2, 1, 0 }},
            { 0.f, 0.f, -1.f },
            vertex_cursor
          );
          vertex_cursor += 4;

          // Bottom face
          LoadBlock(
            data->vertices.data(),
            VertsOrder{verts, { 7, 6, 5, 4 }},
            UvOrder{tex_bottom.uvs, { 1, 0, 3, 2 }},
            { 0.f, -1.f, 0.f },
            vertex_cursor
          );
          vertex_cursor += 4;

        } // for z
      } // for y
    } // for x


    // Grab calculated data into a struct
    data->vertex_size_bytes = data->vertices.size() * sizeof(Vertex);
    data->element_size_bytes = data->elements.size() * sizeof(int32_t);
    data->num_elements = static_cast<uint32_t>(data->elements.size());
  }


  static void LoadBlock(
    Vertex* vert_data,
    const VecOrder<float, 3, 8>& verts,
    const VecOrder<float, 2, 4>& uv,
    const glm::vec3& normal,
    int vertex_cursor)
  {
    for (int i = 0; i < 4; ++i)
    {
      vert_data[vertex_cursor + i].position = verts.vec[verts.order[i]];
      vert_data[vertex_cursor + i].tex_coords = uv.vec[uv.order[i]];
      vert_data[vertex_cursor + i].normal = normal;
    }
  }




  void Chunk::UploadToGpu()
  {
    vao_.Bind();  // VAO begin
    {
      // VBO
      vbo_.BindAndSetData(data->vertex_size_bytes, data->vertices.data(), GL_STATIC_DRAW);

      // EBO
      ebo_.BindAndSetData(data->element_size_bytes, data->elements.data(), GL_STATIC_DRAW);

      //
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(heh::Vertex), (void*)0);
      glEnableVertexAttribArray(0);

      // texture coords
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(heh::Vertex), (void*)(offsetof(heh::Vertex, tex_coords)));
      glEnableVertexAttribArray(1);

      // normals
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(heh::Vertex), (void*)(offsetof(heh::Vertex, normal)));
      glEnableVertexAttribArray(2);

      vbo_.Unbind();
    }
    vao_.Unbind(); // VAO end
  }

  void Chunk::ClearCpuData()
  {
    data->vertices.clear();
    data->vertices.shrink_to_fit();
    data->elements.clear();
    data->elements.shrink_to_fit();
  }

  void Chunk::Render()
  {
    vao_.Bind();
    glDrawElements(GL_TRIANGLES, data->num_elements, GL_UNSIGNED_INT, nullptr);
  }

}  // namespace heh