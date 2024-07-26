#pragma once

#include "core/camera.hpp"
#include "core/game_object.hpp"

// Standard library headers
#include <vulkan/vulkan.h>

namespace heh {

constexpr int kMaxLight = 10;

struct PointLight 
{
  glm::vec4 position{}; // ignore w
  glm::vec4 color{};  // w is intensity
};

struct GlobalUbo 
{
  glm::mat4 projetion{1.f};
  glm::mat4 view{1.f};
  glm::vec4 ambient_light_color{1.f, 1.f, 1.f, 0.02f};
  PointLight point_lights[kMaxLight];
  int light_count;
};

struct FrameInfo 
{
  int frame_index;
  float frame_time;
  VkCommandBuffer command_buffer;
  Camera &camera;
  VkDescriptorSet global_descriptor_set;
  GameObject::Map &game_objects;
};

}  // namespace heh
