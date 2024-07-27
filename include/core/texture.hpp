#pragma once

#include "core/device.hpp"
#include "core/buffer.hpp"

// libs
#include <vulkan/vulkan.h>

// std
#include <string>
#include <stdexcept>

namespace heh {

class Texture {
 public:
  Texture(Device &device, const char* filepath, bool has_alpha = true);

  ~Texture();

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;
  Texture (Texture&&) = delete;
  Texture& operator=(Texture&&) = delete;

  VkImageView GetImageView() const { return image_view_; }
  VkSampler GetSampler() const { return sampler_; }
  VkImageLayout GetImageLayout() const { return image_layout_; }

 private:
  void Load(const char* filepath, bool has_alpha = true);

  void CreateImageView(VkImageAspectFlags aspect_flags, bool has_alpha);
  void CreateSampler();

  void TransitionImageLayout(VkImageLayout old_layout, VkImageLayout new_layout);
  void GenerateMipmaps();

  int width_;
  int height_;
  int mip_levels_;

  Device &device_;

  VkImage image_;
  VkDeviceMemory image_memory_;
  VkImageView image_view_;
  VkSampler sampler_;
  VkImageLayout image_layout_;
  VkFormat format_;
};  // class Texture

// class TextureManager // TODO: Implement this class
// loads or unloads textures when player sees them or not

}  // namespace heh