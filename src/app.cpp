#include "app.hpp"

#include "camera.hpp"
#include "simple_render_system.hpp"
#include "keyboard_controller.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>
#include <chrono>

namespace lve {

FirstApp::FirstApp() { loadGameObjects(); }

FirstApp::~FirstApp() {}

void FirstApp::run() {
  SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.getSwapChainRenderPass()};
  Camera camera{};
  //camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));

  camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

  auto viewerObject = LveGameObject::createGameObject();
  KeyboardController cameraController{};

  auto currentTime = std::chrono::high_resolution_clock::now();

  while (!lveWindow.shouldClose()) {
    glfwPollEvents();

    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
    currentTime = newTime;

    cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), viewerObject, frameTime);
    camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

    float aspect = lveRenderer.getAspectRatio();
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 500.f);

    if (auto commandBuffer = lveRenderer.beginFrame()) {
      lveRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
      lveRenderer.endSwapChainRenderPass(commandBuffer);
      lveRenderer.endFrame();
    }
  }

  vkDeviceWaitIdle(lveDevice.device());
}

void FirstApp::loadGameObjects() {
  std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "models/smooth_vase.obj");
  LveGameObject smooth = LveGameObject::createGameObject();
  smooth.model = lveModel;
  smooth.transform.translation = {0.5f, 0.f, 2.5f};
  smooth.transform.scale = glm::vec3{3.f, 1.f, 3.f};

  lveModel = LveModel::createModelFromFile(lveDevice, "models/flat_vase.obj");
  LveGameObject flat = LveGameObject::createGameObject();
  flat.model = lveModel;
  flat.transform.translation = {-0.5f, 0.f, 2.5f};
  flat.transform.scale = glm::vec3{3.f, 1.f, 3.f};

  lveModel = LveModel::createModelFromFile(lveDevice, "models/colored_cube.obj");
  LveGameObject colored = LveGameObject::createGameObject();
  colored.model = lveModel;
  colored.transform.translation = {0.5f, 1.f, 2.5f};
  colored.transform.scale = glm::vec3{.1f, .1f, .1f};

  lveModel = LveModel::createModelFromFile(lveDevice, "models/cube.obj");
  LveGameObject cube = LveGameObject::createGameObject();
  cube.model = lveModel;
  cube.transform.translation = {-0.5f, -1.f, 2.5f};
  cube.transform.scale = glm::vec3{.1f, .1f, .1f};

  gameObjects.push_back(std::move(smooth));
  gameObjects.push_back(std::move(flat));
  gameObjects.push_back(std::move(colored));
  gameObjects.push_back(std::move(cube));
}

}  // namespace lve
