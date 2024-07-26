#pragma once

#include "core/device.hpp"
#include "core/swap_chain.hpp"
#include "core/window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace heh {

class Renderer {
 public:
  Renderer(Window& window, Device& device);
  ~Renderer();

  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;

  VkRenderPass GetSwapChainRenderPass() const { return swap_chain_->GetRenderPass(); }
  float GetAspectRatio() const { return swap_chain_->GetExtentAspectRatio(); }
  bool IsFrameInProgress() const { return is_frame_started_; }

  VkCommandBuffer GetCurrentCommandBuffer() const {
    assert(is_frame_started_ && "Cannot get command buffer when frame not in progress");
    return command_buffers_[current_frame_index_];
  }

  int GetFrameIndex() const {
    assert(is_frame_started_ && "Cannot get frame index when frame not in progress");
    return current_frame_index_;
  }

  VkCommandBuffer BeginFrame();
  void EndFrame();
  void BeginSwapChainRenderPass(VkCommandBuffer command_buffer);
  void EndSwapChainRenderPass(VkCommandBuffer command_buffer);

 private:
  void CreateCommandBuffers();
  void FreeCommandBuffers();
  void RecreateSwapChain();

  Window& window_;
  Device& device_;
  std::unique_ptr<SwapChain> swap_chain_;
  std::vector<VkCommandBuffer> command_buffers_;

  uint32_t current_image_index_;
  int current_frame_index_{0};
  bool is_frame_started_{false};
};

}  // namespace heh
