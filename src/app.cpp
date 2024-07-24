#include "app.hpp"

#include "camera.hpp"
#include "simple_render_system.hpp"
#include "keyboard_controller.hpp"
#include "buffer.hpp"

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
#include <numeric>

namespace lve {

struct GlobalUbo {
  glm::mat4 viewProjMatrix;
  glm::vec3 lightDirection = glm::normalize(glm::vec3(1.f, -3.f, -1.f));
};

FirstApp::FirstApp() { loadGameObjects(); }

FirstApp::~FirstApp() {}

void FirstApp::run() {

  std::vector<std::unique_ptr<LveBuffer>> uboBuffers{LveSwapChain::MAX_FRAMES_IN_FLIGHT};

  for (int i = 0; i < uboBuffers.size(); i++)
  {
    uboBuffers[i] = std::make_unique<LveBuffer>(
      lveDevice, 
      sizeof(GlobalUbo), 
      1, 
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    );
    uboBuffers[i]->map();
  }

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
      int frameIndex = lveRenderer.getFrameIndex();

      FrameInfo frameInfo{
        frameIndex, 
        frameTime, 
        commandBuffer, 
        camera
      };

      // update
      GlobalUbo ubo{};
      ubo.viewProjMatrix = camera.getProjection() * camera.getView();
      uboBuffers[frameIndex]->writeToBuffer(&ubo);
      uboBuffers[frameIndex]->flush();

      // render
      lveRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
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
