#include "descriptors.hpp"
 
// std
#include <cassert>
#include <stdexcept>
 
namespace heh {
 
// *************** Descriptor Set Layout Builder *********************
 
DescriptorSetLayout::Builder &DescriptorSetLayout::Builder::AddBinding(
    uint32_t            binding,
    VkDescriptorType    descriptor_type,
    VkShaderStageFlags  stage_flags,
    uint32_t            count) 
{
  assert(bindings_.count(binding) == 0 && "Binding already in use");
  VkDescriptorSetLayoutBinding layout_binding{};
  layout_binding.binding = binding;
  layout_binding.descriptorType = descriptor_type;
  layout_binding.descriptorCount = count;
  layout_binding.stageFlags = stage_flags;
  bindings_[binding] = layout_binding;
  return *this;
}
 
std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const {
  return std::make_unique<DescriptorSetLayout>(device_, bindings_);
}
 
// *************** Descriptor Set Layout *********************
 
DescriptorSetLayout::DescriptorSetLayout(
    Device &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : device_{device}, bindings_{bindings}
{
  std::vector<VkDescriptorSetLayoutBinding> set_layout_bindings{};
  for (auto kv : bindings_) {
    set_layout_bindings.push_back(kv.second);
  }
 
  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
  descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(set_layout_bindings.size());
  descriptorSetLayoutInfo.pBindings = set_layout_bindings.data();
 
  if (vkCreateDescriptorSetLayout(
          device_.GetDevice(),
          &descriptorSetLayoutInfo,
          nullptr,
          &descriptor_set_layout_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}
 
DescriptorSetLayout::~DescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(device_.GetDevice(), descriptor_set_layout_, nullptr);
}
 
// *************** Descriptor Pool Builder *********************
 
DescriptorPool::Builder &DescriptorPool::Builder::AddPoolSize(
    VkDescriptorType descriptorType, uint32_t count)
{
  pool_sizes_.push_back({descriptorType, count});
  return *this;
}
 
DescriptorPool::Builder &DescriptorPool::Builder::SetPoolFlags(
    VkDescriptorPoolCreateFlags flags)
{
  pool_flags_ = flags;
  return *this;
}
DescriptorPool::Builder &DescriptorPool::Builder::SetMaxSets(uint32_t count) 
{
  max_sets_ = count;
  return *this;
}
 
std::unique_ptr<DescriptorPool> DescriptorPool::Builder::Build() const {
  return std::make_unique<DescriptorPool>(device_, max_sets_, pool_flags_, pool_sizes_);
}
 
// *************** Descriptor Pool *********************
 
DescriptorPool::DescriptorPool(
    Device                                  &device,
    uint32_t                                max_sets,
    VkDescriptorPoolCreateFlags             pool_flags,
    const std::vector<VkDescriptorPoolSize> &pool_sizes)
    : device_{device} 
{
  VkDescriptorPoolCreateInfo descriptor_pool_info{};
  descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
  descriptor_pool_info.pPoolSizes = pool_sizes.data();
  descriptor_pool_info.maxSets = max_sets;
  descriptor_pool_info.flags = pool_flags;
 
  if (vkCreateDescriptorPool(
        device_.GetDevice(), 
        &descriptor_pool_info, 
        nullptr, 
        &descriptor_pool_) 
    != VK_SUCCESS) 
  {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}
 
DescriptorPool::~DescriptorPool() {
  vkDestroyDescriptorPool(device_.GetDevice(), descriptor_pool_, nullptr);
}
 
bool DescriptorPool::AllocateDescriptor(
    const VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet &descriptor) const 
{
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptor_pool_;
  allocInfo.pSetLayouts = &descriptor_set_layout;
  allocInfo.descriptorSetCount = 1;
 
  // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
  // a new pool whenever an old pool fills up. But this is beyond our current scope
  if (vkAllocateDescriptorSets(device_.GetDevice(), &allocInfo, &descriptor) != VK_SUCCESS)
    return false;

  return true;
}
 
void DescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet> &descriptors) const 
{
  vkFreeDescriptorSets(
      device_.GetDevice(),
      descriptor_pool_,
      static_cast<uint32_t>(descriptors.size()),
      descriptors.data());
}
 
void DescriptorPool::ResetPool() {
  vkResetDescriptorPool(device_.GetDevice(), descriptor_pool_, 0);
}
 
// *************** Descriptor Writer *********************
 
DescriptorWriter::DescriptorWriter(DescriptorSetLayout &set_Layout, DescriptorPool &pool)
    : set_layout_{set_Layout}, pool_{pool} 
{}
 
DescriptorWriter &DescriptorWriter::WriteBuffer(
    uint32_t binding, VkDescriptorBufferInfo *buffer_info) 
{
  assert(set_layout_.bindings_.count(binding) == 1 && "Layout does not contain specified binding");
 
  auto &binding_description = set_layout_.bindings_[binding];
  assert(binding_description.descriptorCount == 1 &&
      "Binding single descriptor info, but binding expects multiple");
 
  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = binding_description.descriptorType;
  write.dstBinding = binding;
  write.pBufferInfo = buffer_info;
  write.descriptorCount = 1;
 
  writes_.push_back(write);
  return *this;
}
 
DescriptorWriter &DescriptorWriter::WriteImage(
    uint32_t binding, VkDescriptorImageInfo *imageInfo) 
{
  assert(set_layout_.bindings_.count(binding) == 1 && "Layout does not contain specified binding");
 
  auto &binding_description = set_layout_.bindings_[binding];
  assert(binding_description.descriptorCount == 1 &&
      "Binding single descriptor info, but binding expects multiple");
 
  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = binding_description.descriptorType;
  write.dstBinding = binding;
  write.pImageInfo = imageInfo;
  write.descriptorCount = 1;
 
  writes_.push_back(write);
  return *this;
}
 
bool DescriptorWriter::Build(VkDescriptorSet &set) 
{
  bool success = pool_.AllocateDescriptor(set_layout_.GetDescriptorSetLayout(), set);
  if (!success)
    return false;
  Overwrite(set);
  return true;
}
 
void DescriptorWriter::Overwrite(VkDescriptorSet &set) 
{
  for (auto &write : writes_) {
    write.dstSet = set;
  }
  vkUpdateDescriptorSets(pool_.device_.GetDevice(), writes_.size(), writes_.data(), 0, nullptr);
}
 
}  // namespace heh