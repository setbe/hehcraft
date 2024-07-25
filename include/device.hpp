#pragma once

#include "window.hpp"

// Standard libraries
#include <string>
#include <vector>

namespace heh {

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> present_modes;
};

struct QueueFamilyIndices {
  uint32_t graphics_family;
  uint32_t present_family;
  bool graphics_family_has_value = false;
  bool present_family_has_value = false;

  bool IsComplete() const {
    return graphics_family_has_value && present_family_has_value;
  }
};

class Device {
 public:
#ifdef NDEBUG
  static constexpr bool kEnableValidationLayers = false;
#else
  static constexpr bool kEnableValidationLayers = true;
#endif

  explicit Device(Window &window);
  ~Device();

  // Not copyable or movable
  Device(const Device &) = delete;
  Device &operator=(const Device &) = delete;
  Device(Device &&) = delete;
  Device &operator=(Device &&) = delete;

  VkCommandPool GetCommandPool() const { return command_pool_; }
  VkDevice GetDevice() const { return device_; }
  VkSurfaceKHR GetSurface() const { return surface_; }
  VkQueue GetGraphicsQueue() const { return graphics_queue_; }
  VkQueue GetPresentQueue() const { return present_queue_; }

  SwapChainSupportDetails GetSwapChainSupport() const {
    return QuerySwapChainSupport(physical_device_);
  }

  uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);
  
  QueueFamilyIndices FindPhysicalQueueFamilies() const {
    return FindQueueFamilies(physical_device_);
  }

  VkFormat FindSupportedFormat(
      const std::vector<VkFormat> &candidates, 
      VkImageTiling tiling, 
      VkFormatFeatureFlags features);

  // Buffer helper functions
  void CreateBuffer(
      VkDeviceSize size,
      VkBufferUsageFlags usage,
      VkMemoryPropertyFlags properties,
      VkBuffer &buffer,
      VkDeviceMemory &buffer_memory);
      
  VkCommandBuffer BeginSingleTimeCommands();
  void EndSingleTimeCommands(VkCommandBuffer command_buffer);
  void CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
  void CopyBufferToImage(
      VkBuffer buffer, 
      VkImage image, 
      uint32_t width, 
      uint32_t height, 
      uint32_t layer_count);

  void CreateImageWithInfo(
      const VkImageCreateInfo &image_info,
      VkMemoryPropertyFlags properties,
      VkImage &image,
      VkDeviceMemory &image_memory);

  VkPhysicalDeviceProperties properties;

 private:
  void CreateInstance();
  void SetupDebugMessenger();
  void CreateSurface();
  void PickPhysicalDevice();
  void CreateLogicalDevice();
  void CreateCommandPool();

  // Helper functions
  bool IsDeviceSuitable(VkPhysicalDevice device);
  std::vector<const char *> GetRequiredExtensions() const;
  bool CheckValidationLayerSupport() const;
  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) const;
  void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &create_info);
  void HasGlfwRequiredInstanceExtensions() const;
  bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;
  SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) const;

  VkInstance instance_;
  VkDebugUtilsMessengerEXT debug_messenger_;
  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  Window &window_;
  VkCommandPool command_pool_;

  VkDevice device_;
  VkSurfaceKHR surface_;
  VkQueue graphics_queue_;
  VkQueue present_queue_;

  const std::vector<const char *> kValidationLayers = {"VK_LAYER_KHRONOS_validation"};
  const std::vector<const char *> kDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

}  // namespace heh
