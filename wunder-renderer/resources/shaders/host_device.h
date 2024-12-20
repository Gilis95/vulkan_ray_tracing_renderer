/*
 * Copyright (c) 2021, NVIDIA CORPORATION.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION
 * SPDX-License-Identifier: Apache-2.0
 */

/*
  Various structure used by CPP and GLSL
*/

#ifndef COMMON_HOST_DEVICE
#define COMMON_HOST_DEVICE

#ifdef __cplusplus
#include <stdint.h>

#include <glm/glm.hpp>

#include "include/assets/material_asset.h"
#include "include/assets/mesh_asset.h"
// GLSL Type
using ivec2 = glm::ivec2;
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;
using uint = unsigned int;
#endif

// clang-format off
#ifdef __cplusplus  // Descriptor binding helper for C++ and GLSL
#define PUBLIC public:

#define START_ENUM(a)                                                                                               \
  enum a                                                                                                               \
  {
#define END_ENUM() }
#else
#define START_ENUM(a) const uint
#define END_ENUM()
#define PUBLIC
#endif

// Sets
START_ENUM(SetBindings)
  S_ACCEL = 0,  // Acceleration structure
  S_OUT   = 1,  // Offscreen output image
  S_SCENE = 2,  // Scene data
  S_ENV   = 3,  // Environment / Sun & Sky
  S_WF    = 4   // Wavefront extra data
END_ENUM();

// Acceleration Structure - Set 0
START_ENUM(AccelBindings)
  eTlas = 0 
END_ENUM();

// Output image - Set 1
START_ENUM(OutputBindings)
  eSampler = 0,  // As sampler
  eStore   = 1   // As storage
END_ENUM();

// Scene Data - Set 2
START_ENUM(SceneBindings)
  eCamera    = 0, 
  eMaterials = 1, 
  eInstData  = 2, 
  eLights    = 3,            
  eTextures  = 4  // must be last elem            
END_ENUM();

// Environment - Set 3
START_ENUM(EnvBindings)
  eSunSky     = 0, 
  eHdr        = 1, 
  eImpSamples = 2 
END_ENUM();

START_ENUM(DebugMode)
  eNoDebug   = 0,   //
  eBaseColor = 1,   //
  eNormal    = 2,   //
  eMetallic  = 3,   //
  eEmissive  = 4,   //
  eAlpha     = 5,   //
  eRoughness = 6,   //
  eTexcoord  = 7,   //
  eTangent   = 8,   //
  eRadiance  = 9,   //
  eWeight    = 10,  //
  eRayDir    = 11,  //
  eHeatmap   = 12   //
END_ENUM();
// clang-format on

// properties of the scene
struct SceneCamera {
  mat4 viewInverse;
  mat4 projInverse;
  float focalDist;
  float aperture;
  // Extra
  int nbLights;
};

struct VertexAttributes {
  vec3 position;
  uint normal;    // compressed using oct
  vec2 texcoord;  // Tangent handiness, stored in LSB of .y
  uint tangent;   // compressed using oct
  uint color;     // RGBA
};

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
  int _pad0;            // 40 byte
  // 10
  vec3 emissiveFactor;  // 52 byte
  int alphaMode;        // 56 byte
  // 14
  float alphaCutoff;         // 60 byte
  int doubleSided;           // 64 byte
  int normalTexture;         // 68 byte
  float normalTextureScale;  // 72 byte
  // 18
  mat4 uvTransform;  // 136 byte
  // 22
  int unlit;  // 140 byte

  float transmissionFactor;  // 144 byte
  int transmissionTexture;   // 148 byte

  float ior;  // 152 byte
  // 26
  vec3 anisotropyDirection;  // 164 byte
  float anisotropy;          // 168 byte
  // 30
  vec3 attenuationColor;      // 180 byte
  float thicknessFactor;      // 184 byte
  int thicknessTexture;       // 188 byte
  float attenuationDistance;  // 192 byte
  // --
  float clearcoatFactor;     // 196 byte
  float clearcoatRoughness;  // 200 byte
  // 38
  int clearcoatTexture;           // 204 byte
  int clearcoatRoughnessTexture;  // 208 byte
  uint sheen;                     // 212 byte
  int _pad1;                      // 216 byte
  // 42

#ifdef __cplusplus  // Descriptor binding helper for C++ and GLSL
  GltfShadeMaterial& operator=(const wunder::material_asset& other) {
    pbrBaseColorFactor = other.m_pbr_base_color_factor;  // 16 byte

    pbrMetallicFactor = other.m_pbr_metallic_factor;       // 24 byte
    pbrRoughnessFactor = other.m_pbr_roughness_factor;     // 28 byte

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

    transmissionFactor = other.m_transmission_factor;    // 144 byte

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
    sheen = other.m_sheen;                    // 212 byte

    return *this;
  }
#endif
};

// Use with PushConstant
struct RtxState {
  int frame;                    // Current frame, start at 0
  int maxDepth;                 // How deep the path is
  int maxSamples;               // How many samples to do per render
  float fireflyClampThreshold;  // to cut fireflies
  float hdrMultiplier;          // To brightening the scene
  int debugging_mode;           // See DebugMode
  int pbrMode;                  // 0-Disney, 1-Gltf
  int _pad0;                    // vec2 need alignment
  ivec2 size;                   // rendering size
  int minHeatmap;               // Debug mode - heat map
  int maxHeatmap;
};

// Structure used for retrieving the primitive information in the closest hit
// using gl_InstanceCustomIndexNV
struct InstanceData {
  uint64_t vertexAddress;
  uint64_t indexAddress;
  int materialIndex;
};

// KHR_lights_punctual extension.
// see
// https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_lights_punctual

const int LightType_Directional = 0;
const int LightType_Point = 1;
const int LightType_Spot = 2;

struct Light {
  vec3 direction;
  float range;

  vec3 color;
  float intensity;

  vec3 position;
  float innerConeCos;

  float outerConeCos;
  int type;

  vec2 padding;
};

// Environment acceleration structure - computed in hdr_sampling
struct EnvAccel {
  uint alias;
  float q;
  float pdf;
  float aliasPdf;
};

// Tonemapper used in post.frag
struct Tonemapper {
  float brightness;
  float contrast;
  float saturation;
  float vignette;
  float avgLum;
  float zoom;
  vec2 renderingRatio;
  int autoExposure;
  float Ywhite;  // Burning white
  float key;     // Log-average luminance
  int dither;
};

struct SunAndSky {
  vec3 rgb_unit_conversion;
  float multiplier;

  float haze;
  float redblueshift;
  float saturation;
  float horizon_height;

  vec3 ground_color;
  float horizon_blur;

  vec3 night_color;
  float sun_disk_intensity;

  vec3 sun_direction;
  float sun_disk_scale;

  float sun_glow_intensity;
  int y_is_up;
  int physically_scaled_sun;
  int in_use;
};

#endif  // COMMON_HOST_DEVICE
