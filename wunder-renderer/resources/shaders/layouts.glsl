//-------------------------------------------------------------------------------------------------
// This file holds the layout used by all ray tracing shaders


#ifndef LAYOUTS_GLSL
#define LAYOUTS_GLSL 1


// C++ shared structures and binding
#include "host_device.h"

//----------------------------------------------
// Descriptor Set Layout
//----------------------------------------------


// clang-format off
layout(set = S_ACCEL, binding = eTlas)					uniform accelerationStructureEXT topLevelAS;
//
layout(set = S_OUT,   binding = eStore)					uniform image2D			rtxGeneratedImage;
//
layout(set = S_SCENE, binding = eInstData,	scalar)     buffer _InstanceInfo	{ InstanceData geoInfo[]; };
layout(set = S_SCENE, binding = eCamera,	scalar)		uniform _SceneCamera	{ SceneCamera sceneCamera; };
layout(set = S_SCENE, binding = eMaterials,	scalar)		buffer _MaterialBuffer	{ GltfShadeMaterial materials[]; };
layout(set = S_SCENE, binding = eLights,	scalar)		buffer _Lights			{ Light lights[]; };
layout(set = S_SCENE, binding = eTextures	      )		uniform sampler2D		texturesMap[];
//
layout(set = S_ENV, binding = eSunSky,		scalar)		uniform _SSBuffer		{ SunAndSky _sunAndSky; };
layout(set = S_ENV, binding = eHdr)						uniform sampler2D		environmentTexture;
layout(set = S_ENV, binding = eImpSamples,  scalar)		buffer _EnvAccel		{ EnvAccel envSamplingData[]; };

layout(buffer_reference, scalar) buffer Vertices { VertexAttributes v[]; };
layout(buffer_reference, scalar) buffer Indices	 { uvec3 i[];            };

// clang-format on


#endif  // LAYOUTS_GLSL
