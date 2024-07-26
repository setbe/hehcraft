#pragma once

#include "core/device.hpp"

// Vulkan headers
#include <vulkan/vulkan.h>

// Standard library headers
#include <memory>
#include <string>
#include <vector>

namespace heh {

class SwapChain {
 public:
  static constexpr int kMaxFramesInFlight = 2;

  SwapChain(Device& device, VkExtent2D window_extent);
  SwapChain(
      Device& device, VkExtent2D window_extent, std::shared_ptr<SwapChain> previous);

  ~SwapChain();

  SwapChain(const SwapChain&) = delete;
  SwapChain& operator=(const SwapChain&) = delete;

  VkFramebuffer GetFrameBuffer(int index) const { return swap_chain_framebuffers_[index]; }
  VkRenderPass GetRenderPass() const { return render_pass_; }
  VkImageView GetImageView(int index) const { return swap_chain_image_views_[index]; }
  size_t GetImageCount() const { return swap_chain_images_.size(); }
  VkFormat GetSwapChainImageFormat() const { return swap_chain_image_format_; }
  VkExtent2D GetSwapChainExtent() const { return swap_chain_extent_; }
  uint32_t GetWidth() const { return swap_chain_extent_.width; }
  uint32_t GetHeight() const { return swap_chain_extent_.height; }

  float GetExtentAspectRatio() const {
    return static_cast<float>(swap_chain_extent_.width) / static_cast<float>(swap_chain_extent_.height);
  }

  VkFormat FindDepthFormat() const;

  VkResult AcquireNextImage(uint32_t* image_index);
  VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* image_index);

  bool CompareSwapFormats(const SwapChain& other) const {
    return swap_chain_depth_format_ == other.swap_chain_depth_format_ &&
           swap_chain_image_format_ == other.swap_chain_image_format_;
  }

 private:
  void Init();
  void CreateSwapChain();
  void CreateImageViews();
  void CreateDepthResources();
  void CreateRenderPass();
  void CreateFramebuffers();
  void CreateSyncObjects();

  // Helper functions
  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& available_formats) const;
  VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR>& available_present_modes) const;
  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

  VkFormat swap_chain_image_format_;
  VkFormat swap_chain_depth_format_;
  VkExtent2D swap_chain_extent_;

  std::vector<VkFramebuffer> swap_chain_framebuffers_;
  VkRenderPass render_pass_;

  std::vector<VkImage> depth_images_;
  std::vector<VkDeviceMemory> depth_image_memories_;
  std::vector<VkImageView> depth_image_views_;
  std::vector<VkImage> swap_chain_images_;
  std::vector<VkImageView> swap_chain_image_views_;

  Device& device_;
  VkExtent2D window_extent_;

  VkSwapchainKHR swap_chain_;
  std::shared_ptr<SwapChain> old_swap_chain_;

  std::vector<VkSemaphore> image_available_semaphores_;
  std::vector<VkSemaphore> render_finished_semaphores_;
  std::vector<VkFence> in_flight_fences_;
  std::vector<VkFence> images_in_flight_;
  size_t current_frame_ = 0;
};

}  // namespace heh
