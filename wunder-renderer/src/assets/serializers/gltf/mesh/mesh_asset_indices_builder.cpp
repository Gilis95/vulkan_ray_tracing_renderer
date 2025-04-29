#include "assets/asset_storage.h"
#include "assets/asset_types.h"
#include "assets/mesh_asset.h"
#include "assets/serializers/gltf/mesh/mesh_asset_indices_builder.h"
#include "glm/vec4.hpp"
#include "tiny_gltf.h"
#include "tinygltf/tinygltf_utils.h"

namespace wunder {
mesh_asset_indices_builder::mesh_asset_indices_builder(
    const tinygltf::Model& gltf_scene_root,
    const tinygltf::Primitive& gltf_primitive, mesh_asset& mesh_asset)
    : m_gltf_scene_root(gltf_scene_root),
      m_gltf_primitive(gltf_primitive),
      m_mesh_asset(mesh_asset) {}

bool mesh_asset_indices_builder::build() {
  if (m_gltf_primitive.indices > -1) {
    const tinygltf::Accessor& index_accessor =
        m_gltf_scene_root.accessors[m_gltf_primitive.indices];
    m_mesh_asset.m_indices.reserve(static_cast<uint32_t>(index_accessor.count));

    auto lambda = [this, &index_accessor]<typename T>() {
      std::vector<T> indices;
      indices.resize(index_accessor.count);

      tinygltf::utils::copy_accessor_data(
          indices, 0, m_gltf_scene_root, index_accessor, 0, index_accessor.count);

      std::move(indices.begin(), indices.end(),
                std::back_inserter(m_mesh_asset.m_indices));
    };

    switch (index_accessor.componentType) {
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
        lambda.operator()<std::uint32_t>();
      } break;
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
        lambda.operator()<std::uint16_t>();
      } break;
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
        lambda.operator()<std::uint8_t>();
      } break;
      default:
        WUNDER_ERROR_TAG("Asset", "Index component type %i not supported!\n",
                         index_accessor.componentType);
        return false;
    }

    return true;
  }

  // Primitive without indices, creating them
  const auto& accessor =
      m_gltf_scene_root
          .accessors[m_gltf_primitive.attributes.find("POSITION")->second];
  for (size_t i = 0; i < accessor.count; i++) {
    m_mesh_asset.m_indices.push_back(i);
  }

  return true;
}
}  // namespace wunder