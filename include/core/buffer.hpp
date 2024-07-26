#pragma once

#include "core/device.hpp"

namespace heh {

class Buffer {
 public:
  Buffer(
      Device &device,
      VkDeviceSize instance_size,
      uint32_t instance_count,
      VkBufferUsageFlags usage_flags,
      VkMemoryPropertyFlags memory_property_flags,
      VkDeviceSize min_offset_alignment = 1);

  ~Buffer();

  // Non-copyable and non-movable
  Buffer(const Buffer &) = delete;
  Buffer &operator=(const Buffer &) = delete;

  VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  void Unmap();

  void WriteToBuffer(void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

  void WriteToIndex(void *data, int index);
  VkResult FlushIndex(int index);
  VkDescriptorBufferInfo DescriptorInfoForIndex(int index);
  VkResult InvalidateIndex(int index);

  VkBuffer GetBuffer() const { return buffer_; }
  void *GetMappedMemory() const { return mapped_; }
  uint32_t GetInstanceCount() const { return instance_count_; }
  VkDeviceSize GetInstanceSize() const { return instance_size_; }
  VkDeviceSize GetAlignmentSize() const { return alignment_size_; }
  VkBufferUsageFlags GetUsageFlags() const { return usage_flags_; }
  VkMemoryPropertyFlags GetMemoryPropertyFlags() const { return memory_property_flags_; }
  VkDeviceSize GetBufferSize() const { return buffer_size_; }

 private:
  static VkDeviceSize GetAlignment(VkDeviceSize instance_size, VkDeviceSize min_offset_alignment);

  Device &device_;
  void *mapped_ = nullptr;
  VkBuffer buffer_ = VK_NULL_HANDLE;
  VkDeviceMemory memory_ = VK_NULL_HANDLE;

  VkDeviceSize buffer_size_;
  uint32_t instance_count_;
  VkDeviceSize instance_size_;
  VkDeviceSize alignment_size_;
  VkBufferUsageFlags usage_flags_;
  VkMemoryPropertyFlags memory_property_flags_;
};

}  // namespace heh
