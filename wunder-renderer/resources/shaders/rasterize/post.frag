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

vec3 toneExposure(vec3 RGB, float logAvgLum)
{
  vec3  XYZ = RGB2XYZ * RGB;
  float Y   = (tm.key / logAvgLum) * XYZ.y;
  float Yd  = (Y * (1.0 + Y / (tm.Ywhite * tm.Ywhite))) / (1.0 + Y);
  return RGB / XYZ.y * Yd;
}

vec3 toneLocalExposure(vec3 RGB, float logAvgLum)
{
  vec3  XYZ = RGB2XYZ * RGB;
  float Y   = (tm.key / logAvgLum) * XYZ.y;
  float La;  // local adaptation luminance
  float factor  = tm.key / logAvgLum;
  float epsilon = 0.05, phi = 2.0;
  float scale[7] = float[7](1, 2, 4, 8, 16, 32, 64);
  for(int i = 0; i < 7; ++i)
  {
    float v1 = luminance(texture(rtxGeneratedImage, uvCoords * tm.zoom, i).rgb) * factor;
    float v2 = luminance(texture(rtxGeneratedImage, uvCoords * tm.zoom, i + 1).rgb) * factor;
    if(abs(v1 - v2) / ((tm.key * pow(2, phi) / (scale[i] * scale[i])) + v1) > epsilon)
    {
      La = v1;
      break;
    }
    else
      La = v2;
  }
  float Yd = Y / (1.0 + La);

  return RGB / XYZ.y * Yd;
}


void main()
{
  // Raw result of ray tracing
  vec4 hdr = texture(rtxGeneratedImage, uvCoords * tm.zoom).rgba;

  if(((tm.autoExposure >> 0) & 1) == 1)
  {
    vec4  avg     = textureLod(rtxGeneratedImage, vec2(0.5), 20);  // Get the average value of the image
    float avgLum2 = luminance(avg.rgb);                  // Find the luminance
    if(((tm.autoExposure >> 1) & 1) == 1)
      hdr.rgb = toneLocalExposure(hdr.rgb, avgLum2);  // Adjust exposure
    else
      hdr.rgb = toneExposure(hdr.rgb, avgLum2);  // Adjust exposure
  }

  // Tonemap + Linear to sRgb
  vec3 color = toneMap(hdr.rgb, tm.avgLum);

  // Remove banding
  if(tm.dither > 0)
  {
    // Generates a 3D random number using the PCG (Permuted Congruential Generator) algorithm
    uvec3 r = pcg3d(uvec3(gl_FragCoord.xy, 0));

    // The HEX value 0x3f800000 corresponds to the 32-bit floating-point representation of 1.0f.
    // It is bitwise ORed into the lower 23 bits of the 32-bit floating-point format,
    // following the IEEE 754 standard (1 sign bit, 8 exponent bits, 23 mantissa bits).
    // This operation effectively sets the exponent to 0 (bias of 127), generating a float between 1.0 and 2.0.
    // The uintBitsToFloat function then interprets this bit pattern as a floating-point number,
    // and finally, 1.0 is subtracted to bring the range to (0.0, 1.0).
    vec3 noise = uintBitsToFloat(0x3f800000 | (r >> 9)) - 1.0f;

    // Apply dithering to hide banding artifacts.
    color = dither(sRGBToLinear(color), noise, 1. / 255.);
  }

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
