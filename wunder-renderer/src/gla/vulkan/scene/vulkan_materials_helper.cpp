#include "gla/vulkan/scene/vulkan_materials_helper.h"

#include <functional>
#include <glm/vec4.hpp>
#include <numeric>
#include <unordered_set>

#include "assets/asset_manager.h"
#include "core/project.h"
#include "core/vector_map.h"
#include "gla/vulkan/vulkan_device_buffer.h"
#include "resources/shaders/host_device.h"

namespace {
void fill_undefined_material_colour(glm::vec4& out_colour) {
  out_colour.r = 0.6f;
  out_colour.g = .0f;
  out_colour.b = 0.9f;
  out_colour.a = 1.0f;
}
}  // namespace

namespace wunder::vulkan {

assets<material_asset> materials_helper::extract_material_assets(
    assets<mesh_asset>& mesh_assets) {
  auto& asset_manager = project::instance().get_asset_manager();

  std::unordered_set<asset_handle> material_ids;
  for (auto& [asset_id, mesh] : mesh_assets) {
    material_ids.emplace(mesh.get().m_material_handle);
  }

  assets<material_asset> result;
  result.reserve(material_ids.size() + 1);
  result.emplace_back(std::make_pair(asset_handle::invalid(),
                                     std::ref(get_default_material())));

  if (!material_ids.empty()) {
    auto found_assets = asset_manager.find_assets<material_asset>(
        material_ids.begin(), material_ids.end());
    std::move(found_assets.begin(), found_assets.end(),
              std::back_inserter(result));
  }

  return result;
}

unique_ptr<buffer> materials_helper::create_material_buffer(
    const assets<material_asset>& materials,
    const assets<texture_asset>& texture_assets) {
  auto create_shader_material = [&texture_assets](
                                    const material_asset& asset,
                                    GltfShadeMaterial& out_shader_material) {
    auto get_texture_idx = [&texture_assets](asset_handle handle) {
      auto asset_it = texture_assets.find(handle);
      if (asset_it == texture_assets.end()) {
        WUNDER_DEBUG_TAG("vulkan_scene", "Failed to find texture asset {0}",
                         handle.value());
        return -1;
      }

      return static_cast<int>(std::distance(texture_assets.begin(), asset_it));
    };

    out_shader_material.pbrBaseColorTexture =
        get_texture_idx(asset.m_pbr_base_color_texture);
    out_shader_material.pbrMetallicRoughnessTexture =
        get_texture_idx(asset.m_pbr_metallic_roughness_texture);
    out_shader_material.emissiveTexture =
        get_texture_idx(asset.m_emissive_texture);
    out_shader_material.normalTexture = get_texture_idx(asset.m_normal_texture);
    out_shader_material.transmissionTexture =
        get_texture_idx(asset.m_transmission_texture);
    out_shader_material.thicknessTexture =
        get_texture_idx(asset.m_thickness_texture);
    out_shader_material.clearcoatTexture =
        get_texture_idx(asset.m_clearcoat_texture);
    out_shader_material.clearcoatRoughnessTexture =
        get_texture_idx(asset.m_clearcoat_roughness_texture);
  };

  std::vector<GltfShadeMaterial> shader_materials;

  for (const auto& [handle, material_ref] : materials) {
    create_shader_material(material_ref.get(), shader_materials.emplace_back());
  }

  return std::make_unique<device_buffer>(
      buffer::descriptor_build_data{.m_enabled=true, .m_descriptor_name = "materials"},
      shader_materials.data(),
      shader_materials.size() * sizeof(GltfShadeMaterial),
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
}

const material_asset& materials_helper::get_default_material() {
  static material_asset default_material{
      .m_pbr_base_color_texture = asset_handle::invalid(),
      .m_pbr_metallic_roughness_texture = asset_handle::invalid(),
      .m_emissive_texture = asset_handle::invalid(),
      .m_normal_texture = asset_handle::invalid(),
      .m_transmission_texture = asset_handle::invalid(),
      .m_thickness_texture = asset_handle::invalid(),
      .m_clearcoat_texture = asset_handle::invalid(),
      .m_clearcoat_roughness_texture = asset_handle::invalid()};
  fill_undefined_material_colour(default_material.m_pbr_base_color_factor);
  return default_material;
}

}  // namespace wunder::vulkan