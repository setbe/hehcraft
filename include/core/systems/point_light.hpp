#pragma once

#include "core/camera.hpp"
#include "core/device.hpp"
#include "core/game_object.hpp"
#include "core/pipeline.hpp"
#include "core/frame_info.hpp"

// std
#include <memory>
#include <vector>

namespace heh {

struct PointLightPushConstant {
  glm::vec4 position{};
  glm::vec4 color{};
  float radius;
};

class PointLightSystem {
 public:
  PointLightSystem(Device& device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout);
  ~PointLightSystem();

  PointLightSystem(const PointLightSystem&) = delete;
  PointLightSystem& operator=(const PointLightSystem&) = delete;

  void Update(FrameInfo& frame_info, GlobalUbo& ubo);
  void Render(FrameInfo& frame_info);

 private:
  void CreatePipelineLayout(VkDescriptorSetLayout global_set_layout);
  void CreatePipeline(VkRenderPass render_pass);

  Device& device_;

  std::unique_ptr<Pipeline> pipeline_;
  VkPipelineLayout pipeline_layout_;
};

}  // namespace heh
