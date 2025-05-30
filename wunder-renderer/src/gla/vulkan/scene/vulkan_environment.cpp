#include "gla/vulkan/scene/vulkan_environment.h"

#include "core/wunder_macros.h"
#include "gla/vulkan/vulkan_buffer.h"
#include "gla/vulkan/vulkan_texture.h"

namespace wunder::vulkan {

vulkan_environment::~vulkan_environment() {
  if (m_image) {
    m_image.reset();
  }

  if (m_acceleration_data.m_buffer) {
    m_acceleration_data.m_buffer.reset();
  }
}

void vulkan_environment::add_descriptor_to(descriptor_set_manager& target) const {
  AssertReturnUnless(m_image);
  AssertReturnUnless(m_acceleration_data.m_buffer);

  m_image->add_descriptor_to(target);
  m_acceleration_data.m_buffer->add_descriptor_to(target);
}

}