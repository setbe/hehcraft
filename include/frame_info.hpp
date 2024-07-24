#pragma once

#include "camera.hpp"

// libs
#include <vulkan/vulkan.h>

namespace lve {

struct FrameInfo {
  int frameIndex;
  float frameTime;
  VkCommandBuffer commandBuffer;
  Camera &camera;
};

}; // namespace lve