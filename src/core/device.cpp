#include "core/device.hpp"

// std headers
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

namespace heh {

// local callback functions
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
    VkDebugUtilsMessageTypeFlagsEXT             message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallback_data,
    void                                       *pUser_data) 
{
  std::cerr << "validation layer: " << pCallback_data->pMessage << std::endl;
  return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance                                instance,
    const VkDebugUtilsMessengerCreateInfoEXT *pCreate_info,
    const VkAllocationCallbacks              *pAllocator,
    VkDebugUtilsMessengerEXT                 *pDebug_messenger) 
{
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");

  if (func != nullptr) {
    return func(instance, pCreate_info, pAllocator, pDebug_messenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void DestroyDebugUtilsMessengerEXT(
    VkInstance                   instance,
    VkDebugUtilsMessengerEXT     debug_messenger,
    const VkAllocationCallbacks *pAllocator) 
{
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");

  if (func != nullptr)
    func(instance, debug_messenger, pAllocator);
}

// class member functions
Device::Device(Window &window) : window_{window} 
{
  CreateInstance();
  SetupDebugMessenger();
  CreateSurface();
  PickPhysicalDevice();
  CreateLogicalDevice();
  CreateCommandPool();
}

Device::~Device() 
{
  vkDestroyCommandPool(device_, command_pool_, nullptr);
  vkDestroyDevice(device_, nullptr);

  if (kEnableValidationLayers)
    DestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);

  vkDestroySurfaceKHR(instance_, surface_, nullptr);
  vkDestroyInstance(instance_, nullptr);
}

void Device::CreateInstance() 
{
  if (kEnableValidationLayers && !CheckValidationLayerSupport())
    throw std::runtime_error("validation layers requested, but not available!");

  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "LittleVulkanEngine App";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "No Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  auto extensions = GetRequiredExtensions();
  create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  create_info.ppEnabledExtensionNames = extensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
  if (kEnableValidationLayers) {
    create_info.enabledLayerCount = static_cast<uint32_t>(kValidationLayers.size());
    create_info.ppEnabledLayerNames = kValidationLayers.data();

    PopulateDebugMessengerCreateInfo(debug_create_info);
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debug_create_info;
  } else {
    create_info.enabledLayerCount = 0;
    create_info.pNext = nullptr;
  }

  if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS)
    throw std::runtime_error("failed to create instance!");

  HasGlfwRequiredInstanceExtensions();
}

void Device::PickPhysicalDevice() 
{
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);
  if (device_count == 0)
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  
  std::cout << "Device count: " << device_count << std::endl;
  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

  for (const auto &device : devices) {
    if (IsDeviceSuitable(device)) {
      physical_device_ = device;
      break;
    }
  }

  if (physical_device_ == VK_NULL_HANDLE)
    throw std::runtime_error("failed to find a suitable GPU!");

  vkGetPhysicalDeviceProperties(physical_device_, &properties);
  std::cout << "physical device: " << properties.deviceName << std::endl;
}

void Device::CreateLogicalDevice() 
{
  QueueFamilyIndices indices = FindQueueFamilies(physical_device_);

  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  std::set<uint32_t> unique_queue_families = {indices.graphics_family, indices.present_family};

  float queue_priority = 1.0f;
  for (uint32_t queue_family : unique_queue_families) {
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue_family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos.push_back(queue_create_info);
  }

  VkPhysicalDeviceFeatures device_features = {};
  device_features.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
  create_info.pQueueCreateInfos = queue_create_infos.data();

  create_info.pEnabledFeatures = &device_features;
  create_info.enabledExtensionCount = static_cast<uint32_t>(kDeviceExtensions.size());
  create_info.ppEnabledExtensionNames = kDeviceExtensions.data();

  // might not really be necessary anymore because device specific validation layers
  // have been deprecated
  if (kEnableValidationLayers) {
    create_info.enabledLayerCount = static_cast<uint32_t>(kValidationLayers.size());
    create_info.ppEnabledLayerNames = kValidationLayers.data();
  } else {
    create_info.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physical_device_, &create_info, nullptr, &device_) != VK_SUCCESS)
    throw std::runtime_error("failed to create logical device!");

  vkGetDeviceQueue(device_, indices.graphics_family, 0, &graphics_queue_);
  vkGetDeviceQueue(device_, indices.present_family, 0, &present_queue_);
}

void Device::CreateCommandPool() 
{
  QueueFamilyIndices queue_family_indices = FindPhysicalQueueFamilies();

  VkCommandPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = queue_family_indices.graphics_family;
  pool_info.flags =
      VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  if (vkCreateCommandPool(device_, &pool_info, nullptr, &command_pool_) != VK_SUCCESS)
    throw std::runtime_error("failed to create command pool!");
}

void Device::CreateSurface() { window_.CreateWindowSurface(instance_, &surface_); }

bool Device::IsDeviceSuitable(VkPhysicalDevice device) 
{
  QueueFamilyIndices indices = FindQueueFamilies(device);

  bool extensions_supported = CheckDeviceExtensionSupport(device);

  bool swap_chain_adequate = false;
  if (extensions_supported) {
    SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(device);
    swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
  }

  VkPhysicalDeviceFeatures supported_features;
  vkGetPhysicalDeviceFeatures(device, &supported_features);

  return indices.IsComplete() && extensions_supported && swap_chain_adequate &&
         supported_features.samplerAnisotropy;
}

void Device::PopulateDebugMessengerCreateInfo(
  VkDebugUtilsMessengerCreateInfoEXT &create_info) 
{
  create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = debug_callback;
  create_info.pUserData = nullptr;  // Optional
}

void Device::SetupDebugMessenger() 
{
  if (!kEnableValidationLayers) {
    std::cout << "Validation layers not enabled, skipping debug messenger setup" << std::endl;
    return;
  }
  VkDebugUtilsMessengerCreateInfoEXT create_info;
  PopulateDebugMessengerCreateInfo(create_info);
  if (CreateDebugUtilsMessengerEXT(instance_, &create_info, nullptr, &debug_messenger_) != VK_SUCCESS)
    throw std::runtime_error("failed to set up debug messenger!");
}

bool Device::CheckValidationLayerSupport() const 
{
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  for (const char *layer_name : kValidationLayers) 
  {
    bool layer_found = false;

    for (const auto &layer_properties : available_layers) {
      if (strcmp(layer_name, layer_properties.layerName) == 0) {
        layer_found = true;
        break;
      }
    }
    if (!layer_found)
      return false;
  }

  return true;
}

std::vector<const char *> Device::GetRequiredExtensions() const 
{
  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

  if (kEnableValidationLayers)
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  return extensions;
}

void Device::HasGlfwRequiredInstanceExtensions() const 
{
  uint32_t extension_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> extensions(extension_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

  std::cout << "available extensions:" << std::endl;
  std::unordered_set<std::string> available;
  for (const auto &extension : extensions) {
    std::cout << "\t" << extension.extensionName << std::endl;
    available.insert(extension.extensionName);
  }

  std::cout << "required extensions:" << std::endl;
  auto required_extensions = GetRequiredExtensions();
  for (const auto &required : required_extensions) {
    std::cout << "\t" << required << std::endl;
    if (available.find(required) == available.end())
      throw std::runtime_error("Missing required glfw extension");
  }
}

bool Device::CheckDeviceExtensionSupport(VkPhysicalDevice device) const 
{
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(
      device,
      nullptr,
      &extension_count,
      available_extensions.data());

  std::set<std::string> required_extensions(kDeviceExtensions.begin(), kDeviceExtensions.end());

  for (const auto &extension : available_extensions) {
    required_extensions.erase(extension.extensionName);
  }

  return required_extensions.empty();
}

QueueFamilyIndices Device::FindQueueFamilies(VkPhysicalDevice device) const 
{
  QueueFamilyIndices indices;

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

  int i = 0;
  for (const auto &queue_family : queue_families) {
    if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family = i;
      indices.graphics_family_has_value = true;
    }
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &present_support);
    if (queue_family.queueCount > 0 && present_support) {
      indices.present_family = i;
      indices.present_family_has_value = true;
    }
    if (indices.IsComplete())
      break;

    i++;
  }

  return indices;
}

SwapChainSupportDetails Device::QuerySwapChainSupport(VkPhysicalDevice device) const 
{
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, nullptr);

  if (format_count != 0) {
    details.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, details.formats.data());
  }

  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, nullptr);

  if (present_mode_count != 0) {
    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device,
        surface_,
        &present_mode_count,
        details.present_modes.data());
  }
  return details;
}

VkFormat Device::FindSupportedFormat(
    const std::vector<VkFormat> &candidates, 
    VkImageTiling                tiling, 
    VkFormatFeatureFlags         features) 
{
  for (VkFormat format : candidates) 
  {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physical_device_, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
      return format;
    } else if (
        tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }
  throw std::runtime_error("failed to find supported format!");
}

uint32_t Device::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties mem_properties;
  vkGetPhysicalDeviceMemoryProperties(physical_device_, &mem_properties);
  for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
    if ((type_filter & (1 << i)) &&
        (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

void Device::CreateBuffer(
    VkDeviceSize          size,
    VkBufferUsageFlags    usage,
    VkMemoryPropertyFlags properties,
    VkBuffer              &buffer,
    VkDeviceMemory        &buffer_memory) 
{
  VkBufferCreateInfo buffer_info{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = size;
  buffer_info.usage = usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device_, &buffer_info, nullptr, &buffer) != VK_SUCCESS)
    throw std::runtime_error("failed to create vertex buffer!");

  VkMemoryRequirements mem_requirements;
  vkGetBufferMemoryRequirements(device_, buffer, &mem_requirements);

  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties);

  if (vkAllocateMemory(device_, &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS)
    throw std::runtime_error("failed to allocate vertex buffer memory!");

  vkBindBufferMemory(device_, buffer, buffer_memory, 0);
}

VkCommandBuffer Device::BeginSingleTimeCommands() {
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandPool = command_pool_;
  alloc_info.commandBufferCount = 1;

  VkCommandBuffer command_buffer;
  vkAllocateCommandBuffers(device_, &alloc_info, &command_buffer);

  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(command_buffer, &begin_info);
  return command_buffer;
}

void Device::EndSingleTimeCommands(VkCommandBuffer command_buffer) {
  vkEndCommandBuffer(command_buffer);

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer;

  vkQueueSubmit(graphics_queue_, 1, &submit_info, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphics_queue_);

  vkFreeCommandBuffers(device_, command_pool_, 1, &command_buffer);
}

void Device::CopyBuffer(
  VkBuffer     src_buffer, 
  VkBuffer     dst_buffer, 
  VkDeviceSize size) 
{
  VkCommandBuffer command_buffer = BeginSingleTimeCommands();

  VkBufferCopy copy_region{};
  copy_region.srcOffset = 0;  // Optional
  copy_region.dstOffset = 0;  // Optional
  copy_region.size = size;
  vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

  EndSingleTimeCommands(command_buffer);
}

void Device::CopyBufferToImage(
    VkBuffer buffer, 
    VkImage  image, 
    uint32_t width, 
    uint32_t height, 
    uint32_t layer_count) 
{
  VkCommandBuffer command_buffer = BeginSingleTimeCommands();

  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = layer_count;

  region.imageOffset = {0, 0, 0};
  region.imageExtent = {width, height, 1};

  vkCmdCopyBufferToImage(
      command_buffer,
      buffer,
      image,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      1,
      &region);
  EndSingleTimeCommands(command_buffer);
}

void Device::CreateImageWithInfo(
    const VkImageCreateInfo &image_info,
    VkMemoryPropertyFlags   properties,
    VkImage                 &image,
    VkDeviceMemory          &image_memory) 
{
  if (vkCreateImage(device_, &image_info, nullptr, &image) != VK_SUCCESS)
    throw std::runtime_error("failed to create image!");

  VkMemoryRequirements mem_requirements;
  vkGetImageMemoryRequirements(device_, image, &mem_requirements);

  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties);

  if (vkAllocateMemory(device_, &alloc_info, nullptr, &image_memory) != VK_SUCCESS)
    throw std::runtime_error("failed to allocate image memory!");

  if (vkBindImageMemory(device_, image, image_memory, 0) != VK_SUCCESS)
    throw std::runtime_error("failed to bind image memory!");
}

}  // namespace heh