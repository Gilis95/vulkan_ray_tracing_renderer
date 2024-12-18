#include "include/assets/serializers/gltf/gltf_material_serializer.h"

#include "assets/asset_types.h"
#include "glm/vec4.hpp"
#include "include/assets/material_asset.h"
#include "tiny_gltf.h"
#include "tinygltf/tinygltf_utils.h"

namespace wunder {
static asset_handle find_texture_or_default(
    const std::unordered_map<std::uint32_t, asset_handle>& textures_map,
    uint32_t idx) {
  auto texture_handle_it = textures_map.find(idx);
  AssertReturnIf(texture_handle_it == textures_map.end(),
                 asset_handle::invalid());

  return texture_handle_it->second;
}

material_asset gltf_material_importer::process_material(
    const tinygltf::Material& gltf_material,
    const std::unordered_map<std::uint32_t, asset_handle>& textures_map) {
  auto emissive_texture_handle = find_texture_or_default(
      textures_map, gltf_material.emissiveTexture.index);

  auto normal_texture_handle =
      find_texture_or_default(textures_map, gltf_material.normalTexture.index);

  auto& tpbr = gltf_material.pbrMetallicRoughness;
  auto base_colour_texture_handle =
      find_texture_or_default(textures_map, tpbr.baseColorTexture.index);

  const KHR_materials_clearcoat& clearcoat =
      tinygltf::utils::get_clearcoat(gltf_material);
  auto clearcoat_roughness_texture_handle = find_texture_or_default(
      textures_map, clearcoat.m_roughness_texture.index);

  auto clearcoat_texture_handle =
      find_texture_or_default(textures_map, clearcoat.m_texture.index);

  const KHR_materials_transmission& transmission =
      tinygltf::utils::get_transmission(gltf_material);
  auto transmission_texture_handle =
      find_texture_or_default(textures_map, transmission.texture.index);

  const KHR_materials_volume& volume =
      tinygltf::utils::get_volume(gltf_material);
  auto volume_tickness_texture_handle =
      find_texture_or_default(textures_map, volume.m_thickness_texture.index);

  auto metallic_texture_handle = find_texture_or_default(
      textures_map, tpbr.metallicRoughnessTexture.index);

  material_asset mat{};

  mat.m_alpha_cutoff = static_cast<float>(gltf_material.alphaCutoff);
  mat.m_alpha_mode = gltf_material.alphaMode == "MASK"
                         ? 1
                         : (gltf_material.alphaMode == "BLEND" ? 2 : 0);
  mat.m_double_sided = gltf_material.doubleSided ? 1 : 0;
  mat.m_emissive_factor = gltf_material.emissiveFactor.size() == 3
                              ? glm::vec3(gltf_material.emissiveFactor[0],
                                          gltf_material.emissiveFactor[1],
                                          gltf_material.emissiveFactor[2])
                              : glm::vec3(0.f);
  mat.m_emissive_texture = emissive_texture_handle;
  mat.m_normal_texture = normal_texture_handle;
  mat.m_normal_texture_scale =
      static_cast<float>(gltf_material.normalTexture.scale);

  // PbrMetallicRoughness
  mat.m_pbr_base_color_factor =
      glm::vec4(tpbr.baseColorFactor[0], tpbr.baseColorFactor[1],
                tpbr.baseColorFactor[2], tpbr.baseColorFactor[3]);
  mat.m_pbr_base_color_texture = base_colour_texture_handle;
  mat.m_pbr_metallic_factor = static_cast<float>(tpbr.metallicFactor);
  mat.m_pbr_metallic_roughness_texture = metallic_texture_handle;
  mat.m_pbr_roughness_factor = static_cast<float>(tpbr.roughnessFactor);

  auto anistropy = tinygltf::utils::get_anisotropy(gltf_material);

  mat.unlit = tinygltf::utils::get_unlit(gltf_material).active;
  mat.m_anisotropy = anistropy.m_anisotropy_strength;
  mat.m_anisotropy_direction =
      glm::vec3(sin(anistropy.m_anisotropy_rotation),
                cos(anistropy.m_anisotropy_rotation), 0.f);

  mat.m_clearcoat_factor = clearcoat.m_factor;
  mat.m_clearcoat_roughness = clearcoat.m_roughness_factor;
  mat.m_clearcoat_roughness_texture = clearcoat_roughness_texture_handle;
  mat.m_clearcoat_texture = clearcoat_texture_handle;

  const KHR_materials_sheen& sheen = tinygltf::utils::get_sheen(gltf_material);
  mat.m_sheen = glm::packUnorm4x8(
      glm::vec4(sheen.m_sheen_color_factor, sheen.m_sheen_roughness_factor));
  mat.m_transmission_factor = transmission.factor;
  mat.m_transmission_texture = transmission_texture_handle;
  mat.m_ior = tinygltf::utils::get_ior(gltf_material).ior;

  mat.m_attenuation_color = volume.m_attenuation_color;
  mat.m_thickness_factor = volume.m_thickness_factor;
  mat.m_thickness_texture = volume_tickness_texture_handle;
  mat.m_attenuation_distance = volume.m_attenuation_distance;

  mat.m_emissive_factor =
      glm::make_vec3<double>(gltf_material.emissiveFactor.data());
  return mat;
}

}  // namespace wunder