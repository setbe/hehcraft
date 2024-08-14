#pragma once

#include "world/chunk.hpp"
#include "core/shader.hpp"
#include "core/camera.hpp"

#include "utils/math.hpp"

// std
#include <random>
#include <chrono>
#include <cstdint>
#include <array>
#include <memory>

namespace heh {
  /* Check if kNumChunks is a perfect square to determine chunk grid size.
     Perfect squares up to 100: 1, 4, 9, 16, 25, 36, 49, 64, 81, 100.    
     100, 121, 144, 169, 196, 225, 256, ..., 1024 */
  constexpr int kNumChunks = 1024;

  static constexpr int kChunksPerSide = SqrtInt(kNumChunks);
  static constexpr int kHalfChunks = kChunksPerSide / 2;

  class Window; // forward declaration (friend class)

  class World {
    friend class Window;

    uint32_t seed_;
    std::unique_ptr<Chunk[]>chunks_;

    Camera::Data &camera_data_;
    Shader shader_;
    ImageWriter atlas_writer_;
    
   public:
    World(Camera::Data &camera_data);
    ~World();

    void Init();
    uint32_t GetSeed() const { return seed_; }

    void Render();
  };
  

}  // namespace heh