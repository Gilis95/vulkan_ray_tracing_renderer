#include "include/assets/gltf/gltf_material_serializer.h"

#include <tiny_gltf.h>

#include "assets/components/material_component.h"
#include "glm/vec4.hpp"
#include "tinygltf/tinygltf_utils.h"

namespace wunder::gltf_material_serializer {
material_component process_material(const tinygltf::Material& gltf_material) {
  material_component mat{};

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
  mat.m_emissive_texture = gltf_material.emissiveTexture.index;
  mat.m_normal_texture = gltf_material.normalTexture.index;
  mat.m_normal_texture_scale =
      static_cast<float>(gltf_material.normalTexture.scale);

  // PbrMetallicRoughness
  auto& tpbr = gltf_material.pbrMetallicRoughness;
  mat.m_pbr_base_color_factor =
      glm::vec4(tpbr.baseColorFactor[0], tpbr.baseColorFactor[1],
                tpbr.baseColorFactor[2], tpbr.baseColorFactor[3]);
  mat.m_pbr_base_color_texture = tpbr.baseColorTexture.index;
  mat.m_pbr_metallic_factor = static_cast<float>(tpbr.metallicFactor);
  mat.m_pbr_metallic_roughness_texture = tpbr.metallicRoughnessTexture.index;
  mat.m_pbr_roughness_factor = static_cast<float>(tpbr.roughnessFactor);

  auto anistropy = tinygltf::utils::get_anisotropy(gltf_material);

  mat.unlit = tinygltf::utils::get_unlit(gltf_material).active;
  mat.m_anisotropy = anistropy.m_anisotropy_strength;
  mat.m_anisotropy_direction =
      glm::vec3(sin(anistropy.m_anisotropy_rotation),
                cos(anistropy.m_anisotropy_rotation), 0.f);
  ;

  const KHR_materials_clearcoat& clearcoat =
      tinygltf::utils::get_clearcoat(gltf_material);
  mat.m_clearcoat_factor = clearcoat.m_factor;
  mat.m_clearcoat_roughness = clearcoat.m_roughness_factor;
  mat.m_clearcoat_roughness_texture = clearcoat.m_roughness_texture.index;
  mat.m_clearcoat_texture = clearcoat.m_texture.index;

  const KHR_materials_sheen& sheen = tinygltf::utils::get_sheen(gltf_material);
  mat.m_sheen = glm::packUnorm4x8(
      glm::vec4(sheen.m_sheen_color_factor, sheen.m_sheen_roughness_factor));
  mat.m_transmission_factor =
      tinygltf::utils::get_transmission(gltf_material).factor;
  mat.m_transmission_texture =
      tinygltf::utils::get_transmission(gltf_material).texture.index;
  mat.m_ior = tinygltf::utils::get_ior(gltf_material).ior;

  const KHR_materials_volume& volume =
      tinygltf::utils::get_volume(gltf_material);
  mat.m_attenuation_color = volume.m_attenuation_color;
  mat.m_thickness_factor = volume.m_thickness_factor;
  mat.m_thickness_texture = volume.m_thickness_texture.index;
  mat.m_attenuation_distance = volume.m_attenuation_distance;

  mat.m_emissive_factor =
      glm::make_vec3<double>(gltf_material.emissiveFactor.data());
  mat.m_emissive_texture = gltf_material.emissiveTexture.index;
  return mat;
}

}  // namespace wunder::gltf_material_serializer