#include "assets/serializers/gltf/gltf_asset_importer.h"

#include <queue>

#include "assets/asset_storage.h"
#include "assets/asset_types.h"
#include "assets/scene_asset.h"
#include "assets/serializers/gltf/camera_asset_builder.h"
#include "assets/serializers/gltf/light_asset_builder.h"
#include "assets/serializers/gltf/material_asset_builder.h"
#include "assets/serializers/gltf/mesh/mesh_asset_builder.h"
#include "assets/serializers/gltf/texture_asset_builder.h"
#include "core/wunder_logger.h"
#include "core/wunder_macros.h"
#include "glm/mat4x4.hpp"
#include "include/assets/light_asset.h"
#include "include/assets/material_asset.h"
#include "include/assets/texture_asset.h"
#include "tiny_gltf.h"
#include "tinygltf/tinygltf_utils.h"

namespace wunder {
namespace {
std::unordered_set<std::string> s_supported_extensions{
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
}

gltf_asset_importer::gltf_asset_importer(asset_storage& storage)
    : m_storage(storage) {}

asset_serialization_result_codes gltf_asset_importer::import_asset(
    tinygltf::Model& gltf_model) {
  check_required_extensions(gltf_model.extensionsRequired);

  auto textures_map = import_textures(gltf_model);
  auto materials_map = import_materials(gltf_model, textures_map);
  auto lights_map = import_lights(gltf_model);
  auto cameras_map = import_cameras(gltf_model);
  auto mesh_id_to_mesh_primitives = import_meshes(gltf_model, materials_map);

  auto result = import_scenes(gltf_model, mesh_id_to_mesh_primitives,
                              cameras_map, lights_map);
  return result;
}

void gltf_asset_importer::check_required_extensions(
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

std::unordered_map<std::uint32_t, asset_handle>
gltf_asset_importer::import_textures(const tinygltf::Model& gltf_scene_root) {
  std::unordered_map<std::uint32_t, asset_handle> textures_map;
  std::uint32_t i = 0;
  for (const auto& gltf_texture : gltf_scene_root.textures) {
    texture_asset_builder texture_builder(gltf_scene_root, gltf_texture);
    auto maybe_texture = texture_builder.build();
    AssertContinueUnless(maybe_texture.has_value());
    textures_map.emplace(i, m_storage.add_asset(maybe_texture.value()));
    ++i;
  }

  return textures_map;
}

std::unordered_map<std::uint32_t, asset_handle>
gltf_asset_importer::import_materials(
    const tinygltf::Model& gltf_scene_root,
    const std::unordered_map<std::uint32_t, asset_handle>& textures_map) {
  std::unordered_map<std::uint32_t, asset_handle> materials_map;
  std::uint32_t i = 0;
  for (auto& gltf_material : gltf_scene_root.materials) {
    material_asset_builder material_importer(gltf_material, textures_map);

    material_asset mat = material_importer.build();
    materials_map.emplace(i, m_storage.add_asset(std::move(mat)));
    ++i;
  }

  return materials_map;
}

std::unordered_map<std::uint32_t, asset_handle>
gltf_asset_importer::import_lights(const tinygltf::Model& gltf_scene_root) {
  std::unordered_map<std::uint32_t, asset_handle> lights_map;
  std::uint32_t i = 0;
  for (auto& gltf_light : gltf_scene_root.lights) {
    light_asset_builder light_builder(gltf_light);

    auto maybe_light_asset = light_builder.build();
    AssertContinueUnless(maybe_light_asset.has_value());

    lights_map.emplace(
        i, m_storage.add_asset(std::move(maybe_light_asset.value())));
    ++i;
  }

  return lights_map;
}

std::unordered_map<std::uint32_t, asset_handle>
gltf_asset_importer::import_cameras(tinygltf::Model& gltf_scene_root) {
  std::unordered_map<std::uint32_t, asset_handle> cameras_map;

  std::uint32_t i = 0;
  for (auto& gltf_camera : gltf_scene_root.cameras) {
    camera_asset_builder asset_builder(gltf_camera, gltf_scene_root.extensions);

    auto maybe_camera_asset = asset_builder.build();
    ContinueUnless(maybe_camera_asset.has_value());

    cameras_map.emplace(
        i, m_storage.add_asset(std::move(maybe_camera_asset.value())));
    ++i;
  }

  return cameras_map;
}

std::unordered_map<std::uint32_t /*mesh_id*/, std::vector<asset_handle>>
gltf_asset_importer::import_meshes(
    tinygltf::Model& gltf_scene_root,
    const std::unordered_map<uint32_t, asset_handle>& material_map) {
  // Convert all mesh/primitives+ to a single primitive per mesh
  std::unordered_map<std::uint32_t /*mesh_id*/, std::vector<asset_handle>>
      mesh_id_to_primitives;
  std::uint32_t mesh_index = 0;
  for (const auto& gltf_mesh : gltf_scene_root.meshes) {
    std::vector<asset_handle> mesh_nodes;
    for (const auto& gltf_primitive : gltf_mesh.primitives) {
      mesh_asset_builder mesh_builder(gltf_scene_root, gltf_primitive,
                                      gltf_mesh.name, material_map);
      auto maybe_mesh_node = mesh_builder.build();
      ContinueUnless(maybe_mesh_node.has_value());

      mesh_nodes.emplace_back(
          m_storage.add_asset(std::move(maybe_mesh_node.value())));
    }

    mesh_id_to_primitives[mesh_index] = std::move(mesh_nodes);
    ++mesh_index;
  }

  return mesh_id_to_primitives;
}

asset_serialization_result_codes gltf_asset_importer::import_scenes(
    const tinygltf::Model& gltf_root_node,
    const std::unordered_map<std::uint32_t /*mesh_id*/, std::vector<asset_handle>>&
        mesh_id_to_primitive,
    const std::unordered_map<std::uint32_t, asset_handle>& cameras_map,
    const std::unordered_map<std::uint32_t, asset_handle>& lights_map) {
  std::queue<std::pair<std::uint32_t, glm::mat4 /*parent matrix*/>> nodes;
  for (auto& gltf_scene : gltf_root_node.scenes) {
    scene_asset scene;

    for (auto root_node : gltf_scene.nodes) {
      nodes.emplace(root_node, glm::mat4(1));
    }

    // TODO:: flatenning the scene is currently happening on scene deserialize,
    // instead it should happen on load!!!
    //  BFS
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

        for (auto mesh_handle : primitives_it->second) {
          mesh_component mesh_component;
          mesh_component.m_handle = mesh_handle;

          auto maybe_mesh_asset = m_storage.find_asset<mesh_asset>(mesh_handle);
          AssertContinueUnless(maybe_mesh_asset);

          scene.mutable_aabb().insert(
              maybe_mesh_asset->get().m_bounding_box.transform(model_matrix));

          scene_node node;
          node.add_component(
              transform_component{.m_world_matrix = model_matrix});
          node.add_component(mesh_component);

          scene.add_node(std::move(node));
        }
      }

      if (gltf_scene_node.camera > -1) {
        auto primitives_it = cameras_map.find(gltf_scene_node.camera);
        AssertContinueIf(primitives_it == cameras_map.end());

        camera_component camera_component;
        camera_component.m_handle = primitives_it->second;

        scene_node node;
        node.add_component(transform_component{.m_world_matrix = model_matrix});
        node.add_component(camera_component);

        scene.add_node(std::move(node));
      }

      if (gltf_scene_node.extensions.find(KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME) !=
          gltf_scene_node.extensions.end()) {
        const auto& ext =
            gltf_scene_node.extensions.find(KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME)
                ->second;
        auto lightIdx = ext.Get("light").GetNumberAsInt();

        auto primitives_it = lights_map.find(lightIdx);
        AssertContinueIf(primitives_it == lights_map.end());

        light_component light_component;
        light_component.m_handle = primitives_it->second;

        scene_node node;
        node.add_component(transform_component{.m_world_matrix = model_matrix});
        node.add_component(light_component);

        scene.add_node(std::move(node));
      }

      for (auto child : gltf_scene_node.children) {
        nodes.emplace(child, model_matrix);
      }
    }

    m_storage.add_asset(scene);
  }

  return asset_serialization_result_codes::ok;
}

// PRIVATE COMPILATION UNIT FUNCTION DEFINITION ENDS HERE //

}  // namespace wunder