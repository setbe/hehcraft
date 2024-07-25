#pragma once

#include "device.hpp"
#include "game_object.hpp"
#include "renderer.hpp"
#include "window.hpp"
#include "descriptors.hpp"

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
