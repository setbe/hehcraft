#pragma once

#include "device.hpp"

// Standard library headers
#include <memory>
#include <unordered_map>
#include <vector>

namespace heh {

class DescriptorSetLayout {
 public:
  class Builder {
   public:
    explicit Builder(Device &device) : device_{device} {}

    Builder &AddBinding(
        uint32_t binding,
        VkDescriptorType descriptor_type,
        VkShaderStageFlags stage_flags,
        uint32_t count = 1);

    std::unique_ptr<DescriptorSetLayout> Build() const;

   private:
    Device &device_;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_;
  };

  DescriptorSetLayout(
      Device &device,
      std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);

  ~DescriptorSetLayout();

  DescriptorSetLayout(const DescriptorSetLayout &) = delete;
  DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

  VkDescriptorSetLayout GetDescriptorSetLayout() const { return descriptor_set_layout_; }

 private:
  Device &device_;
  VkDescriptorSetLayout descriptor_set_layout_;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_;

  friend class DescriptorWriter;
};

class DescriptorPool {
 public:
  class Builder {
   public:
    explicit Builder(Device &device) : device_{device} {}

    Builder &AddPoolSize(VkDescriptorType descriptor_type, uint32_t count);
    Builder &SetPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder &SetMaxSets(uint32_t count);
    std::unique_ptr<DescriptorPool> Build() const;

   private:
    Device &device_;
    std::vector<VkDescriptorPoolSize> pool_sizes_;
    uint32_t max_sets_ = 1000;
    VkDescriptorPoolCreateFlags pool_flags_ = 0;
  };

  DescriptorPool(
      Device &device,
      uint32_t max_sets,
      VkDescriptorPoolCreateFlags pool_flags,
      const std::vector<VkDescriptorPoolSize> &pool_sizes);

  ~DescriptorPool();

  DescriptorPool(const DescriptorPool &) = delete;
  DescriptorPool &operator=(const DescriptorPool &) = delete;

  bool AllocateDescriptor(
      const VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet &descriptor) const;

  void FreeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

  void ResetPool();

 private:
  Device &device_;
  VkDescriptorPool descriptor_pool_;

  friend class DescriptorWriter;
};

class DescriptorWriter {
 public:
  DescriptorWriter(DescriptorSetLayout &set_layout, DescriptorPool &pool);

  DescriptorWriter &WriteBuffer(uint32_t binding, VkDescriptorBufferInfo *buffer_info);
  DescriptorWriter &WriteImage(uint32_t binding, VkDescriptorImageInfo *image_info);

  bool Build(VkDescriptorSet &set);
  void Overwrite(VkDescriptorSet &set);

 private:
  DescriptorSetLayout &set_layout_;
  DescriptorPool &pool_;
  std::vector<VkWriteDescriptorSet> writes_;
};

}  // namespace heh
