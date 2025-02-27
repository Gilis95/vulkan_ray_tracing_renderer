#include "gla/vulkan/scene/vulkan_lights_helper.h"

#include "assets/asset_manager.h"
#include "assets/light_asset.h"
#include "core/project.h"
#include "gla/vulkan/vulkan_device_buffer.h"
#include "glm/ext/matrix_transform.hpp"
#include "resources/shaders/host_device.h"

namespace wunder::vulkan {
using host_light_type = Light;

using host_light_array = std::vector<host_light_type>;

unique_ptr<storage_buffer> lights_helper::create_light_buffer(
    const std::vector<ref<scene_node>>& light_nodes) {
  vector_map<asset_handle, transform_component> transformations;

  assets<light_asset> light_assets =
      extract_scene_light_data(light_nodes, transformations);

  host_light_array host_lights =
      create_host_light_array(transformations, light_assets);

  return std::make_unique<storage_device_buffer>(
      descriptor_build_data{.m_enabled = true,
                                    .m_descriptor_name = "_Lights"},
      host_lights.data(),
      host_lights.size() * sizeof(host_light_array ::value_type),
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
}

assets<light_asset> lights_helper::extract_scene_light_data(
    const std::vector<ref<scene_node>>& light_nodes,
    vector_map<asset_handle, transform_component>& out_transformations) {
  auto& asset_manager = project::instance().get_asset_manager();

  std::vector<asset_handle> light_asset_handles;
  for (auto& light_ref : light_nodes) {
    optional_const_ref<light_component> maybe_light_component =
        light_ref.get().get_component<light_component>();
    AssertContinueUnless(maybe_light_component.has_value());
    optional_const_ref<transform_component> maybe_transform_component =
        light_ref.get().get_component<transform_component>();
    AssertContinueUnless(maybe_transform_component.has_value());

    asset_handle light_handle = maybe_light_component->get().m_handle;
    light_asset_handles.emplace_back(light_handle);
    out_transformations[light_handle] = maybe_transform_component->get();
  }

  auto result = asset_manager.find_assets<light_asset>(light_asset_handles.begin(),
                                                light_asset_handles.end());
  if (result.empty()) {
    result.emplace_back(asset_handle::invalid(), get_default_light_asset());
    out_transformations.emplace_back(asset_handle::invalid(), transform_component{});
  }

  return std::move(result);
}

std::vector<Light> lights_helper::create_host_light_array(
    const vector_map<asset_handle, transform_component>& transformations,
    assets<light_asset>& light_assets) {
  std::vector<Light> host_lights;

  for (auto& [id, light_asset] : light_assets) {
    auto& host_light = host_lights.emplace_back();

    auto transformation_it = transformations.find(id);

    auto model_matrix = glm::identity<glm::mat4>();

    if(transformation_it != transformations.end()){
      model_matrix = transformation_it->second.m_world_matrix;
    }

    map_to_host_light(host_light, light_asset, model_matrix);
  }

  return host_lights;
}

void lights_helper::map_to_host_light(Light& host_light,
                                      const light_asset& light,
                                      glm::mat4 model_matrix) {
  host_light.range = static_cast<float>(light.range);
  host_light.intensity = static_cast<float>(light.intensity);
  host_light.color = light.color;

  host_light.position = glm::vec3(model_matrix * glm::vec4(0, 0, 0, 1));
  host_light.direction = glm::vec3(model_matrix * glm::vec4(0, 0, -1, 0));

  map_host_light_specific_data(host_light, light);
}

void lights_helper::map_host_light_specific_data(Light& host_light,
                                                 const light_asset& light) {
  std::visit(overloaded{[&host_light](const spot_light& light) {
                          host_light.type = LightType_Spot;
                          host_light.innerConeCos =
                              static_cast<float>(cos(light.inner_cone_angle));
                          host_light.outerConeCos =
                              static_cast<float>(cos(light.outer_cone_angle));
                        },
                        [&host_light](directional_light light) {
                          host_light.type = LightType_Directional;
                        },
                        [&host_light](point_light light) {
                          host_light.type = LightType_Point;
                        }},
             light.specific_data);
}

light_asset& lights_helper::get_default_light_asset() {
  static light_asset res{};
  return res;
}


}  // namespace wunder::vulkan