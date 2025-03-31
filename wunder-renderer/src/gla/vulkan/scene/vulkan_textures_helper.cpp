#include "gla/vulkan/scene/vulkan_textures_helper.h"

#include "assets/asset_manager.h"
#include "assets/asset_types.h"
#include "core/project.h"
#include "gla/vulkan/vulkan_texture.h"
#include "include/assets/material_asset.h"
#include "include/assets/texture_asset.h"

namespace wunder::vulkan {

assets<texture_asset> textures_helper::extract_texture_assets(
    const assets<material_asset>& material_assets) {
  auto& asset_manager = project::instance().get_asset_manager();

  std::unordered_set<asset_handle> texture_ids;

  for (auto& [asset_id, material] : material_assets) {
    const auto& material_asset = material.get();
    texture_ids.emplace(material_asset.m_pbr_base_color_texture);
    texture_ids.emplace(material_asset.m_pbr_metallic_roughness_texture);
    texture_ids.emplace(material_asset.m_emissive_texture);
    texture_ids.emplace(material_asset.m_normal_texture);
    texture_ids.emplace(material_asset.m_transmission_texture);
    texture_ids.emplace(material_asset.m_thickness_texture);
    texture_ids.emplace(material_asset.m_clearcoat_texture);
    texture_ids.emplace(material_asset.m_clearcoat_roughness_texture);
    texture_ids.emplace(material_asset.m_specular_texture);
    texture_ids.emplace(material_asset.m_specular_colour_texture);
  }

  vector_map<asset_handle, std::reference_wrapper<const texture_asset>> result =
      asset_manager.find_assets<texture_asset>(texture_ids.begin(),
                                               texture_ids.end());

  return result;
}

std::vector<unique_ptr<sampled_texture>>
textures_helper::create_texture_buffers(
    const assets<texture_asset>& texture_assets) {
  std::vector<unique_ptr<sampled_texture>> result;

  for (auto& [_, asset] : texture_assets) {
    auto& texture = result.emplace_back();
    texture.reset(new wunder::vulkan::sampled_texture(
        {.m_enabled = true, .m_descriptor_name = "texturesMap"}, asset.get()));
  }

  return std::move(result);
}
}  // namespace wunder::vulkan