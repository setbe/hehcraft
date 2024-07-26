#pragma once

#include "core/device.hpp"

// std
#include <string>
#include <vector>

namespace heh {

struct PipelineConfigInfo {
  PipelineConfigInfo(const PipelineConfigInfo&) = delete;
  PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

  std::vector<VkVertexInputBindingDescription>   binding_descriptions{};
  std::vector<VkVertexInputAttributeDescription> attribute_descriptions{};
  VkPipelineViewportStateCreateInfo              viewport_info;
  VkPipelineInputAssemblyStateCreateInfo         input_assembly_info;
  VkPipelineRasterizationStateCreateInfo rasterization_info;
  VkPipelineMultisampleStateCreateInfo   multisample_info;
  VkPipelineColorBlendAttachmentState    color_blend_attachment;
  VkPipelineColorBlendStateCreateInfo    color_blend_info;
  VkPipelineDepthStencilStateCreateInfo  depth_stencil_info;
  std::vector<VkDynamicState>            dynamic_state_enables;
  VkPipelineDynamicStateCreateInfo       dynamic_state_info;
  VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
  VkRenderPass     render_pass = VK_NULL_HANDLE;
  uint32_t         subpass = 0;
};

class Pipeline {
 public:
  Pipeline(
      Device& device,
      const std::string& vert_filepath,
      const std::string& frag_filepath,
      const PipelineConfigInfo& config_info);
  ~Pipeline();

  Pipeline(const Pipeline&) = delete;
  Pipeline& operator=(const Pipeline&) = delete;

  void Bind(VkCommandBuffer command_buffer);

  static void DefaultPipelineConfigInfo(PipelineConfigInfo& config_info);

 private:
  static std::vector<char> ReadFile(const std::string& filepath);

  void CreateGraphicsPipeline(
      const std::string& vert_filepath,
      const std::string& frag_filepath,
      const PipelineConfigInfo& config_info);

  void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shader_module);

  Device& device_;
  VkPipeline graphics_pipeline_;
  VkShaderModule vert_shader_module_;
  VkShaderModule frag_shader_module_;
};

}  // namespace heh
