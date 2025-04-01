#include "assets/mesh_asset.h"
#include "assets/serializers/gltf/mesh/mesh_asset_tangents_builder.h"
#include "tinygltf/tinygltf_utils.h"

namespace wunder {
mesh_asset_tangents_builder::mesh_asset_tangents_builder(
    const tinygltf::Model& gltf_scene_root,
    const tinygltf::Primitive& gltf_primitive, mesh_asset& out_mesh_asset)
    : m_gltf_scene_root(gltf_scene_root),
      m_gltf_primitive(gltf_primitive),
      m_out_mesh_asset(out_mesh_asset) {}

void mesh_asset_tangents_builder::build() {
  std::vector<glm::vec4> tangents;
  if (!tinygltf::utils::get_attribute<glm::vec4>(
          m_gltf_scene_root, m_gltf_primitive, tangents, "TANGENT")) {
    create_tangents( tangents);
  }

  for (uint32_t i = 0; i < tangents.size(); ++i) {
    auto& tangent = tangents[i];
    m_out_mesh_asset.m_vertices[i].m_tangent = tangent;
  }
}

void mesh_asset_tangents_builder::create_tangents(
     std::vector<glm::vec4>& out_tangents) {
  std::size_t vertices_count = m_out_mesh_asset.m_vertices.size();
  std::vector<glm::vec3> tangent(vertices_count);
  std::vector<glm::vec3> bitangent(vertices_count);

  // Current implementation
  // http://foundationsofgameenginedev.com/FGED2-sample.pdf
  for (size_t i = 0; i < m_out_mesh_asset.m_indices.size(); i += 3) {
    // local index
    uint32_t i0 = m_out_mesh_asset.m_indices[i + 0];
    uint32_t i1 = m_out_mesh_asset.m_indices[i + 1];
    uint32_t i2 = m_out_mesh_asset.m_indices[i + 2];
    AssertContinueUnless(i0 < vertices_count);
    AssertContinueUnless(i1 < vertices_count);
    AssertContinueUnless(i2 < vertices_count);

    const auto& p0 = m_out_mesh_asset.m_vertices[i0];
    const auto& p1 = m_out_mesh_asset.m_vertices[i1];
    const auto& p2 = m_out_mesh_asset.m_vertices[i2];

    const auto& uv0 = p0.m_texcoord;
    const auto& uv1 = p1.m_texcoord;
    const auto& uv2 = p2.m_texcoord;

    glm::vec3 e1 = p1.m_position - p0.m_position;
    glm::vec3 e2 = p2.m_position - p0.m_position;

    glm::vec2 duvE1 = uv1 - uv0;
    glm::vec2 duvE2 = uv2 - uv0;

    float r = 1.0F;
    float a = duvE1.x * duvE2.y - duvE2.x * duvE1.y;
    if (fabs(a) > 0)  // Catch degenerated UV
    {
      r = 1.0f / a;
    }

    glm::vec3 t = (e1 * duvE2.y - e2 * duvE1.y) * r;
    glm::vec3 b = (e2 * duvE1.x - e1 * duvE2.x) * r;

    tangent[i0] += t;
    tangent[i1] += t;
    tangent[i2] += t;

    bitangent[i0] += b;
    bitangent[i1] += b;
    bitangent[i2] += b;
  }

  for (uint32_t a = 0; a < vertices_count; a++) {
    const auto& t = tangent[a];
    const auto& b = bitangent[a];
    const auto& n = m_out_mesh_asset.m_vertices[a].m_normal;

    // Gram-Schmidt orthogonalize
    glm::vec3 otangent = glm::normalize(t - (glm::dot(n, t) * n));

    // In case the tangent is invalid
    if (otangent == glm::vec3(0, 0, 0)) {
      otangent = glm::vec3(make_fast_tangent(n));
    }

    // Calculate handedness
    float handedness = (glm::dot(glm::cross(n, t), b) <= 0.0F) ? 1.0F : -1.0F;
    out_tangents.emplace_back(otangent.x, otangent.y, otangent.z, handedness);
  }
}

glm::vec4 mesh_asset_tangents_builder::make_fast_tangent(const glm::vec3& n) {
  if (n.z < -0.99998796F)  // Handle the singularity
  {
    return {0.0F, -1.0F, 0.0F, 1.0F};
  }
  const float a = 1.0F / (1.0F + n.z);
  const float b = -n.x * n.y * a;
  return {1.0F - n.x * n.x * a, b, -n.x, 1.0F};
}
}  // namespace wunder