#ifndef WUNDER_VULKAN_FRAMEBUFFER_H
#define WUNDER_VULKAN_FRAMEBUFFER_H

#include "core/non_copyable.h"
#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_texture_fwd.h"

#include <glad/vulkan.h>

namespace wunder::vulkan {
class framebuffer : public non_copyable{

  ~framebuffer();

  VkRenderPass m_render_pass;
  unique_ptr<storage_texture> m_image;
};

}
#endif  // WUNDER_VULKAN_FRAMEBUFFER_H
