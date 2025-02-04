#include "assets/texture_asset.h"

#include "core/wunder_macros.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"

#include "gla/vulkan/vulkan_memory_allocator.h"

namespace wunder {
bool texture_data::is_empty() const {
  return std::visit(overloaded{[](const std::vector<unsigned char>& pixels) {
                                 return pixels.empty();
                               },
                               [](const std::vector<float>& pixels) {
                                 return pixels.empty();
                               }},
                    m_data);
}
size_t texture_data::size() const {
  return std::visit(overloaded{[](const std::vector<unsigned char>& pixels) {
                                 return pixels.size() * sizeof(unsigned char);
                               },
                               [](const std::vector<float>& pixels) {
                                 return pixels.size() * sizeof(float);
                               }},
                    m_data);
}

void texture_data::copy_to(VmaAllocation& stagingBufferAllocation) const {
  auto& vulkan_context =
      vulkan::layer_abstraction_factory::instance().get_vulkan_context();
  auto& allocator = vulkan_context.mutable_resource_allocator();

  return std::visit(
      overloaded{[&stagingBufferAllocation,
                  &allocator](const std::vector<unsigned char>& pixels) {
                   auto* dest_data =
                       allocator.map_memory<uint8_t>(stagingBufferAllocation);
                   memcpy(dest_data, pixels.data(), pixels.size());

                   return;
                 },
                 [&stagingBufferAllocation,
                  &allocator](const std::vector<float>& pixels) {
                   auto* dest_data =
                       allocator.map_memory<float_t>(stagingBufferAllocation);
                   memcpy(dest_data, pixels.data(), pixels.size());

                   return;
                 }},
      m_data);
}
}  // namespace wunder