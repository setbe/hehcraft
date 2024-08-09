#include "world/chunk.hpp"

// std
#include <vector>
#include <limits>
#include <array>
#include <type_traits>
#include <cstdint>
#include <random>

namespace heh {

  

  void Chunk::Generate()
  {
    data = std::make_unique<ChunkRenderData>();
    data->vertices.resize(kChunkWidth * kChunkHeight * kChunkDepth * 24);
    data->elements.resize(kChunkWidth * kChunkHeight * kChunkDepth * 36);
    blocks_data.resize(kChunkWidth * kChunkHeight * kChunkDepth);

    

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
          const int array_expansion = (x * (kChunkDepth * kChunkHeight) + (y + kChunkHeight * z));


          // data->elements
          const uint32_t element_index = array_expansion * 36;
          const uint32_t element_offset = array_expansion * 24;
          if (element_offset >= std::numeric_limits<uint32_t>::max() - 24)
            printf("Chunk Overflow\n");

          // 0 1 2
          // 0 2 3
          for (int i = 0; i < 6; ++i)
          {
            data->elements[element_index + (i * 6) + 0] = element_offset + 0 + (i * 4);
            data->elements[element_index + (i * 6) + 1] = element_offset + 1 + (i * 4);
            data->elements[element_index + (i * 6) + 2] = element_offset + 2 + (i * 4);

            data->elements[element_index + (i * 6) + 3] = element_offset + 0 + (i * 4);
            data->elements[element_index + (i * 6) + 4] = element_offset + 2 + (i * 4);
            data->elements[element_index + (i * 6) + 5] = element_offset + 3 + (i * 4);
          }


          // tex_coords


          const int vertex_offset = array_expansion * 24;
          const int tex_coords_index = array_expansion * 24;


          const std::string& name = block_map::id_to_name[blocks_data[array_expansion]];
          const BlockFormat& block = block_map::block_formats[blocks_data[array_expansion] - 1];

          const TextureFormat& tex_top = block_map::texture_formats[block.top];
          const TextureFormat& tex_side = block_map::texture_formats[block.side];
          const TextureFormat& tex_bottom = block_map::texture_formats[block.bottom];

          // verts begin

          // position
          std::array<Vertex, 8> verts;
          verts[0].position = glm::vec3(
            (float)x - 0.5f,
            (float)y + 0.5f,
            (float)z + 0.5f
          );
          verts[1].position = verts[0].position + glm::vec3(1.f, 0.f, 0.f);
          verts[2].position = verts[1].position - glm::vec3(0.f, 0.f, 1.f);
          verts[3].position = verts[0].position - glm::vec3(0.f, 0.f, 1.f);

          verts[4].position = verts[0].position - glm::vec3(0.f, 1.f, 0.f);
          verts[5].position = verts[1].position - glm::vec3(0.f, 1.f, 0.f);
          verts[6].position = verts[2].position - glm::vec3(0.f, 1.f, 0.f);
          verts[7].position = verts[3].position - glm::vec3(0.f, 1.f, 0.f);


          // verts end
          
          // Top face
          data->vertices[vertex_offset + 0] = { verts[0].position, tex_top.uvs[0], glm::vec3(0.f, 1.f, 0.f) };
          data->vertices[vertex_offset + 1] = { verts[1].position, tex_top.uvs[1], glm::vec3(0.f, 1.f, 0.f) };
          data->vertices[vertex_offset + 2] = { verts[2].position, tex_top.uvs[2], glm::vec3(0.f, 1.f, 0.f) };
          data->vertices[vertex_offset + 3] = { verts[3].position, tex_top.uvs[3], glm::vec3(0.f, 1.f, 0.f) };

          // Right face
          data->vertices[vertex_offset + 4] = { verts[0].position, tex_side.uvs[0], glm::vec3(1.f, 0.f, 0.f) };
          data->vertices[vertex_offset + 5] = { verts[4].position, tex_side.uvs[1], glm::vec3(1.f, 0.f, 0.f) };
          data->vertices[vertex_offset + 6] = { verts[5].position, tex_side.uvs[2], glm::vec3(1.f, 0.f, 0.f) };
          data->vertices[vertex_offset + 7] = { verts[1].position, tex_side.uvs[3], glm::vec3(1.f, 0.f, 0.f) };

          // Forward face
          data->vertices[vertex_offset + 8] = { verts[1].position, tex_side.uvs[3], glm::vec3(0.f, 0.f, 1.f) };
          data->vertices[vertex_offset + 9] = { verts[5].position, tex_side.uvs[2], glm::vec3(0.f, 0.f, 1.f) };
          data->vertices[vertex_offset + 10] = { verts[6].position, tex_side.uvs[1], glm::vec3(0.f, 0.f, 1.f) };
          data->vertices[vertex_offset + 11] = { verts[2].position, tex_side.uvs[0], glm::vec3(0.f, 0.f, 1.f) };

          // Left face
          data->vertices[vertex_offset + 12] = { verts[2].position, tex_side.uvs[0], glm::vec3(-1.f, 0.f, 0.f) };
          data->vertices[vertex_offset + 13] = { verts[6].position, tex_side.uvs[1], glm::vec3(-1.f, 0.f, 0.f) };
          data->vertices[vertex_offset + 14] = { verts[7].position, tex_side.uvs[2], glm::vec3(-1.f, 0.f, 0.f) };
          data->vertices[vertex_offset + 15] = { verts[3].position, tex_side.uvs[3], glm::vec3(-1.f, 0.f, 0.f) };

          // Back face
          data->vertices[vertex_offset + 16] = { verts[3].position, tex_side.uvs[3], glm::vec3(0.f, 0.f, -1.f) };
          data->vertices[vertex_offset + 17] = { verts[7].position, tex_side.uvs[2], glm::vec3(0.f, 0.f, -1.f) };
          data->vertices[vertex_offset + 18] = { verts[4].position, tex_side.uvs[1], glm::vec3(0.f, 0.f, -1.f) };
          data->vertices[vertex_offset + 19] = { verts[0].position, tex_side.uvs[0], glm::vec3(0.f, 0.f, -1.f) };

          // Bottom face
          data->vertices[vertex_offset + 20] = { verts[7].position, tex_bottom.uvs[1], glm::vec3(0.f, -1.f, 0.f) };
          data->vertices[vertex_offset + 21] = { verts[6].position, tex_bottom.uvs[0], glm::vec3(0.f, -1.f, 0.f) };
          data->vertices[vertex_offset + 22] = { verts[5].position, tex_bottom.uvs[3], glm::vec3(0.f, -1.f, 0.f) };
          data->vertices[vertex_offset + 23] = { verts[4].position, tex_bottom.uvs[2], glm::vec3(0.f, -1.f, 0.f) };

        } // for z
      } // for y
    } // for x


    // Grab calculated data into a struct
    data->vertex_size_bytes = data->vertices.size() * sizeof(Vertex);
    data->element_size_bytes = data->elements.size() * sizeof(int32_t);
    data->num_elements = static_cast<uint32_t>(data->elements.size());
  }




  void Chunk::UploadToGpu()
  {
    vao_.Bind();  // VAO begin
    {
      // VBO
      vbo_.BindAndSetData(data->vertex_size_bytes, data->vertices.data(), GL_STATIC_DRAW);

      // EBO
      ebo_.BindAndSetData(data->element_size_bytes, data->elements.data(), GL_STATIC_DRAW);

      // position
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