#pragma once

#include "core/device.hpp"
#include "core/buffer.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>

namespace heh {

class Model {
 public:
  struct Vertex {
    glm::vec3 position{};
    glm::vec3 color{};
    glm::vec3 normal{};
    glm::vec2 uv{};

    static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

    bool operator==(const Vertex &other) const {
      return position == other.position &&
             color == other.color &&
             normal == other.normal &&
             uv == other.uv;
    }
  };

  struct Builder {
    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};

    void LoadModel(const std::string &filename);
  };

  Model(Device &device, const Builder &builder);
  ~Model();

  Model(const Model &) = delete;
  Model &operator=(const Model &) = delete;

  static std::unique_ptr<Model> CreateModelFromFile(Device &device, const std::string &filename);

  void Bind(VkCommandBuffer command_buffer);
  void Draw(VkCommandBuffer command_buffer);

 private:
  void CreateVertexBuffers(const std::vector<Vertex> &vertices);
  void CreateIndexBuffers(const std::vector<uint32_t> &indices);

  Device &device_;

  std::unique_ptr<Buffer> vertex_buffer_;
  uint32_t vertex_count_{};

  bool has_index_buffer_ = false;
  std::unique_ptr<Buffer> index_buffer_;
  uint32_t index_count_{};
};

}  // namespace heh
