#pragma once

#include "core/device.hpp"
#include "core/game_object.hpp"
#include "core/renderer.hpp"
#include "core/window.hpp"
#include "core/descriptors.hpp"

// std
#include <memory>
#include <vector>

namespace heh {
class App {
 public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  App();
  ~App();

  App(const App &) = delete;
  App &operator=(const App &) = delete;

  void Run();

 private:
  void LoadGameObjects();

  Window window_{WIDTH, HEIGHT, "Vulkan Tutorial"};
  Device device_{window_};
  Renderer renderer_{window_, device_};

  std::unique_ptr<DescriptorPool> global_pool_{};
  GameObject::Map game_objects_;
};
}  // namespace heh
