//
// Created by christian on 7/3/24.
//

#ifndef WUNDER_WUNDER_RENDERER_INCLUDE_GLA_RENDERER_PROPERTIES_H_
#define WUNDER_WUNDER_RENDERER_INCLUDE_GLA_RENDERER_PROPERTIES_H_

namespace wunder {
enum class gla_type { Inavlid = -1, Vulkan, Count };

enum class gpu_to_use { Invalid = -1, Integrated, Dedicated, Count };

struct renderer_properties {
 public:
  gla_type m_renderer_type;
  gpu_to_use m_gpu_to_use;
  bool m_enable_validation;
};
}  // namespace wunder
#endif  // WUNDER_WUNDER_RENDERER_INCLUDE_GLA_RENDERER_PROPERTIES_H_
