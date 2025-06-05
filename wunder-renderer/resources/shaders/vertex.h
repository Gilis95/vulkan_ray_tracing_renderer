#ifndef VERTEX_H
#define VERTEX_H
struct VertexAttributes {
  vec3 position;  // 12
  uint normal;    // 16 compressed using oct
  vec2 texcoord;  // 24 Tangent handiness, stored in LSB of .y
  uint tangent;   // 28 compressed using oct
  uint color;     // 32 RGBA
};


// Structure used for retrieving the primitive information in the closest hit
// using gl_InstanceCustomIndexExt
struct InstanceData {
  uint64_t vertexAddress;
  uint64_t indexAddress;
  int materialIndex;
  vec3 _pad;
};

#endif //VERTEX_H
