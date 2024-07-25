#pragma once

#include "camera.hpp"
#include "game_object.hpp"

// Standard library headers
#include <vulkan/vulkan.h>

namespace heh {

struct FrameInfo {
  int frame_index;
  float frame_time;
  VkCommandBuffer command_buffer;
  Camera &camera;
  VkDescriptorSet global_descriptor_set;
  GameObject::Map &game_objects;
};

}  // namespace heh
