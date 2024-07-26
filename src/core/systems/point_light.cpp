#include "core/systems/point_light.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>
#include <map>

namespace heh {
  
PointLightSystem::PointLightSystem(
  Device&               device, 
  VkRenderPass          render_pass, 
  VkDescriptorSetLayout global_set_layout)
    : device_{device} 
{
  CreatePipelineLayout(global_set_layout);
  CreatePipeline(render_pass);
}

PointLightSystem::~PointLightSystem() {
  vkDestroyPipelineLayout(device_.GetDevice(), pipeline_layout_, nullptr);
}

void PointLightSystem::CreatePipelineLayout(
  VkDescriptorSetLayout global_set_layout) 
{
  VkPushConstantRange push_constant_range{};
  push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  push_constant_range.offset = 0;
  push_constant_range.size = sizeof(PointLightPushConstant);

  std::vector<VkDescriptorSetLayout> descriptor_set_layouts{global_set_layout};

  VkPipelineLayoutCreateInfo pipeline_layout_info{};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
  pipeline_layout_info.pSetLayouts = descriptor_set_layouts.data();
  pipeline_layout_info.pushConstantRangeCount = 1;
  pipeline_layout_info.pPushConstantRanges = &push_constant_range;
  if (vkCreatePipelineLayout(
        device_.GetDevice(), 
        &pipeline_layout_info, 
        nullptr, 
        &pipeline_layout_)
    != VK_SUCCESS) 
  {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void PointLightSystem::CreatePipeline(VkRenderPass render_pass) 
{
  assert(pipeline_layout_ != nullptr && "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipeline_config{};
  Pipeline::DefaultPipelineConfigInfo(pipeline_config);
  Pipeline::EnableAlphaBlending(pipeline_config);

  pipeline_config.attribute_descriptions.clear();
  pipeline_config.binding_descriptions.clear();
  pipeline_config.render_pass = render_pass;
  pipeline_config.pipeline_layout = pipeline_layout_;

  pipeline_ = std::make_unique<Pipeline>(
      device_,
      "shaders/point_light.vert.spv",
      "shaders/point_light.frag.spv",
      pipeline_config);
}

void PointLightSystem::Update(FrameInfo &frame_info, GlobalUbo &ubo) 
{
  auto rotate_light = glm::rotate(
      glm::mat4(1.f), 
      frame_info.frame_time, 
      glm::vec3(0.f, -1.f, 0.f));

  int light_index = 0;

  for (auto& kv : frame_info.game_objects) {
    GameObject& obj = kv.second;
    if (obj.point_light == nullptr)
      continue;

    assert(light_index < kMaxLight && 
        "point light count exceeds max lights");

    // update light position
    obj.transform.translation = 
      glm::vec3(rotate_light * glm::vec4(obj.transform.translation, 1.f));
    
    // copy light to ubo
    ubo.point_lights[light_index].position = glm::vec4(obj.transform.translation, 1.0f);
    ubo.point_lights[light_index].color = glm::vec4(obj.color, obj.point_light->light_intensity);
    
    light_index++;
  }
  ubo.light_count = light_index;
}

void PointLightSystem::Render(FrameInfo &frame_info) 
{
  // sort lights by distance to camera
  std::map<float, GameObject::IdType> sorted;

  for (auto& kv : frame_info.game_objects) {
    GameObject& obj = kv.second;
    if (obj.point_light == nullptr)
      continue;

    // calculate distance
    auto offset = frame_info.camera.GetPosition() - obj.transform.translation;
    float dis_squared = glm::dot(offset, offset);
    sorted[dis_squared] = obj.GetId();
  }

  pipeline_->Bind(frame_info.command_buffer);

  vkCmdBindDescriptorSets(
      frame_info.command_buffer,
      VK_PIPELINE_BIND_POINT_GRAPHICS,
      pipeline_layout_,
      0,
      1,
      &frame_info.global_descriptor_set,
      0,
      nullptr);

  // iterate over sorted lights in reverse order
  for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
    GameObject& obj = frame_info.game_objects.at(it->second);

    PointLightPushConstant push{};
    push.position = glm::vec4(obj.transform.translation, 1.0f);
    push.color = glm::vec4(obj.color, obj.point_light->light_intensity);
    push.radius = obj.transform.scale.x;  // radius is stored in scale.x

    vkCmdPushConstants(
        frame_info.command_buffer,
        pipeline_layout_,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(PointLightPushConstant),
        &push);

    vkCmdDraw(frame_info.command_buffer, 6, 1, 0, 0);
  }
}

}  // namespace heh
