//
// Created by christian on 7/3/24.
//

#ifndef WUNDER_WUNDER_RENDERER_INCLUDE_GLA_RENDERER_PROPERTIES_H_
#define WUNDER_WUNDER_RENDERER_INCLUDE_GLA_RENDERER_PROPERTIES_H_

namespace wunder {
enum class driver { Inavlid = -1, Vulkan, Count };

enum class renderer_type { Inavlid = -1, RAY_TRACE, Rasterize };

enum class gpu_to_use { Invalid = -1, Integrated, Dedicated, Count };

struct renderer_properties {
 public:
  driver m_driver;
  renderer_type m_renderer;
  gpu_to_use m_gpu_to_use;
  bool m_enable_validation;
};
}  // namespace wunder
#endif  // WUNDER_WUNDER_RENDERER_INCLUDE_GLA_RENDERER_PROPERTIES_H_
