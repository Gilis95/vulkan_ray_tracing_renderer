//-------------------------------------------------------------------------------------------------
// This file as all constant, global values and structures not shared with CPP

#ifndef GLOBALS_GLSL
#define GLOBALS_GLSL 1

#define PI 3.14159265358979323
#define TWO_PI 6.28318530717958648
#define INFINITY 1e32
#define EPS 0.0001

//precision highp int;
precision highp float;

const float M_PI        = 3.14159265358979323846;   // pi
const float M_TWO_PI    = 6.28318530717958648;      // 2*pi
const float M_PI_2      = 1.57079632679489661923;   // pi/2
const float M_PI_4      = 0.785398163397448309616;  // pi/4
const float M_1_OVER_PI = 0.318309886183790671538;  // 1/pi
const float M_2_OVER_PI = 0.636619772367581343076;  // 2/pi


#define RngStateType uint  // Random type

//-----------------------------------------------------------------------
struct Ray
{
  vec3 origin;
  vec3 direction;
};


struct PtPayload
{
  uint   seed; //4
  float  hitT; //8
  int    primitiveID; //12
  int    instanceID; //16
  int    instanceCustomIndex; //20
  vec2   baryCoord; //28
  mat4x3 objectToWorld; //76
  mat4x3 worldToObject; //124
  int    _pad;
};

struct ShadowHitPayload
{
  RngStateType seed;
  bool         isHit;
};

// This material is the shading material after applying textures and any
// other operation. This structure is filled in gltfmaterial.glsl
struct Material
{
  vec3  albedo;
  float specular;
  vec3  emission;
  float anisotropy;
  float metallic;
  float roughness;
  float subsurface;
  float specularTint;
  float sheen;
  vec3  sheenTint;
  float clearcoat;
  float clearcoatRoughness;
  float transmission;
  float ior;
  vec3  attenuationColor;
  float attenuationDistance;

  // Roughness calculated from anisotropic
  float ax;
  float ay;
  // ----
  vec3  f0; //specular facotr
  float alpha; //surface roughness
  bool  unlit;
  bool  thinwalled;
};

// From shading state, this is the structure pass to the eval functions
struct State
{
  int   depth;
  float eta;

  vec3 position;
  vec3 normal;
  vec3 ffnormal;
  vec3 tangent;
  vec3 bitangent;
  vec2 texCoord;

  bool isEmitter;
  bool specularBounce;
  bool isSubsurface;

  uint     matID;
  Material mat;
};


//-----------------------------------------------------------------------
struct BsdfSampleRec
{
  vec3  L;
  vec3  f;
  float pdf;
};

//-----------------------------------------------------------------------
struct LightSampleRec
{
  vec3  surfacePos;
  vec3  normal;
  vec3  emission;
  float pdf;
};


#endif  // GLOBALS_GLSL
