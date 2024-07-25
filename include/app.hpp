#pragma once

#include "device.hpp"
#include "game_object.hpp"
#include "renderer.hpp"
#include "window.hpp"
#include "descriptors.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
class FirstApp {
 public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  FirstApp();
  ~FirstApp();

  FirstApp(const FirstApp &) = delete;
  FirstApp &operator=(const FirstApp &) = delete;

  void run();

 private:
  void loadGameObjects();

  LveWindow lveWindow{WIDTH, HEIGHT, "Vulkan Tutorial"};
  LveDevice lveDevice{lveWindow};
  LveRenderer lveRenderer{lveWindow, lveDevice};

  std::unique_ptr<LveDescriptorPool> globalPull{};
  LveGameObject::Map gameObjects;
};
}  // namespace lve
