#include "core/app.hpp"

#include "core/camera.hpp"
#include "core/keyboard_controller.hpp"
#include "core/buffer.hpp"

#include "core/systems/simple_render.hpp"
#include "core/systems/point_light.hpp"

#include "core/texture.hpp"

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

namespace heh {

App::App() 
{
  global_pool_ = DescriptorPool::Builder(device_)
    .SetMaxSets(SwapChain::kMaxFramesInFlight)
    .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::kMaxFramesInFlight)
    .AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::kMaxFramesInFlight)
    .Build();

  LoadGameObjects();
}

App::~App() {}

void App::Run() 
{
  std::vector<std::unique_ptr<Buffer>> ubo_buffers(SwapChain::kMaxFramesInFlight);

  // Initialize UBO buffers
  for (int i = 0; i < ubo_buffers.size(); ++i) {
    ubo_buffers[i] = std::make_unique<Buffer>(
      device_,
      sizeof(GlobalUbo),
      1,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    );
    ubo_buffers[i]->Map();
  }

  auto global_set_layout = DescriptorSetLayout::Builder(device_)
    .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
    .AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
    .Build();

  VkDescriptorImageInfo image_info{};
  image_info = game_objects_.at(0).model->GetTextureDescriptorInfo();


  // Initialize descriptor sets
  std::vector<VkDescriptorSet> global_descriptor_sets(SwapChain::kMaxFramesInFlight);
  for (int i = 0; i < global_descriptor_sets.size(); ++i) 
  {
    auto buffer_info = ubo_buffers[i]->DescriptorInfo();
    DescriptorWriter(*global_set_layout, *global_pool_)
      .WriteBuffer(0, &buffer_info)
      .WriteImage(1, &image_info)
      .Build(global_descriptor_sets[i]);
  }


  SimpleRenderSystem simple_render_system{
    device_, 
    renderer_.GetSwapChainRenderPass(), 
    global_set_layout->GetDescriptorSetLayout()};

  PointLightSystem point_light_system{
    device_, 
    renderer_.GetSwapChainRenderPass(), 
    global_set_layout->GetDescriptorSetLayout()};
  
  Camera camera;
  camera.SetViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

  auto viewer_object = GameObject::Create();
  viewer_object.transform.translation.z = -3.f;
  KeyboardController camera_controller;

  auto current_time = std::chrono::high_resolution_clock::now();

  while (!window_.ShouldClose()) {
    glfwPollEvents();

    auto new_time = std::chrono::high_resolution_clock::now();
    float frame_time = std::chrono::duration<float>(new_time - current_time).count();
    current_time = new_time;

    camera_controller.MoveInPlaneXZ(window_.GetGLFWwindow(), viewer_object, frame_time);
    camera_controller.Look(window_.GetGLFWwindow(), viewer_object, frame_time);
    camera_controller.UpdateCursorState(window_.GetGLFWwindow());
    camera.SetViewYXZ(viewer_object.transform.translation, viewer_object.transform.rotation);

    float aspect = renderer_.GetAspectRatio();
    camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 500.f);

    if (auto command_buffer = renderer_.BeginFrame()) {
      int frame_index = renderer_.GetFrameIndex();

      FrameInfo frame_info{
        frame_index,
        frame_time,
        command_buffer,
        camera,
        global_descriptor_sets[frame_index],
        game_objects_
      };

      // Update UBO
      GlobalUbo ubo{};
      ubo.projetion = camera.GetProjection();
      ubo.view = camera.GetView();
      ubo.inverse_view = camera.GetInverseView();

      point_light_system.Update(frame_info, ubo);
      ubo_buffers[frame_index]->WriteToBuffer(&ubo);
      ubo_buffers[frame_index]->Flush();

      // Render
      renderer_.BeginSwapChainRenderPass(command_buffer);

      // Render game objects
      simple_render_system.RenderGameObjects(frame_info);
      point_light_system.Render(frame_info);
      
      renderer_.EndSwapChainRenderPass(command_buffer);
      renderer_.EndFrame();
    }
  }

  vkDeviceWaitIdle(device_.GetDevice());
}

void App::LoadGameObjects() 
{
  // std::shared_ptr<Model> model = Model::CreateModel(device_, "models/smooth_vase");
  // GameObject smooth = GameObject::Create();
  // smooth.model = model;
  // smooth.transform.translation = {0.5f, 0.f, -2.5f};
  // smooth.transform.scale = glm::vec3{1.f, 1.f, 1.f};
  // game_objects_.emplace(smooth.GetId(), std::move(smooth));

  // model = Model::CreateModel(device_, "models/flat_vase");
  // GameObject flat = GameObject::Create();
  // flat.model = model;
  // flat.transform.translation = {-0.5f, 0.f, -1.f};
  // flat.transform.scale = glm::vec3{1.f, 1.f, 1.f};
  // game_objects_.emplace(flat.GetId(), std::move(flat));

  // model = Model::CreateModel(device_, "models/cube");
  // GameObject cube = GameObject::Create();
  // cube.model = model;
  // cube.transform.translation = {-1.5f, -0.2f, 0.f};
  // cube.transform.scale = glm::vec3{.2f};
  // game_objects_.emplace(cube.GetId(), std::move(cube));

  // model = Model::CreateModel(device_, "models/quad");
  // GameObject floor = GameObject::Create();
  // floor.model = model;
  // floor.transform.translation = {0.f, 0.f, 0.f};
  // floor.transform.scale = glm::vec3{3.f, 1.f, 3.f};
  // game_objects_.emplace(floor.GetId(), std::move(floor));

  std::shared_ptr<Model> model = Model::CreateModel(device_, "grass");
  GameObject grass = GameObject::Create();
  grass.model = model;
  grass.transform.translation = {-1.5f, -.5f, -1.f};
  grass.transform.scale = glm::vec3{.15f, .15f, .15f};
  game_objects_.emplace(grass.GetId(), std::move(grass));

  for (int i = 0; i < 10; i++) {
    model = Model::CreateModel(device_, "grass");
    GameObject cube = GameObject::Create();
    cube.model = model;
    cube.transform.translation = {i * 0.5f, 0.f, 0.f};
    cube.transform.scale = glm::vec3{.2f};
    game_objects_.emplace(cube.GetId(), std::move(cube));
  }

  std::vector<glm::vec3> lightColors{
    {1.f, .1f, .1f},
    {.1f, .1f, 1.f},
    {.1f, 1.f, .1f},
    {1.f, 1.f, .1f},
    {.1f, 1.f, 1.f},
    {1.f, 1.f, 1.f}  //
  };

  for (int i = 0; i < lightColors.size(); i++) {
    GameObject light = GameObject::MakePointLight(.4f);
    light.color = lightColors[i];
    auto rotate_light = glm::rotate(
      glm::mat4(1.f), 
      (i * glm::two_pi<float>()) / lightColors.size(), 
      glm::vec3(0.f, -1.f, 0.f));

    light.transform.translation = glm::vec3(rotate_light * glm::vec4(-1.f, -1.f, -1.f, 1.f));
    game_objects_.emplace(light.GetId(), std::move(light));
  }

}

}  // namespace heh
