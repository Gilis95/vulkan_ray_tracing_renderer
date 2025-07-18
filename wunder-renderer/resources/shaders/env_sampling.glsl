//-------------------------------------------------------------------------------------------------
// This file has the functions to sample the environment


#ifndef ENV_SAMPLING_GLSL
#define ENV_SAMPLING_GLSL

#include "globals.glsl"
#include "common.glsl"

//-------------------------------------------------------------------------------------------------
// Environment Sampling (HDR)
// See:  https://arxiv.org/pdf/1901.05423.pdf
//-------------------------------------------------------------------------------------------------
vec3 Environment_sample(sampler2D lat_long_tex, in vec3 randVal, out vec3 to_light, out float pdf)
{

  // Uniformly pick a texel index idx in the environment map
  vec3  xi     = randVal;
  uvec2 tsize  = textureSize(lat_long_tex, 0);
  uint  width  = tsize.x;
  uint  height = tsize.y;

  const uint size = width * height;
  const uint idx  = min(uint(xi.x * float(size)), size - 1);

  // Fetch the sampling data for that texel, containing the ratio q between its
  // emitted radiance and the average of the environment map, the texel alias,
  // the probability distribution function (PDF) values for that texel and its
  // alias
  EnvAccel sample_data = envSamplingData[idx];

  uint env_idx;

  if(xi.y < sample_data.q)
  {
    // If the random variable is lower than the intensity ratio q, we directly pick
    // this texel, and renormalize the random variable for later use. The PDF is the
    // one of the texel itself
    env_idx = idx;
    xi.y /= sample_data.q;
    pdf = sample_data.pdf;
  }
  else
  {
    // Otherwise we pick the alias of the texel, renormalize the random variable and use
    // the PDF of the alias
    env_idx = sample_data.alias;
    xi.y    = (xi.y - sample_data.q) / (1.0f - sample_data.q);
    pdf     = sample_data.aliasPdf;
  }

  // Compute the 2D integer coordinates of the texel
  const uint px = env_idx % width;
  uint       py = env_idx / width;

  // Uniformly sample the solid angle subtended by the pixel.
  // Generate both the UV for texture lookup and a direction in spherical coordinates
  const float u       = float(px + xi.y) / float(width);
  const float phi     = u * (2.0f * M_PI) - M_PI;
  float       sin_phi = sin(phi);
  float       cos_phi = cos(phi);

  const float step_theta = M_PI / float(height);
  const float theta0     = float(py) * step_theta;
  const float cos_theta  = cos(theta0) * (1.0f - xi.z) + cos(theta0 + step_theta) * xi.z;
  const float theta      = acos(cos_theta);
  const float sin_theta  = sin(theta);
  const float v          = theta * M_1_OVER_PI;

  // Convert to a light direction vector in Cartesian coordinates
  to_light = vec3(cos_phi * sin_theta, cos_theta, sin_phi * sin_theta);

  // Lookup the environment value using bilinear filtering
  return texture(lat_long_tex, vec2(u, v)).xyz;
}


//-----------------------------------------------------------------------
// Sampling the HDR environment or Sun and Sky
//-----------------------------------------------------------------------
vec4 EnvSample(inout vec3 radiance)
{
  vec3  lightDir;
  float pdf;

  // Sampling the HDR with importance sampling
  vec3 randVal = vec3(rand(prd.seed), rand(prd.seed), rand(prd.seed));
  radiance     = Environment_sample(environmentTexture, randVal, lightDir, pdf);

  radiance *= rtxState.hdrMultiplier;
  return vec4(lightDir, pdf);
}


#endif  // ENV_SAMPLING_GLSL
