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
#include <thread>

namespace heh {

  using NumChunkInt = uint16_t;
  /* 
  Perfect square roots 
    - from 4 to 196:
        4 (2x2 chunks), 9 (3x3), 16 (4x4), ..., 64 (8x8), ..., 196 (14x14)
  
    - from 1024 to 24025:
        1024  (32x32 chunks), 2025, 3025, 
        4096  (64x64 chunks), 4225, 5625, 7225, 9025, 11025, 13225,            
        16384 (128x128 chunks), 18225, 21025, 24025
  */
  constexpr NumChunkInt kNumChunks = 64;
  
  static constexpr NumChunkInt kChunksPerSide = heh::math::SqrtInt(kNumChunks);
  static constexpr NumChunkInt kHalfChunks = kChunksPerSide / 2;

  class Window; // forward declaration (friend class)

  class World {
    friend class Window;

    uint32_t seed_;
    std::unique_ptr<Chunk[]>chunks_;

    Camera::Data &camera_data_;
    Shader shader_;
    ImageWriter atlas_writer_;

    glm::mat4 model_{1.0f};
    
   public:
    World(Camera::Data &camera_data);
    ~World();

    void Init();
    uint32_t GetSeed() const { return seed_; }

    void Render();
  };
  

}  // namespace heh