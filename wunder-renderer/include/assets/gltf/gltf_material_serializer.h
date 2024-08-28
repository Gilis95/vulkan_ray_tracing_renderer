//
// Created by christian on 8/28/24.
//

#ifndef WUNDER_GLTF_MATERIAL_SERIALIZER_H
#define WUNDER_GLTF_MATERIAL_SERIALIZER_H
namespace tinygltf {
struct Material;
}

namespace wunder {
struct material_component;
namespace gltf_material_serializer {
material_component process_material(const tinygltf::Material& gltf_material);
}
}  // namespace wunder
#endif  // WUNDER_GLTF_MATERIAL_SERIALIZER_H
