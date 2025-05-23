#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable

#include "globals.glsl"

//-------------------------------------------------------------------------------------------------
// This will be executed when sending shadow rays and missing all geometries
// - There are no hit shader for the shadow ray, therefore
// - Before calling Trace, set isHit=true
// - The default anyhit, closesthit won't change isHit, but if nothing is hit, it will be
//   set to false.
//-------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------
// Miss shader for the shadow rayPayloadInEXT
//

layout(location = 1) rayPayloadInEXT ShadowHitPayload payload;

void main()
{
  payload.isHit = false;
}
