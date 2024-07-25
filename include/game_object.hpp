#pragma once

#include "model.hpp"

// Standard library headers
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <unordered_map>

namespace heh {

struct TransformComponent {
  glm::vec3 translation{};  // Position offset
  glm::vec3 scale{1.f, 1.f, 1.f};  // Scale factors
  glm::vec3 rotation{};  // Tait-Bryan angles (YXZ order)

  // Matrix corresponds to Translate * Ry * Rx * Rz * Scale
  // Rotations correspond to Tait-Bryan angles of Y (1), X (2), Z (3)
  // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
  glm::mat4 Mat4();
  glm::mat3 NormalMatrix();
};

class GameObject {
 public:
  using IdType = unsigned int;
  using Map = std::unordered_map<IdType, GameObject>;

  static GameObject Create() {
    static IdType current_id = 0;
    return GameObject{current_id++};
  }

  GameObject(const GameObject &) = delete;
  GameObject &operator=(const GameObject &) = delete;
  GameObject(GameObject &&) = default;
  GameObject &operator=(GameObject &&) = default;

  IdType GetId() const { return id_; }

  std::shared_ptr<Model> model{};
  glm::vec3 color{};
  TransformComponent transform{};

 private:
  explicit GameObject(IdType obj_id) : id_{obj_id} {}

  IdType id_;
};

}  // namespace heh
