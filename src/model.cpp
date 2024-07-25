#include "model.hpp"

#include "utils.hpp"

// std
#include <cassert>
#include <cstring>
#include <unordered_map>

// tinyobjloader
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace std {
template <>
struct hash<heh::Model::Vertex> {
  size_t operator()(heh::Model::Vertex const &vertex) const 
  {
    size_t seed = 0;
    heh::hash_combine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
    return seed;
  }
};
}  // namespace std

namespace heh {

Model::Model(Device &device, const Model::Builder &builder) 
  : device_{device} 
{
  CreateVertexBuffers(builder.vertices);
  CreateIndexBuffers(builder.indices);
}

Model::~Model() {}

std::unique_ptr<Model> Model::CreateModelFromFile(
  Device &device, const std::string &filename)
{
  Builder builder{};
  builder.LoadModel(filename);
  return std::make_unique<Model>(device, builder);
}

void Model::CreateVertexBuffers(const std::vector<Vertex> &vertices) 
{
  vertex_count_ = static_cast<uint32_t>(vertices.size());
  assert(vertex_count_ >= 3 && "Vertex count must be at least 3");

  VkDeviceSize buffer_size = sizeof(vertices[0]) * vertex_count_;
  uint32_t vertex_size = sizeof(vertices[0]);

  Buffer staging_buffer{ 
    device_, 
    vertex_size, 
    vertex_count_, 
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
  };

  staging_buffer.Map();
  staging_buffer.WriteToBuffer((void *)vertices.data());

  vertex_buffer_ = std::make_unique<Buffer>(
      device_,
      vertex_size,
      vertex_count_,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

  device_.CopyBuffer(staging_buffer.GetBuffer(), vertex_buffer_->GetBuffer(), buffer_size);
}

void Model::CreateIndexBuffers(const std::vector<uint32_t> &indices) 
{
  index_count_ = static_cast<uint32_t>(indices.size());
  has_index_buffer_ = index_count_ > 0;

  if (!has_index_buffer_) return;

  VkDeviceSize buffer_size = sizeof(indices[0]) * index_count_;

  uint32_t index_size = sizeof(indices[0]);

  Buffer stagingBuffer{
      device_,
      index_size,
      index_count_,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  };

  stagingBuffer.Map();
  stagingBuffer.WriteToBuffer((void *)indices.data());

  index_buffer_ = std::make_unique<Buffer>(
      device_,
      index_size,
      index_count_,
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );

  device_.CopyBuffer(stagingBuffer.GetBuffer(), index_buffer_->GetBuffer(), buffer_size);
}

void Model::Draw(VkCommandBuffer command_buffer) 
{
  if (has_index_buffer_) {
    vkCmdDrawIndexed(command_buffer, index_count_, 1, 0, 0, 0);
  } else {
    vkCmdDraw(command_buffer, vertex_count_, 1, 0, 0);
  }
}

void Model::Bind(VkCommandBuffer command_buffer) 
{
  VkBuffer buffers[] = {vertex_buffer_->GetBuffer()};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);

  if (has_index_buffer_)
    vkCmdBindIndexBuffer(command_buffer, index_buffer_->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescriptions() 
{
  std::vector<VkVertexInputBindingDescription> binding_descriptions(1);
  binding_descriptions[0].binding = 0;
  binding_descriptions[0].stride = sizeof(Vertex);
  binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return binding_descriptions;
}

std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions() 
{
  std::vector<VkVertexInputAttributeDescription> attribute_descriptions{};

  attribute_descriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
  attribute_descriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
  attribute_descriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
  attribute_descriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});

  return attribute_descriptions;
}

void Model::Builder::LoadModel(const std::string &filename)
{
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str())) 
    throw std::runtime_error(warn + err);

  vertices.clear();
  indices.clear();

  std::unordered_map<Vertex, uint32_t> unique_vertices{};

  for (const auto &shape : shapes) 
  {
    for (const auto &index : shape.mesh.indices) 
    {
      Vertex vertex{};

      if (index.vertex_index >= 0) 
      {
        vertex.position = {
          attrib.vertices[3 * index.vertex_index + 0],
          attrib.vertices[3 * index.vertex_index + 1],
          attrib.vertices[3 * index.vertex_index + 2]};
        vertex.color = {
          attrib.colors[3 * index.vertex_index + 0],
          attrib.colors[3 * index.vertex_index + 1],
          attrib.colors[3 * index.vertex_index + 2]};
      }

      if (index.normal_index >= 0) {
        vertex.normal = {
          attrib.normals[3 * index.normal_index + 0],
          attrib.normals[3 * index.normal_index + 1],
          attrib.normals[3 * index.normal_index + 2]};
      }
      if (index.texcoord_index >= 0) {
        vertex.uv = {
          attrib.texcoords[2 * index.texcoord_index + 0],
          attrib.texcoords[2 * index.texcoord_index + 1]};
      }

      if (unique_vertices.count(vertex) == 0) {
        unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }

      indices.push_back(unique_vertices[vertex]);
    }
  }
}

}  // namespace heh