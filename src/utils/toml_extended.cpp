#include "utils/toml_extended.hpp"

// libs
#include <toml11/toml.hpp>
#include <glm/glm.hpp>

// std
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <iomanip>


namespace heh {
  namespace config {
    std::string main_file_path = "config.toml";
    std::string blocks_file_path = "blocks.toml";
    std::string textures_file_path = "textures.toml";
    Config file = LoadConfig();

    Config LoadConfig()
    {
      Config config;

      // Load main config
      std::ifstream main_file(main_file_path);
      if (!main_file) {
        CreateDefaultMainConfig();
        std::cerr << "Default main config file created at: " << main_file_path << std::endl;
      }

      try {
        const auto main_data = toml::parse(main_file_path);

        // Load camera config
        const auto camera = toml::find(main_data, "camera");
        config.camera.sensitivity = toml::find<float>(camera, "sensitivity");
        config.camera.fov = toml::find<float>(camera, "fov");

        // Load window config
        const auto window = toml::find(main_data, "window");
        config.window.width = toml::find<int>(window, "width");
        config.window.height = toml::find<int>(window, "height");
        config.window.window_name = toml::find<std::string>(window, "window_name");
        config.window.fullscreen = toml::find<bool>(window, "fullscreen");
      }
      catch (const std::exception& e) {
        throw std::runtime_error(std::string("Error parsing main TOML file: ") + e.what());
      }

      // Load blocks config
      std::ifstream blocks_file(blocks_file_path);
      if (!blocks_file) {
        CreateDefaultBlocksConfig();
        std::cerr << "Default blocks config file created at: " << blocks_file_path << std::endl;
      }

      try {
        const auto blocks_data = toml::parse(blocks_file_path);

        // Load block config
        const auto blocks = toml::find(blocks_data, "blocks");
        for (const auto& block : blocks.as_array()) {
          BlockConfig block_config;
          block_config.side_texture = toml::find<std::string>(block, "side_texture");
          block_config.top_texture = toml::find<std::string>(block, "top_texture");
          block_config.bottom_texture = toml::find<std::string>(block, "bottom_texture");
          config.blocks[toml::find<std::string>(block, "name")] = block_config;
        }
      }
      catch (const std::exception& e) {
        throw std::runtime_error(std::string("Error parsing blocks TOML file: ") + e.what());
      }

      // Load textures config
      std::ifstream textures_file(textures_file_path);
      if (!textures_file) {
        CreateDefaultTexturesConfig();
        std::cerr << "Default textures config file created at: " << textures_file_path << std::endl;
      }

      try {
        const auto textures_data = toml::parse(textures_file_path);

        // Load texture config
        const auto textures = toml::find(textures_data, "textures");
        for (const auto& texture : textures.as_array()) {
          TextureConfig texture_config;
          texture_config.name = toml::find<std::string>(texture, "name");
          const auto uvs = toml::find(texture, "uvs").as_array();
          for (size_t i = 0; i < uvs.size(); ++i) {
            texture_config.uvs[i] = ParseVec2(uvs[i]);
          }
          config.textures[texture_config.name] = texture_config;
        }
      }
      catch (const std::exception& e) {
        throw std::runtime_error(std::string("Error parsing textures TOML file: ") + e.what());
      }

      return config;
    }

    glm::vec2 ParseVec2(const toml::value& v) {
      const auto arr = toml::get<std::vector<float>>(v);
      return glm::vec2(arr[0], arr[1]);
    }

    void SaveMainConfig() {
      std::ofstream out(main_file_path);
      if (!out)
        throw std::runtime_error("Failed to open file for writing: " + main_file_path);

      out << "# Camera configuration\n";
      out << "[camera]\n";
      out << std::fixed << std::setprecision(2) << "sensitivity = " << file.camera.sensitivity << "\n";
      out << std::fixed << std::setprecision(1) << "fov = " << file.camera.fov << "\n";
      out << "\n";
      out << "# Window configuration\n";
      out << "[window]\n";
      out << "width = " << file.window.width << "\n";
      out << "height = " << file.window.height << "\n";
      out << "window_name = \"" << file.window.window_name << "\"\n";
      out << "fullscreen = " << (file.window.fullscreen ? "true" : "false") << "\n";
    }

    void CreateDefaultMainConfig() {
      std::ofstream out(main_file_path);
      if (!out)
        throw std::runtime_error("Failed to open file for writing: " + main_file_path);

      out << R"(
# Camera configuration
[camera]
sensitivity = 0.2
fov = 80.0

# Window configuration
[window]
width = 800
height = 600
window_name = "Hehcraft"
fullscreen = false
)";
    }

    void CreateDefaultBlocksConfig() {
      std::ofstream out(blocks_file_path);
      if (!out)
        throw std::runtime_error("Failed to open file for writing: " + blocks_file_path);

      out << R"(
# Blocks configuration
[[blocks]]
name = "cobblestone"
side_texture = "cobblestone.png"
top_texture = "cobblestone.png"
bottom_texture = "cobblestone.png"
)";
    }

    void CreateDefaultTexturesConfig() {
      std::ofstream out(textures_file_path);
      if (!out)
        throw std::runtime_error("Failed to open file for writing: " + textures_file_path);

      out << R"(
# Textures configuration
[[textures]]
name = "cobblestone"
uvs = [[0.0, 0.0], [1.0, 0.0], [1.0, 1.0], [0.0, 1.0]]
)";
    }

  } // namespace config

} // namespace heh