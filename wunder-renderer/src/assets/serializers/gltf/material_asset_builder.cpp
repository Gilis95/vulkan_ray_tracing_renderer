#include "assets/asset_types.h"
#include "glm/vec4.hpp"
#include "include/assets/material_asset.h"
#include "include/assets/serializers/gltf/material_asset_builder.h"
#include "resources/shaders/host_device.h"
#include "tiny_gltf.h"
#include "tinygltf/tinygltf_utils.h"

namespace wunder {
static asset_handle find_texture_or_default(
    const std::unordered_map<std::uint32_t, asset_handle>& textures_map,
    int32_t idx) {
  if (idx < 0) {
    return asset_handle::invalid();
  }

  auto texture_handle_it = textures_map.find(idx);
  AssertReturnIf(texture_handle_it == textures_map.end(),
                 asset_handle::invalid());

  return texture_handle_it->second;
}

material_asset_builder::material_asset_builder(
    const tinygltf::Material& gltf_material,
    const std::unordered_map<std::uint32_t, asset_handle>& textures_map)
    : m_gltf_material(gltf_material), m_textures_map(textures_map) {}

material_asset material_asset_builder::build() {
  auto emissive_texture_handle = find_texture_or_default(
      m_textures_map, m_gltf_material.emissiveTexture.index);

  auto normal_texture_handle =
      find_texture_or_default(m_textures_map, m_gltf_material.normalTexture.index);

  auto& tpbr = m_gltf_material.pbrMetallicRoughness;
  auto base_colour_texture_handle =
      find_texture_or_default(m_textures_map, tpbr.baseColorTexture.index);

  const KHR_materials_clearcoat& clearcoat =
      tinygltf::utils::get_clearcoat(m_gltf_material);
  auto clearcoat_roughness_texture_handle = find_texture_or_default(
      m_textures_map, clearcoat.m_roughness_texture.index);

  auto clearcoat_texture_handle =
      find_texture_or_default(m_textures_map, clearcoat.m_texture.index);

  const KHR_materials_transmission& transmission =
      tinygltf::utils::get_transmission(m_gltf_material);
  auto transmission_texture_handle =
      find_texture_or_default(m_textures_map, transmission.texture.index);

  const KHR_materials_volume& volume =
      tinygltf::utils::get_volume(m_gltf_material);
  auto volume_tickness_texture_handle =
      find_texture_or_default(m_textures_map, volume.m_thickness_texture.index);

  std::optional<KHR_materials_specular> maybe_specular =
      tinygltf::utils::get_specular(m_gltf_material);

  auto metallic_texture_handle = find_texture_or_default(
      m_textures_map, tpbr.metallicRoughnessTexture.index);

  material_asset mat{};

  mat.m_alpha_cutoff = static_cast<float>(m_gltf_material.alphaCutoff);
  mat.m_alpha_mode = m_gltf_material.alphaMode == "MASK"
                         ? 1
                         : (m_gltf_material.alphaMode == "BLEND" ? 2 : 0);
  mat.m_double_sided = m_gltf_material.doubleSided ? 1 : 0;
  mat.m_emissive_factor = m_gltf_material.emissiveFactor.size() == 3
                              ? glm::vec3(m_gltf_material.emissiveFactor[0],
                                          m_gltf_material.emissiveFactor[1],
                                          m_gltf_material.emissiveFactor[2])
                              : glm::vec3(0.f);
  mat.m_emissive_texture = emissive_texture_handle;
  mat.m_normal_texture = normal_texture_handle;
  mat.m_normal_texture_scale =
      static_cast<float>(m_gltf_material.normalTexture.scale);

  mat.m_uv_transform = glm::identity<glm::mat4>();

  // PbrMetallicRoughness
  mat.m_pbr_base_color_factor =
      glm::vec4(tpbr.baseColorFactor[0], tpbr.baseColorFactor[1],
                tpbr.baseColorFactor[2], tpbr.baseColorFactor[3]);
  mat.m_pbr_base_color_texture = base_colour_texture_handle;
  mat.m_pbr_metallic_factor = static_cast<float>(tpbr.metallicFactor);
  mat.m_pbr_metallic_roughness_texture = metallic_texture_handle;
  mat.m_pbr_roughness_factor = static_cast<float>(tpbr.roughnessFactor);

  auto anistropy = tinygltf::utils::get_anisotropy(m_gltf_material);

  mat.unlit = tinygltf::utils::get_unlit(m_gltf_material).active;
  mat.m_anisotropy = anistropy.m_anisotropy_strength;
  mat.m_anisotropy_direction =
      glm::vec3(sin(anistropy.m_anisotropy_rotation),
                cos(anistropy.m_anisotropy_rotation), 0.f);

  mat.m_clearcoat_factor = clearcoat.m_factor;
  mat.m_clearcoat_roughness = clearcoat.m_roughness_factor;
  mat.m_clearcoat_roughness_texture = clearcoat_roughness_texture_handle;
  mat.m_clearcoat_texture = clearcoat_texture_handle;

  const KHR_materials_sheen& sheen = tinygltf::utils::get_sheen(m_gltf_material);
  mat.m_sheen = glm::packUnorm4x8(
      glm::vec4(sheen.m_sheen_color_factor, sheen.m_sheen_roughness_factor));
  mat.m_transmission_factor = transmission.factor;
  mat.m_transmission_texture = transmission_texture_handle;
  mat.m_ior = tinygltf::utils::get_ior(m_gltf_material).ior;

  mat.m_attenuation_color = volume.m_attenuation_color;
  mat.m_thickness_factor = volume.m_thickness_factor;
  mat.m_thickness_texture = volume_tickness_texture_handle;
  mat.m_attenuation_distance = volume.m_attenuation_distance;

  mat.m_emissive_factor =
      glm::make_vec3<double>(m_gltf_material.emissiveFactor.data());

  if (maybe_specular.has_value()) {
    auto& specular = maybe_specular.value();

    auto specular_colour_texture_handle = find_texture_or_default(
        m_textures_map, specular.m_specular_color_texture.index);
    auto specular_texture_handle = find_texture_or_default(
        m_textures_map, specular.m_specular_texture.index);

    mat.m_specular_texture = specular_texture_handle;
    mat.m_specular_colour_texture = specular_colour_texture_handle;
    mat.m_specular_factor = specular.m_specular_factor;
    mat.m_specular_colour_factor = specular.m_specular_color_factor;
  } else {
    mat.m_specular_texture = asset_handle::invalid();
    mat.m_specular_colour_texture = asset_handle::invalid();
    mat.m_specular_factor = 0.5;
    mat.m_specular_colour_factor = vec3(1.f);
  }

  return mat;
}

}  // namespace wunder