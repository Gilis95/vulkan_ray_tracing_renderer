#include "assets/gltf_scene_serializer.h"

#include <tiny_gltf.h>

#include <glm/mat4x4.hpp>
#include <queue>

#include "assets/asset_types.h"
#include "assets/components/light_component.h"
#include "assets/gltf_mesh_serializer.h"
#include "assets/gltf_scene_node_creator.h"
#include "assets/scene_asset.h"
#include "core/wunder_logger.h"
#include "core/wunder_macros.h"
#include "include/tinygltf/tinygltf_utils.h"

namespace wunder {

std::unordered_set<std::string> gltf_scene_serializer::s_supported_extensions{
    KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME,
    KHR_TEXTURE_TRANSFORM_EXTENSION_NAME,
    KHR_MATERIALS_SPECULAR_EXTENSION_NAME,
    KHR_MATERIALS_UNLIT_EXTENSION_NAME,
    KHR_MATERIALS_ANISOTROPY_EXTENSION_NAME,
    KHR_MATERIALS_IOR_EXTENSION_NAME,
    KHR_MATERIALS_VOLUME_EXTENSION_NAME,
    KHR_MATERIALS_TRANSMISSION_EXTENSION_NAME,
    KHR_TEXTURE_BASISU_EXTENSION_NAME,
};

std::expected<scene_asset, asset_serialization_result_codes>
gltf_scene_serializer::serialize(tinygltf::Model& gltf_scene_root) {
  check_required_extensions(gltf_scene_root.extensionsRequired);

  int defaultScene =
      gltf_scene_root.defaultScene > -1 ? gltf_scene_root.defaultScene : 0;
  const auto& gltf_scene = gltf_scene_root.scenes[defaultScene];

  // In the gltf we have a indirection, so one mesh could be used multiple times
  // in the scene, without copy-pasting it. Those why we should first find index
  // of all used meshes and instantiate only them
  std::unordered_set<uint32_t> used_meshes;
  for (auto nodeIdx : gltf_scene.nodes) {
    find_used_meshes(gltf_scene_root, used_meshes, nodeIdx);
  }

  std::unordered_map<std::uint32_t /*mesh_id*/, std::vector<mesh_component>>
      mesh_id_to_mesh_primitives =
          gltf_mesh_serializer::process_meshes(gltf_scene_root, used_meshes);

  scene_asset scene;
  auto result = process_nodes(gltf_scene_root, gltf_scene,
                              mesh_id_to_mesh_primitives, scene);
  ReturnUnless(result == asset_serialization_result_codes::ok,
               std::unexpected(result));
  process_materials(gltf_scene_root, scene);

  return scene;
}

void gltf_scene_serializer::find_used_meshes(
    const tinygltf::Model& tmodel, std::unordered_set<uint32_t>& usedMeshes,
    int nodeIdx) {
  const auto& node = tmodel.nodes[nodeIdx];
  if (node.mesh >= 0) usedMeshes.insert(node.mesh);
  for (const auto& c : node.children) {
    find_used_meshes(tmodel, usedMeshes, c);
  }
}

void gltf_scene_serializer::check_required_extensions(
    const std::vector<std::string>& required_extensions) {
  for (auto& e : required_extensions) {
    if (s_supported_extensions.find(e) == s_supported_extensions.end()) {
      WUNDER_ERROR(
          "\n---------------------------------------\n"
          "The extension {0} is REQUIRED and not supported \n",
          e.c_str());
    }
  }
}

asset_serialization_result_codes gltf_scene_serializer::process_nodes(
    const tinygltf::Model& gltf_root_node,
    const tinygltf::Scene& scene_nodes_to_process,
    std::unordered_map<std::uint32_t /*mesh_id*/, std::vector<mesh_component>>&
        mesh_id_to_primitive,
    scene_asset& out_scene) {
  //  const auto& gltf_scene_node = gltf_root_node.nodes[node_idx];

  std::queue<std::pair<std::uint32_t, glm::mat4 /*parent matrix*/>> nodes;

  for (auto root_node : scene_nodes_to_process.nodes) {
    nodes.emplace(root_node, glm::mat4(1));
  }

  // BFS
  while (!nodes.empty()) {
    auto& [node_idx, parent_matrix] = nodes.front();
    nodes.pop();

    AssertReturnIf(node_idx >= gltf_root_node.nodes.size(),
                   asset_serialization_result_codes::error);
    auto& gltf_scene_node = gltf_root_node.nodes[node_idx];

    glm::mat4 matrix = tinygltf::utils::getLocalMatrix(gltf_scene_node);
    glm::mat4 model_matrix = parent_matrix * matrix;

    if (gltf_scene_node.mesh > -1) {
      auto primitives_it = mesh_id_to_primitive.find(gltf_scene_node.mesh);
      AssertContinueIf(primitives_it == mesh_id_to_primitive.end());
      gltf_mesh_node_creator(model_matrix, out_scene, primitives_it->second)
          .create();
    }
    if (gltf_scene_node.camera > -1) {
      gltf_camera_node_creator(model_matrix, out_scene).create();
    } else if (gltf_scene_node.extensions.find(
                   KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME) !=
               gltf_scene_node.extensions.end()) {
      const auto& ext =
          gltf_scene_node.extensions.find(KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME)
              ->second;
      auto lightIdx = ext.Get("light").GetNumberAsInt();
      auto& gltf_light = gltf_root_node.lights[lightIdx];

      gltf_light_node_creator(model_matrix, out_scene, gltf_light).create();
    }

    for (auto child : gltf_scene_node.children) {
      nodes.emplace(child,model_matrix);
    }
  }

  return asset_serialization_result_codes::ok;
}

void gltf_scene_serializer::process_materials(
    const tinygltf::Model& gltf_scene_root, scene_asset& out_scene) {
  for (auto& gltf_material : gltf_scene_root.materials) {
    material_component mat;

    mat.m_alpha_cutoff = static_cast<float>(gltf_material.alphaCutoff);
    mat.m_alpha_mode = gltf_material.alphaMode == "MASK" ? 1 : (gltf_material.alphaMode == "BLEND" ? 2 : 0);
    mat.m_double_sided = gltf_material.doubleSided ? 1 : 0;
    mat.m_emissive_factor = gltf_material.emissiveFactor.size() == 3
            ? glm::vec3(gltf_material.emissiveFactor[0],
                                            gltf_material.emissiveFactor[1],
                                            gltf_material.emissiveFactor[2])
            : glm::vec3(0.f);
    mat.m_emissive_texture = gltf_material.emissiveTexture.index;
    mat.m_normal_texture = gltf_material.normalTexture.index;
    mat.m_normal_texture_scale = static_cast<float>(gltf_material.normalTexture.scale);
    mat.m_occlusion_texture = gltf_material.occlusionTexture.index;
    mat.m_occlusion_texture_strength =
        static_cast<float>(gltf_material.occlusionTexture.strength);

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
    mat.m_transmission_factor = tinygltf::utils::get_transmission(gltf_material).factor;
    mat.m_transmission_texture =
        tinygltf::utils::get_transmission(gltf_material).texture.index;
    mat.m_ior = tinygltf::utils::get_ior(gltf_material).ior;

    const KHR_materials_volume& volume = tinygltf::utils::get_volume(gltf_material);
    mat.m_attenuation_color = volume.m_attenuation_color;
    mat.m_thickness_factor = volume.m_thickness_factor;
    mat.m_thickness_texture = volume.m_thickness_texture.index;
    mat.m_attenuation_distance = volume.m_attenuation_distance;

    mat.m_emissive_factor = glm::make_vec3<double>(gltf_material.emissiveFactor.data());
    mat.m_emissive_texture = gltf_material.emissiveTexture.index;

    scene_node node;
    node.add_component(mat);
    out_scene.add_node(std::move(node));
  }
}

}  // namespace wunder