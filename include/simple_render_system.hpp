#pragma once

#include "camera.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "pipeline.hpp"
#include "frame_info.hpp"

// std
#include <memory>
#include <vector>

namespace heh {

class SimpleRenderSystem {
 public:
  SimpleRenderSystem(Device& device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem&) = delete;
  SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

  void RenderGameObjects(FrameInfo& frame_info);

 private:
  void CreatePipelineLayout(VkDescriptorSetLayout global_set_layout);
  void CreatePipeline(VkRenderPass render_pass);

  Device& device_;

  std::unique_ptr<Pipeline> pipeline_;
  VkPipelineLayout pipeline_layout_;
};

}  // namespace heh
