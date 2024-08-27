
#include "assets/gltf_scene_node_creator.h"

#include "assets/components/camera_component.h"
#include "assets/gltf_scene_serializer.h"
#include "assets/scene_asset.h"
#include "assets/scene_node.h"
#include "include/tinygltf/tinygltf_utils.h"

namespace wunder {
gltf_scene_node_creator::gltf_scene_node_creator(glm::mat4& model_matrix,
                                                 scene_asset& scene_asset)
    : m_model_matrix(model_matrix), m_scene_asset(scene_asset) {}

transform_component gltf_scene_node_creator::get_transform_component() const {
  return transform_component{.m_world_matrix = m_model_matrix};
}

gltf_mesh_node_creator::gltf_mesh_node_creator(
    glm::mat4& model_matrix, scene_asset& scene_asset,
    const std::vector<mesh_component>& mesh_components)
    : gltf_scene_node_creator(model_matrix, scene_asset),
      m_mesh_components(mesh_components) {}

void gltf_mesh_node_creator::create() /*override*/
{
  // primitives
  for (auto& mesh_component : m_mesh_components) {
    scene_node node;
    node.add_component(mesh_component);
    node.add_component(get_transform_component());

    m_scene_asset.add_node(std::move(node));
  }
}

gltf_camera_node_creator::gltf_camera_node_creator(glm::mat4& model_matrix,
                                                   scene_asset& scene_asset)
    : gltf_scene_node_creator(model_matrix, scene_asset) {}

void gltf_camera_node_creator::create() /*override*/
{
  camera_component camera;

  // If the node has the Iray extension, extract the camera information.
  if (tinygltf::utils::has_element_name(m_gltf_node_extensions_map,
                                        EXTENSION_ATTRIB_IRAY)) {
    auto& iray_ext = m_gltf_node_extensions_map.at(EXTENSION_ATTRIB_IRAY);
    auto& attributes = iray_ext.Get("attributes");
    tinygltf::utils::get_array_value(attributes, "iview:position", camera.eye);
    tinygltf::utils::get_array_value(attributes, "iview:interest",
                                     camera.center);
    tinygltf::utils::get_array_value(attributes, "iview:up", camera.up);
  }

  scene_node node;
  node.add_component(camera);
  node.add_component(get_transform_component());
  m_scene_asset.add_node(std::move(node));
}

gltf_light_node_creator::gltf_light_node_creator(
    glm::mat4& model_matrix, scene_asset& scene_asset,
    const tinygltf::Light& gltf_light)
    : gltf_scene_node_creator(model_matrix, scene_asset),
      m_gltf_light(gltf_light) {}

void gltf_light_node_creator::create() /*override*/
{
  auto maybe_colour =
      tinygltf::utils::vector_to_glm<glm::vec4>(m_gltf_light.color);
  AssertReturnUnless(maybe_colour.has_value());
  speecific_light_type_data light_specific_data;
  if (m_gltf_light.type == "point") {
    light_specific_data = point_light{};
  } else if (m_gltf_light.type == "directional") {
    light_specific_data = directional_light{};
  } else if (m_gltf_light.type == "spot") {
    light_specific_data =
        spot_light{.inner_cone_angle = m_gltf_light.spot.innerConeAngle,
                   .outer_cone_angle = m_gltf_light.spot.outerConeAngle};
  }

  light_component light{.color = maybe_colour.value(),
                        .intensity = m_gltf_light.intensity,
                        .range = m_gltf_light.range};
  scene_node node;
  node.add_component(light);
  node.add_component(get_transform_component());

  m_scene_asset.add_node(std::move(node));
}
}  // namespace wunder