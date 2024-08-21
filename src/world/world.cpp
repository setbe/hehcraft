#include "world/world.hpp"


// std
#include <type_traits>

namespace heh {

  ImageWriter World::atlas_writer_;
  
  World::World(Camera::Data& camera_data)
    : shader_{ "shaders/specular.vert", "shaders/specular.frag" },
    camera_data_{ camera_data }
  {
    // Generate seed
    {
      long long time_count = std::chrono::high_resolution_clock::now().time_since_epoch().count();
      std::mt19937 rng(time_count);
    
      static_assert(std::is_same<uint32_t, decltype(seed_)>::value, "seed_ is not of type uint32_t");

      std::uniform_int_distribution<uint32_t> distribution(0, UINT32_MAX);
      seed_ = distribution(rng);
      std::cout << "Seed: " << seed_ << std::endl;
    }

    // Create atlas
    atlas_writer_.CreateAtlas("textures", "atlas.png");
    assert(atlas_writer_.GetAtlasSize() == kAtlasSize && "kAtlasSize must be updated");
    

    // Generate chunks
    chunks_ = std::make_unique<Chunk[]>(kNumChunks);
    NumChunkInt chunk_index = 0;
    for (int32_t z = -kHalfChunks; z < kHalfChunks; ++z)
    {
      for (int32_t x = -kHalfChunks; x < kHalfChunks; ++x)
      {
        //chunks_[chunk_index].Deserialize("large_world", x, z);
        chunks_[chunk_index].Generate(seed_, x, z);
        chunks_[chunk_index].BuildMesh();
        //chunks_[chunk_index].Serialize("large_world");
        chunk_index++;
      }
    }
  }

  void World::Init()
  {
    shader_.Use();
    shader_.SetInt("texture1", 0);
    atlas_writer_.BindAtlas();
  }

  World::~World() {

  }

  void World::Render() {
    shader_.Use();
    shader_.SetMat4("view", camera_data_.view);
    shader_.SetMat4("projection", camera_data_.projection);
    shader_.SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    shader_.SetVec3("lightPos", glm::vec3(-4.2f, 300.0f, -2.0f));
    shader_.SetVec3("viewPos", camera_data_.camera_pos);

    shader_.SetVec3("dirLightDirection", glm::vec3(0.2f, 0.7f, -0.6f));
    shader_.SetVec3("dirLightColor", glm::vec3(1.0f, 0.95f, 0.85f));

    shader_.SetMat4("model", model_);

    for (int i = 0; i < kNumChunks; ++i) {
      chunks_[i].Render();
    }
  }

} // namespace heh