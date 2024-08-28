#ifndef WUNDER_GLTF_TEXTURE_SERIALIZER_H
#define WUNDER_GLTF_TEXTURE_SERIALIZER_H

#include <vector>

namespace tinygltf {
class Model;
}

namespace wunder {
struct texture_component;
namespace gltf_texture_serializer {
[[nodiscard]] std::vector<texture_component> process_textures(
    tinygltf::Model& gltf_scene_root);
}
}  // namespace wunder
#endif  // WUNDER_GLTF_TEXTURE_SERIALIZER_H
