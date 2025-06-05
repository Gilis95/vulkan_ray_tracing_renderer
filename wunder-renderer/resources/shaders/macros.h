#ifndef MACROS_H
#define MACROS_H

// clang-format off
#ifdef __cplusplus  // Descriptor binding helper for C++ and GLSL
#include <stdint.h>

#include <glm/glm.hpp>

#include "assets/material_asset.h"
#include "assets/mesh_asset.h"
#include "enums.h"

// GLSL Type
using ivec2 = glm::ivec2;
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;
using uint = unsigned int;


#define START_ENUM(a)                                                                                               \
enum a                                                                                                               \
{
#define END_ENUM() }

#define PUBLIC public:

#else
#define START_ENUM(a) const uint
#define END_ENUM()

#define PUBLIC
#endif


#endif //MACROS_H
