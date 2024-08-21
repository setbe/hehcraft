#pragma once

// libs
#include <toml11/toml.hpp>
#include <glm/glm.hpp>

// std
#include <string>
#include <unordered_map>

namespace heh {

  namespace config {
    struct CameraConfig {
      float sensitivity{ 1.0f };    ///< The sensitivity of the camera.
      float fov{ 80.0f };           ///< The field of view of the camera.
    };

    struct WindowConfig {
      int width{ 800 };             ///< The width of the window.
      int height{ 600 };            ///< The height of the window.
      std::string window_name;      ///< The name of the window.
      bool fullscreen{ false };     ///< Whether the window is fullscreen or not.
    };

    struct BlockConfig {
      uint32_t id{0};
      std::string side;
      std::string top;
      std::string bottom;
      bool is_transparent{false};
    };

    struct TextureConfig {
      std::string name;
      glm::vec2 uvs[4]{};
    };

    struct Config {
      CameraConfig camera;
      WindowConfig window;
      std::unordered_map<std::string, BlockConfig> blocks;
      std::unordered_map<std::string, TextureConfig> textures;
    };

    extern Config file;
    extern std::string main_file_path;
    extern std::string blocks_file_path;
    extern std::string textures_file_path;

    void InitConfigFiles(
      const std::string& main_config, 
      const std::string& blocks_config, 
      const std::string& textures_config);

    void SaveMainConfig();
    void SaveBlocksConfig();
    void SaveTexturesConfig();

    void CreateDefaultMainConfig();
    void CreateDefaultBlocksConfig();
    void CreateDefaultTexturesConfig();

    glm::vec2 ParseVec2(const toml::value& v);
  } // namespace config 

} // namespace heh