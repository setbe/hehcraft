#pragma once

#include "core/device.hpp"
#include "core/buffer.hpp"
#include "core/utils.hpp" // for extracting word from path

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

    /**
     * @brief Load a model from a file
     * @param base_dir The base directory of the model
     * @note Only OBJ files are supported
     * @note Object filename must be named as base_dir
     */
    void LoadModel(const std::string &base_dir);
  };

  Model(Device &device, const Builder &builder);
  ~Model();

  Model(const Model &) = delete;
  Model &operator=(const Model &) = delete;

  /**
   * @brief Create a model from a file
   * @param device The logical device
   * @param base_dir The base directory of the model
   * @return Unique pointer to the model
   * @note Only OBJ files are supported
   * @note Object filename must be named as base_dir
   */
  static std::unique_ptr<Model> CreateModel(Device &device, const std::string &base_dir);

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
