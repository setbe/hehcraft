#pragma once

#include "camera.hpp"
#include "game_object.hpp"

// libs
#include <vulkan/vulkan.h>

namespace lve {

struct FrameInfo {
  int frameIndex;
  float frameTime;
  VkCommandBuffer commandBuffer;
  Camera &camera;
  VkDescriptorSet globalDescriptorSet;
  LveGameObject::Map &gameObjects;
};

}; // namespace lve