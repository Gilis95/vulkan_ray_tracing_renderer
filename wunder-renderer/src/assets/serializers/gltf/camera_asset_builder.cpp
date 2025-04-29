#include "assets/serializers/gltf/camera_asset_builder.h"

#include <algorithm>
#include <cctype>

#include "include/assets/camera_asset.h"
#include "tiny_gltf.h"
#include "tinygltf/tinygltf_utils.h"

bool ichar_equals(char a, char b) {
  return std::tolower(static_cast<unsigned char>(a)) ==
         std::tolower(static_cast<unsigned char>(b));
}

namespace wunder {

camera_asset_builder::camera_asset_builder(
    const tinygltf::Camera& gltf_camera,
    const tinygltf::ExtensionMap& gltf_node_extensions)
    : gltf_camera(gltf_camera), gltf_node_extensions(gltf_node_extensions) {}

std::optional<camera_asset> camera_asset_builder::build() {
  ReturnUnless(std::ranges::equal(gltf_camera.type,
                                  GLTF_PERSPECTIVE_CAMERA_TYPE, ichar_equals),
               std::nullopt);

  camera_asset camera{};

  auto gltf_perspective_camera = gltf_camera.perspective;
  camera.m_perspective_camera_data.aspect_ratio =
      gltf_perspective_camera.aspectRatio;
  camera.m_perspective_camera_data.fov = gltf_perspective_camera.yfov;

  // If the node has the Iray extension, extract the camera information.
  const auto found_ext = gltf_node_extensions.find(EXTENSION_ATTRIB_IRAY);
  ReturnIf(found_ext == gltf_node_extensions.end(), camera);

  auto& iray_ext = found_ext->second;
  auto& attributes = iray_ext.Get("attributes");
  tinygltf::utils::get_array_value(attributes, "iview:position", camera.eye);
  tinygltf::utils::get_array_value(attributes, "iview:interest", camera.center);
  tinygltf::utils::get_array_value(attributes, "iview:up", camera.up);

  return camera;
}
}  // namespace wunder