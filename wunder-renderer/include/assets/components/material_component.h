#ifndef WUNDER_MATERIAL_COMPONENT_H
#define WUNDER_MATERIAL_COMPONENT_H

#include "glm/detail/type_mat4x4.hpp"
#include "glm/fwd.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace wunder {
struct material_component {
  glm::vec4 m_pbr_base_color_factor;  // 16 byte
  // 4
  int m_pbr_base_color_texture;          // 20 byte
  float m_pbr_metallic_factor;           // 24 byte
  float m_pbr_roughness_factor;          // 28 byte
  int m_pbr_metallic_roughness_texture;  // 32 byte
  // 8
  int m_emissive_texture;  // 36 byte
  // 10
  glm::vec3 m_emissive_factor;  // 52 byte
  int m_alpha_mode;             // 56 byte
  // 14
  float m_alpha_cutoff;          // 60 byte
  int m_double_sided;            // 64 byte
  int m_normal_texture;          // 68 byte
  float m_normal_texture_scale;  // 72 byte
  // 18
  glm::mat4 m_uv_transform;  // 136 byte
  // 22
  int unlit;  // 140 byte

  float m_transmission_factor;  // 144 byte
  int m_transmission_texture;   // 148 byte

  float m_ior;  // 152 byte
  // 26
  glm::vec3 m_anisotropy_direction;  // 164 byte
  float m_anisotropy;                // 168 byte
  // 30
  glm::vec3 m_attenuation_color;  // 180 byte
  float m_thickness_factor;       // 184 byte
  int m_thickness_texture;        // 188 byte
  float m_attenuation_distance;   // 192 byte
  // --
  float m_clearcoat_factor;     // 196 byte
  float m_clearcoat_roughness;  // 200 byte
  // 38
  int m_clearcoat_texture;            // 204 byte
  int m_clearcoat_roughness_texture;  // 208 byte
  uint m_sheen;                       // 212 byte
};
}  // namespace wunder
#endif  // WUNDER_MATERIAL_COMPONENT_H
