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

#include "enums.h"
#include "environment.h"
#include "macros.h"
#include "material.h"
#include "vertex.h"

// properties of the scene
struct SceneCamera {
  mat4 viewInverse;
  mat4 projInverse;
  float focalDist;
  float aperture;
  // Extra
  uint nbLights;
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


#endif  // COMMON_HOST_DEVICE
