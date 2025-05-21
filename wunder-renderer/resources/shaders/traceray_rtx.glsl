//-------------------------------------------------------------------------------------------------
// This file has the RTX functions for Closest-Hit and Any-Hit shader.
// The Ray Query pipeline implementation of thoses functions are in traceray_rq.
// This is used in pathtrace.glsl (Ray-Generation shader)


//-----------------------------------------------------------------------
// Shoot a ray an return the information of the closest hit, in the
// PtPayload structure (PRD)
//
void ClosestHit(Ray r)
{
  uint rayFlags = gl_RayFlagsCullBackFacingTrianglesEXT;
  prd.hitT      = INFINITY;
  traceRayEXT(topLevelAS,   // acceleration structure
              rayFlags,     // rayFlags
              0xFF,         // cullMask
              0,            // sbtRecordOffset
              0,            // sbtRecordStride
              0,            // missIndex
              r.origin,     // ray origin
              0.0,          // ray min range
              r.direction,  // ray direction
              INFINITY,     // ray max range
              0             // payload (location = 0)
  );
}


//-----------------------------------------------------------------------
// Shadow ray - return true if a ray hits anything
//
bool AnyHit(Ray r, float maxDist)
{
  shadow_payload.isHit = true;      // Asume hit, will be set to false if hit nothing (miss shader)
  shadow_payload.seed  = prd.seed;  // don't care for the update - but won't affect the rahit shader
  uint rayFlags = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsSkipClosestHitShaderEXT | gl_RayFlagsCullBackFacingTrianglesEXT;

  traceRayEXT(topLevelAS,   // acceleration structure
              rayFlags,     // rayFlags
              0xFF,         // cullMask
              0,            // sbtRecordOffset
              0,            // sbtRecordStride
              1,            // missIndex
              r.origin,     // ray origin
              0.0,          // ray min range
              r.direction,  // ray direction
              maxDist,      // ray max range
              1             // payload layout(location = 1)
  );

  // add to ray contribution from next event estimation
  return shadow_payload.isHit;
}
