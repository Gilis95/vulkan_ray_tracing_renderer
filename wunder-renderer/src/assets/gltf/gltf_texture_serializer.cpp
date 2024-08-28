#include "assets/gltf/gltf_texture_serializer.h"

#include <tiny_gltf.h>

#include "assets/components/texture_component.h"
#include "core/wunder_macros.h"

namespace wunder {
namespace {
std::unordered_map<int, texture_filter_type> s_gltf_filter_type_to_internal{
    {9728, texture_filter_type::NEAREST},  // NEAREST
    {9729, texture_filter_type::LINEAR},   // LINEAR
    {9984, texture_filter_type::NEAREST},  // NEAREST_MIPMAP_NEAREST
    {9985, texture_filter_type::LINEAR},   // LINEAR_MIPMAP_NEAREST
    {9986, texture_filter_type::NEAREST},  // NEAREST_MIPMAP_LINEAR
    {9987, texture_filter_type::LINEAR}    // LINEAR_MIPMAP_LINEAR
};
std::unordered_map<int, mipmap_mode_type> s_gltf_mipmap_type_to_internal{
    {9728, mipmap_mode_type::NEAREST},  // NEAREST
    {9729, mipmap_mode_type::NEAREST},  // LINEAR
    {9984, mipmap_mode_type::NEAREST},  // NEAREST_MIPMAP_NEAREST
    {9985, mipmap_mode_type::NEAREST},  // LINEAR_MIPMAP_NEAREST
    {9986, mipmap_mode_type::LINEAR},   // NEAREST_MIPMAP_LINEAR
    {9987, mipmap_mode_type::LINEAR},   // LINEAR_MIPMAP_LINEAR
};

std::unordered_map<int, address_mode_type> s_gltf_address_mode_to_internal{
    {33071, address_mode_type::CLAMP_TO_EDGE},
    {33648, address_mode_type::MIRRORED_REPEAT},
    {10497, address_mode_type::REPEAT}
};

}  // namespace

namespace gltf_texture_serializer {
std::vector<texture_component> process_textures(
    tinygltf::Model& gltf_scene_root) {
  std::vector<texture_component> result;

  for (auto& gltf_texture : gltf_scene_root.textures) {
    int gltf_source_image_idx = gltf_texture.source;
    AssertContinueUnless(gltf_source_image_idx < gltf_scene_root.images.size());

    auto& gltf_source_image = gltf_scene_root.images[gltf_source_image_idx];
    texture_component texture{.m_texture_data =
                                  std::move(gltf_source_image.image)};

    if (gltf_texture.sampler > -1) {
      const auto& gltf_sampler = gltf_scene_root.samplers[gltf_texture.sampler];

      texture.m_mag_filter  = s_gltf_filter_type_to_internal[gltf_sampler.magFilter];
      texture.m_min_filter  = s_gltf_filter_type_to_internal[gltf_sampler.minFilter];
      texture.m_mipmap_mode = s_gltf_mipmap_type_to_internal[gltf_sampler.minFilter];
      texture.m_address_mode_u = s_gltf_address_mode_to_internal[gltf_sampler.wrapS];
      texture.m_address_mode_v = s_gltf_address_mode_to_internal[gltf_sampler.wrapT];

    }

    result.push_back(std::move(texture));
  }

  return result;
}

}  // namespace gltf_texture_serializer
}  // namespace wunder