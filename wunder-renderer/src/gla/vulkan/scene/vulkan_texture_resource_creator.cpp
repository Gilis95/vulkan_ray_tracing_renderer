#include "gla/vulkan/scene/vulkan_texture_resource_creator.h"

#include "assets/asset_manager.h"
#include "assets/asset_types.h"
#include "core/project.h"
#include "gla/vulkan/vulkan_texture.h"
#include "include/assets/material_asset.h"
#include "include/assets/texture_asset.h"

namespace wunder::vulkan {

texture_resource_creator::texture_resource_creator() = default;

std::vector<unique_ptr<sampled_texture>>
texture_resource_creator::create_texture_buffers(
    const assets<material_asset>& material_assets) {
  std::vector<unique_ptr<sampled_texture>> result;

  extract_texture_assets(material_assets);

  for (auto& [_, asset] : m_texture_assets) {
    auto& texture = result.emplace_back();
    texture.reset(new wunder::vulkan::sampled_texture(
        {.m_enabled = true, .m_descriptor_name = "texturesMap"}, asset.get()));
  }

  return result;
}

void texture_resource_creator::extract_texture_assets(
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

  m_texture_assets = asset_manager.find_assets<texture_asset>(
      texture_ids.begin(), texture_ids.end());
}

}  // namespace wunder::vulkan