//-------------------------------------------------------------------------------------------------
// This is called by the post process shader to display the result of ray tracing.
// It applied a tonemapper and do dithering on the image to avoid banding.

#version 450
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_debug_printf : enable
#extension GL_ARB_gpu_shader_int64 : enable  // Shader reference


#define TONEMAP_UNCHARTED
#include "../random.glsl"
#include "../tonemapping.glsl"
#include "../host_device.h"


layout(location = 0) in vec2 uvCoords;
layout(location = 0) out vec4 fragColor;

layout(set = 0, binding = 0) uniform sampler2D rtxGeneratedImage;

layout(push_constant) uniform _Tonemapper
{
  Tonemapper tm;
};

// http://www.thetenthplanet.de/archives/5367
// Apply dithering to hide banding artifacts.
vec3 dither(vec3 linear_color, vec3 noise, float quant)
{
  vec3 c0    = floor(linearTosRGB(linear_color) / quant) * quant;
  vec3 c1    = c0 + quant;
  vec3 discr = mix(sRGBToLinear(c0), sRGBToLinear(c1), noise);
  return mix(c0, c1, lessThan(discr, linear_color));
}


// http://user.ceng.metu.edu.tr/~akyuz/files/hdrgpu.pdf
const mat3 RGB2XYZ = mat3(0.4124564, 0.3575761, 0.1804375, 0.2126729, 0.7151522, 0.0721750, 0.0193339, 0.1191920, 0.9503041);
float luminance(vec3 color)
{
  return dot(color, vec3(0.2126f, 0.7152f, 0.0722f));  //color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;
}


void main()
{
  // Raw result of ray tracing
  vec4 hdr = texture(rtxGeneratedImage, uvCoords * tm.zoom).rgba;

  // Tonemap + Linear to sRgb
  vec3 color = toneMap(hdr.rgb, tm.avgLum);

  // contrast
  color = clamp(mix(vec3(0.5), color, tm.contrast), 0, 1);
  // brighness
  color = pow(color, vec3(1.0 / tm.brightness));
  // saturation
  vec3 i = vec3(dot(color, vec3(0.299, 0.587, 0.114)));
  color  = mix(i, color, tm.saturation);
  // vignette
  vec2 uv = ((uvCoords * tm.renderingRatio) - 0.5) * 2.0;
  color *= 1.0 - dot(uv, uv) * tm.vignette;

  fragColor.xyz = color;
  fragColor.a   = hdr.a;
}
