#include "core/block.hpp"

namespace heh {

std::string GrassTexture::GetFullPath(Face face, const std::string &tex_name) const {
  switch (face) {
    case Face::kTop:
      return GetBasePath(tex_name) + "top.png";
    case Face::kBottom:
      return GetBasePath(tex_name) + "bottom.png";
    default:
      // For left, right, front, and back faces
      return GetBasePath(tex_name) + "side.png";
  }
}

std::string CobblestoneTexture::GetFullPath(Face face, const std::string &tex_name) const {
  return GetBasePath(tex_name) + "side.png";
}
} // namespace heh