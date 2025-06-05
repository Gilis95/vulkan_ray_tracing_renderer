#ifndef MATERIAL_H
#define MATERIAL_H

#include "macros.h"

// GLTF material
#define MATERIAL_METALLICROUGHNESS 0
#define MATERIAL_SPECULARGLOSSINESS 1
#define ALPHA_OPAQUE 0
#define ALPHA_MASK 1
#define ALPHA_BLEND 2


struct GltfShadeMaterial {
  PUBLIC
  // 0
  vec4 pbrBaseColorFactor;  // 16 byte
  // 4
  int pbrBaseColorTexture;          // 20 byte
  float pbrMetallicFactor;          // 24 byte
  float pbrRoughnessFactor;         // 28 byte
  int pbrMetallicRoughnessTexture;  // 32 byte
  // 8
  int emissiveTexture;  // 36 byte
  // 10
  vec3 emissiveFactor;  // 48 byte
  int alphaMode;        // 52 byte
  // 14
  float alphaCutoff;         // 56 byte
  int doubleSided;           // 60 byte
  int normalTexture;         // 64 byte
  float normalTextureScale;  // 68 byte
  // 18
  mat4 uvTransform;  // 132 byte
  // 22
  int unlit;  // 136 byte

  float transmissionFactor;  // 140 byte
  int transmissionTexture;   // 144 byte

  float ior;  // 148 byte
  // 26
  vec3 anisotropyDirection;  // 160 byte
  float anisotropy;          // 164 byte
  // 30
  vec3 attenuationColor;      // 176 byte
  float thicknessFactor;      // 180 byte
  int thicknessTexture;       // 184 byte
  float attenuationDistance;  // 188 byte
  // --
  float clearcoatFactor;     // 192 byte
  float clearcoatRoughness;  // 196 byte
  // 38
  int clearcoatTexture;           // 200 byte
  int clearcoatRoughnessTexture;  // 204 byte
  uint sheen;                     // 208 byte

  int specularTexture;        // 212 byte
  int specularColourTexture;  // 216 byte
  float specularFactor;       // 220 byte
  vec3 specularColourFactor;  // 232 byte
  vec2 _pad1;                 // 240 byte
  vec4 _pad2;                 // 256 byte
  // 42

#ifdef __cplusplus  // Descriptor binding helper for C++ and GLSL
  GltfShadeMaterial& operator=(const wunder::material_asset& other) {
    pbrBaseColorFactor = other.m_pbr_base_color_factor;  // 16 byte

    pbrMetallicFactor = other.m_pbr_metallic_factor;    // 24 byte
    pbrRoughnessFactor = other.m_pbr_roughness_factor;  // 28 byte

    // 10
    emissiveFactor = other.m_emissive_factor;  // 52 byte
    alphaMode = other.m_alpha_mode;            // 56 byte
    // 14
    alphaCutoff = other.m_alpha_cutoff;                 // 60 byte
    doubleSided = other.m_double_sided;                 // 64 byte
    normalTextureScale = other.m_normal_texture_scale;  // 72 byte
    // 18
    uvTransform = other.m_uv_transform;  // 136 byte
    // 22
    //    unlit = other.m_uv_transform;  // 140 byte

    transmissionFactor = other.m_transmission_factor;  // 144 byte

    ior = other.m_ior;  // 152 byte
    // 26
    anisotropyDirection = other.m_anisotropy_direction;  // 164 byte
    anisotropy = other.m_anisotropy;                     // 168 byte
    // 30
    attenuationColor = other.m_attenuation_color;        // 180 byte
    thicknessFactor = other.m_thickness_factor;          // 184 byte
    attenuationDistance = other.m_attenuation_distance;  // 192 byte
    // --
    clearcoatFactor = other.m_clearcoat_factor;        // 196 byte
    clearcoatRoughness = other.m_clearcoat_roughness;  // 200 byte
    // 38
    sheen = other.m_sheen;  // 212 byte

    specularFactor = other.m_specular_factor;               // 218 byte
    specularColourFactor = other.m_specular_colour_factor;  // 230 byte

    return *this;
  }
#endif
};

#endif //MATERIAL_H
