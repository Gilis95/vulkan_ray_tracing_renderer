//-------------------------------------------------------------------------------------------------
// Functions implementing various tonemappers
//

const float GAMMA = 2.2;
const float INV_GAMMA = 1.0 / GAMMA;

// linear to sRGB approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
vec3 linearTosRGB(vec3 color)
{
  return pow(color, vec3(INV_GAMMA));
}

// sRGB to linear approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
vec3 sRGBToLinear(vec3 srgbIn)
{
  return vec3(pow(srgbIn.xyz, vec3(GAMMA)));
}

vec4 sRGBToLinear(vec4 srgbIn)
{
  return vec4(sRGBToLinear(srgbIn.xyz), srgbIn.w);
}

// Uncharted 2 tone map
// see: http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 toneMapUncharted2Impl(vec3 color)
{
  const float A = 0.15;
  const float B = 0.50;
  const float C = 0.10;
  const float D = 0.20;
  const float E = 0.02;
  const float F = 0.30;
  return ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
}

vec3 toneMapUncharted(vec3 color)
{
  const float W = 11.2;
  const float exposure = 2.0;

  color = toneMapUncharted2Impl(color * exposure);

  vec3 whiteScale = 1.0 / toneMapUncharted2Impl(vec3(W));
  color *= whiteScale;

  return linearTosRGB(color);
}

vec3 toneMap(vec3 color, float u_Exposure)
{
  color *= u_Exposure;

  return toneMapUncharted(color);
}
