#include "swap_chain.hpp"

// std
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

namespace heh {

SwapChain::SwapChain(
  Device &device, VkExtent2D extent)
    : device_{device}, window_extent_{extent} 
{
  Init();
}

SwapChain::SwapChain(
    Device &device, VkExtent2D extent, std::shared_ptr<SwapChain> previous)
    : device_{device}, window_extent_{extent}, old_swap_chain_{previous} 
{
  Init();
  old_swap_chain_ = nullptr;
}

void SwapChain::Init() 
{
  CreateSwapChain();
  CreateImageViews();
  CreateRenderPass();
  CreateDepthResources();
  CreateFramebuffers();
  CreateSyncObjects();
}

SwapChain::~SwapChain() 
{
  for (auto image_view : swap_chain_image_views_) {
    vkDestroyImageView(device_.GetDevice(), image_view, nullptr);
  }
  swap_chain_image_views_.clear();

  if (swap_chain_ != nullptr) {
    vkDestroySwapchainKHR(device_.GetDevice(), swap_chain_, nullptr);
    swap_chain_ = nullptr;
  }

  for (int i = 0; i < depth_images_.size(); i++) {
    vkDestroyImageView(device_.GetDevice(), depth_image_views_[i], nullptr);
    vkDestroyImage(device_.GetDevice(), depth_images_[i], nullptr);
    vkFreeMemory(device_.GetDevice(), depth_image_memories_[i], nullptr);
  }

  for (auto framebuffer : swap_chain_framebuffers_) {
    vkDestroyFramebuffer(device_.GetDevice(), framebuffer, nullptr);
  }
  vkDestroyRenderPass(device_.GetDevice(), render_pass_, nullptr);

  // cleanup synchronization objects
  for (size_t i = 0; i < kMaxFramesInFlight; i++) {
    vkDestroySemaphore(device_.GetDevice(), render_finished_semaphores_[i], nullptr);
    vkDestroySemaphore(device_.GetDevice(), image_available_semaphores_[i], nullptr);
    vkDestroyFence(device_.GetDevice(), in_flight_fences_[i], nullptr);
  }
}

VkResult SwapChain::AcquireNextImage(uint32_t *image_index) 
{
  vkWaitForFences(
      device_.GetDevice(),
      1,
      &in_flight_fences_[current_frame_],
      VK_TRUE,
      std::numeric_limits<uint64_t>::max());

  VkResult result = vkAcquireNextImageKHR(
      device_.GetDevice(),
      swap_chain_,
      std::numeric_limits<uint64_t>::max(),
      image_available_semaphores_[current_frame_],  // must be a not signaled semaphore
      VK_NULL_HANDLE,
      image_index);

  return result;
}

VkResult SwapChain::SubmitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *image_index) 
{
  if (images_in_flight_[*image_index] != VK_NULL_HANDLE) {
    vkWaitForFences(device_.GetDevice(), 1, &images_in_flight_[*image_index], VK_TRUE, UINT64_MAX);
  }
  images_in_flight_[*image_index] = in_flight_fences_[current_frame_];

  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore wait_semaphores[] = {image_available_semaphores_[current_frame_]};
  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  submit_info.pWaitDstStageMask = wait_stages;

  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = buffers;

  VkSemaphore signal_semaphores[] = {render_finished_semaphores_[current_frame_]};
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_semaphores;

  vkResetFences(device_.GetDevice(), 1, &in_flight_fences_[current_frame_]);
  if (
    vkQueueSubmit(
        device_.GetGraphicsQueue(), 
        1, 
        &submit_info, 
        in_flight_fences_[current_frame_]) 
    != VK_SUCCESS) 
  {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signal_semaphores;

  VkSwapchainKHR swapChains[] = {swap_chain_};
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swapChains;

  present_info.pImageIndices = image_index;

  auto result = vkQueuePresentKHR(device_.GetPresentQueue(), &present_info);

  current_frame_ = (current_frame_ + 1) % kMaxFramesInFlight;
  return result;
}

void SwapChain::CreateSwapChain()
{
  SwapChainSupportDetails swap_chain_support = device_.GetSwapChainSupport();

  VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.formats);
  VkPresentModeKHR present_mode = ChooseSwapPresentMode(swap_chain_support.present_modes);
  VkExtent2D extent = ChooseSwapExtent(swap_chain_support.capabilities);

  uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
  if (swap_chain_support.capabilities.maxImageCount > 0 &&
      image_count > swap_chain_support.capabilities.maxImageCount) {
    image_count = swap_chain_support.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = device_.GetSurface();

  create_info.minImageCount = image_count;
  create_info.imageFormat = surface_format.format;
  create_info.imageColorSpace = surface_format.colorSpace;
  create_info.imageExtent = extent;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices = device_.FindPhysicalQueueFamilies();
  uint32_t queue_family_indices[] = {indices.graphics_family, indices.present_family};

  if (indices.graphics_family != indices.present_family) {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = queue_family_indices;
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;      // Optional
    create_info.pQueueFamilyIndices = nullptr;  // Optional
  }

  create_info.preTransform = swap_chain_support.capabilities.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE;

  create_info.oldSwapchain = old_swap_chain_ == nullptr ? VK_NULL_HANDLE :old_swap_chain_->swap_chain_;

  if (vkCreateSwapchainKHR(device_.GetDevice(), &create_info, nullptr, &swap_chain_) != VK_SUCCESS)
    throw std::runtime_error("failed to create swap chain!");

  // we only specified a minimum number of images in the swap chain, so the implementation is
  // allowed to create a swap chain with more. That's why we'll first query the final number of
  // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
  // retrieve the handles.
  vkGetSwapchainImagesKHR(device_.GetDevice(), swap_chain_, &image_count, nullptr);
  swap_chain_images_.resize(image_count);
  vkGetSwapchainImagesKHR(device_.GetDevice(), swap_chain_, &image_count, swap_chain_images_.data());

  swap_chain_image_format_ = surface_format.format;
  swap_chain_extent_ = extent;
}

void SwapChain::CreateImageViews() 
{
  swap_chain_image_views_.resize(swap_chain_images_.size());
  for (size_t i = 0; i < swap_chain_images_.size(); i++) {
    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = swap_chain_images_[i];
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = swap_chain_image_format_;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(
          device_.GetDevice(), 
          &view_info, 
          nullptr, 
          &swap_chain_image_views_[i]) 
      != VK_SUCCESS) 
    {
      throw std::runtime_error("failed to create texture image view!");
    }
  }
}

void SwapChain::CreateRenderPass() 
{
  VkAttachmentDescription depth_attachment{};
  depth_attachment.format = FindDepthFormat();
  depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_ref{};
  depth_attachment_ref.attachment = 1;
  depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription color_attachment = {};
  color_attachment.format = GetSwapChainImageFormat();
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_ref = {};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;
  subpass.pDepthStencilAttachment = &depth_attachment_ref;

  VkSubpassDependency dependency = {};
  dependency.dstSubpass = 0;
  dependency.dstAccessMask =
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  dependency.dstStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.srcAccessMask = 0;
  dependency.srcStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

  std::array<VkAttachmentDescription, 2> attachments = {color_attachment, depth_attachment};
  VkRenderPassCreateInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
  render_pass_info.pAttachments = attachments.data();
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if (
    vkCreateRenderPass(
        device_.GetDevice(), 
        &render_pass_info, 
        nullptr, 
        &render_pass_)
    != VK_SUCCESS) 
  {
    throw std::runtime_error("failed to create render pass!");
  }
}

void SwapChain::CreateFramebuffers() 
{
  swap_chain_framebuffers_.resize(GetImageCount());
  for (size_t i = 0; i < GetImageCount(); i++) {
    std::array<VkImageView, 2> attachments = {swap_chain_image_views_[i], depth_image_views_[i]};

    VkExtent2D swap_chain_extent_ = GetSwapChainExtent();
    VkFramebufferCreateInfo framebuffer_info = {};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = render_pass_;
    framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebuffer_info.pAttachments = attachments.data();
    framebuffer_info.width = swap_chain_extent_.width;
    framebuffer_info.height = swap_chain_extent_.height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(
            device_.GetDevice(),
            &framebuffer_info,
            nullptr,
            &swap_chain_framebuffers_[i])
        != VK_SUCCESS) 
    {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void SwapChain::CreateDepthResources() 
{
  VkFormat depth_format = FindDepthFormat();
  swap_chain_depth_format_ = depth_format;
  VkExtent2D swap_chain_extent_ = GetSwapChainExtent();

  depth_images_.resize(GetImageCount());
  depth_image_memories_.resize(GetImageCount());
  depth_image_views_.resize(GetImageCount());

  for (int i = 0; i < depth_images_.size(); i++) {
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = swap_chain_extent_.width;
    image_info.extent.height = swap_chain_extent_.height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = depth_format;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.flags = 0;

    device_.CreateImageWithInfo(
        image_info,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        depth_images_[i],
        depth_image_memories_[i]);

    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = depth_images_[i];
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = depth_format;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    if (
      vkCreateImageView(
          device_.GetDevice(), 
          &view_info, 
          nullptr, 
          &depth_image_views_[i]) 
      != VK_SUCCESS) 
    {
      throw std::runtime_error("failed to create texture image view!");
    }
  }
}

void SwapChain::CreateSyncObjects() 
{
  image_available_semaphores_.resize(kMaxFramesInFlight);
  render_finished_semaphores_.resize(kMaxFramesInFlight);
  in_flight_fences_.resize(kMaxFramesInFlight);
  images_in_flight_.resize(GetImageCount(), VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphore_info = {};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence_info = {};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < kMaxFramesInFlight; i++) 
  {
    if (vkCreateSemaphore(device_.GetDevice(), &semaphore_info, nullptr, &image_available_semaphores_[i]) !=
            VK_SUCCESS ||
        vkCreateSemaphore(device_.GetDevice(), &semaphore_info, nullptr, &render_finished_semaphores_[i]) !=
            VK_SUCCESS ||
        vkCreateFence(device_.GetDevice(), &fence_info, nullptr, &in_flight_fences_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
  }
}

VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &available_formats) const
{
  for (const auto &available_f : available_formats) 
  {
    if (available_f.format == VK_FORMAT_B8G8R8A8_SRGB &&
        available_f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return available_f;
    }
  }

  return available_formats[0];
}

VkPresentModeKHR SwapChain::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &available_present_modes) const
{
  /*for (const auto &available_mode : available_present_modes) {
    if (available_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      std::cout << "Present mode: Mailbox" << std::endl;
      return available_mode;
    }
  }

  for (const auto &available_mode : available_present_modes) {
    if (available_mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
      std::cout << "Present mode: Immediate" << std::endl;
      return available_mode;
    }
  }*/

  std::cout << "Present mode: V-Sync" << std::endl;
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const
{
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    VkExtent2D actual_extent = window_extent_;
    actual_extent.width = std::max(
        capabilities.minImageExtent.width,
        std::min(capabilities.maxImageExtent.width, actual_extent.width));
    actual_extent.height = std::max(
        capabilities.minImageExtent.height,
        std::min(capabilities.maxImageExtent.height, actual_extent.height));

    return actual_extent;
  }
}

VkFormat SwapChain::FindDepthFormat() const
{
  return device_.FindSupportedFormat(
      {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
      VK_IMAGE_TILING_OPTIMAL,
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

}  // namespace heh
