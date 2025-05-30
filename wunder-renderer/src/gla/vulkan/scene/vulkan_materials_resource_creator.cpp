#include "gla/vulkan/scene/vulkan_materials_resource_creator.h"

#include <functional>
#include <glm/ext/matrix_transform.hpp>
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

const assets<material_asset>&
materials_resource_creator::extract_material_assets(
    assets<mesh_asset>& mesh_assets) {
  auto& asset_manager = project::instance().get_asset_manager();

  std::unordered_set<asset_handle> material_ids;
  for (auto& [asset_id, mesh] : mesh_assets) {
    material_ids.emplace(mesh.get().m_material_handle);
  }

  material_assets.reserve(material_ids.size());

  if (material_ids.empty()) {
    material_assets.emplace_back(std::make_pair(
        asset_handle::invalid(), std::ref(get_default_material())));
  } else {
    auto found_assets = asset_manager.find_assets<material_asset>(
        material_ids.begin(), material_ids.end());
    std::move(found_assets.begin(), found_assets.end(),
              std::back_inserter(material_assets));
  }

  return material_assets;
}

unique_ptr<storage_buffer> materials_resource_creator::create_material_buffer(
    const assets<texture_asset>& texture_assets) {
  auto create_shader_material = [&texture_assets](
                                    const material_asset& asset,
                                    GltfShadeMaterial& out_shader_material) {
    auto get_texture_idx = [&texture_assets](asset_handle handle) {
      if (!handle) {
        WUNDER_DEBUG_TAG("vulkan_scene", "Handle is invalid skipping",
                         handle.value());
        return -1;
      }

      auto asset_it = texture_assets.find(handle);
      AssertReturnIf(asset_it == texture_assets.end(), -1);

      return static_cast<int>(std::distance(texture_assets.begin(), asset_it));
    };

    out_shader_material = asset;

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
    out_shader_material.specularTexture =
        get_texture_idx(asset.m_specular_texture);
    out_shader_material.specularColourTexture =
        get_texture_idx(asset.m_specular_colour_texture);
  };

  std::vector<GltfShadeMaterial> shader_materials;

  for (const auto& [handle, material_ref] : material_assets) {
    create_shader_material(material_ref.get(), shader_materials.emplace_back());
  }

  return std::move(std::make_unique<storage_device_buffer>(
      descriptor_build_data{.m_enabled = true,
                            .m_descriptor_name = "_MaterialBuffer"},
      shader_materials.data(),
      shader_materials.size() * sizeof(GltfShadeMaterial),
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT));
}

const material_asset& materials_resource_creator::get_default_material() {
  static material_asset default_material{
      .m_pbr_base_color_factor = {1.f, 1.f, 1.f, 1.f},
      .m_pbr_base_color_texture = asset_handle::invalid(),
      .m_pbr_metallic_factor = 0.f,
      .m_pbr_roughness_factor = 1.0f,
      .m_pbr_metallic_roughness_texture = asset_handle::invalid(),
      .m_emissive_texture = asset_handle::invalid(),
      .m_emissive_factor = glm::vec3{0.f},
      .m_alpha_mode = 0,
      .m_alpha_cutoff = 0.5f,
      .m_double_sided = 0,
      .m_normal_texture = asset_handle::invalid(),
      .m_normal_texture_scale = 0.f,
      .m_uv_transform = glm::identity<glm::mat4>(),
      .unlit = 0,
      .m_transmission_factor = 0.f,
      .m_transmission_texture = asset_handle::invalid(),
      .m_ior = 0,
      .m_anisotropy_direction = {0.f, 1.f, 0.f},
      .m_anisotropy = 0.f,
      .m_attenuation_color = {1.f, 1.f, 1.f},
      .m_thickness_factor = 0.f,
      .m_thickness_texture = asset_handle::invalid(),
      .m_attenuation_distance = std::numeric_limits<float>::max(),
      .m_clearcoat_factor = 0.f,
      .m_clearcoat_roughness = 0.f,
      .m_clearcoat_texture = asset_handle::invalid(),
      .m_clearcoat_roughness_texture = asset_handle::invalid(),
      .m_sheen = 0,
      .m_specular_texture = asset_handle::invalid(),
      .m_specular_colour_texture = asset_handle::invalid(),
      .m_specular_factor = 0.f,
      .m_specular_colour_factor = glm::vec3{0.f},
  };
  fill_undefined_material_colour(default_material.m_pbr_base_color_factor);
  return default_material;
}

}  // namespace wunder::vulkan