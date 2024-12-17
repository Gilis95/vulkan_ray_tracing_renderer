#include "gla/vulkan/vulkan_types.h"

#include "core/wunder_macros.h"

namespace wunder::vulkan {

namespace {
template <typename resource_type, typename vulkan_resource_type>
void emplace_resource_template(
    const resource_type& image,
    vulkan_descriptor_set_bind_identifier bind_identifier,
    vulkan_descriptor_bindings::container_type descriptor_bindings) {
  auto found_resource_list_it = descriptor_bindings.find(bind_identifier);
  if (found_resource_list_it == descriptor_bindings.end()) {
    std::vector<vulkan_resource_type> images_descriptors{image.m_descriptor};
    descriptor_bindings.emplace(bind_identifier, images_descriptors);
    return;
  }

  AssertReturnUnless(holds_alternative<std::vector<vulkan_resource_type>>(
      found_resource_list_it->second));

  auto& images_descriptors = std::get<std::vector<vulkan_resource_type>>(
      found_resource_list_it->second);
  images_descriptors.emplace_back(image.m_descriptor);
}
}  // namespace

namespace shader_resource::instance {

acceleration_structure::acceleration_structure() = default;
acceleration_structure::acceleration_structure(
    acceleration_structure&& other) noexcept
    : m_descriptor(other.m_descriptor) {}

}  // namespace shader_resource::instance

void vulkan_descriptor_bindings::emplace_resource(
    vulkan_descriptor_set_bind_identifier bind_identifier,
    const shader_resource::instance::element& resource) {
  std::visit(
      overloaded{
          [this,
           bind_identifier](const std::reference_wrapper<
                            shader_resource::instance::sampled_images>& image) {
            emplace_resource_template<shader_resource::instance::sampled_images,
                                      VkDescriptorImageInfo>(
                image.get(), bind_identifier, m_bindings);
          },
          [this,
           bind_identifier](const std::reference_wrapper<
                            shader_resource::instance::acceleration_structure>&
                                acceleration_structure) {
            emplace_resource_template<
                shader_resource::instance::acceleration_structure,
                VkAccelerationStructureKHR>(
                acceleration_structure.get(), bind_identifier, m_bindings);
          },
          [this, bind_identifier](const std::reference_wrapper<
                                  shader_resource::instance::uniform_buffer>&) {
          },
          [this,
           bind_identifier](const std::reference_wrapper<
                            shader_resource::instance::storage_buffers>&) {},

          [this,
           bind_identifier](const std::reference_wrapper<
                            shader_resource::instance::separate_images>&) {},

          [this,
           bind_identifier](const std::reference_wrapper<
                            shader_resource::instance::separate_samplers>&) {},

          [this, bind_identifier](const std::reference_wrapper<
                                  shader_resource::instance::storage_images>&) {
          }},
      resource);
}

void vulkan_descriptor_bindings::clear_resource(
    vulkan_descriptor_set_bind_identifier bind_identifier) {
  auto found_binding_descriptors = m_bindings.find(bind_identifier);
  ReturnIf(found_binding_descriptors == m_bindings.end());

  // @formatter:off
  std::visit(
      overloaded{
          // @formatter:off
          [](std::vector<VkDescriptorImageInfo>& descriptors) {
            // @formatter:off
            descriptors.clear();
          },
          // @formatter:off
          [](std::vector<VkDescriptorBufferInfo>& descriptors) {
            // @formatter:off
            descriptors.clear();
          },
          // @formatter:off
          [](std::vector<VkBufferView>& descriptors) { descriptors.clear(); },
          // @formatter:off
          [](std::vector<VkAccelerationStructureKHR>&
                 descriptors) {
            // @formatter:off
            descriptors.clear();
          }},
      found_binding_descriptors->second);
  // @formatter:on
}

}  // namespace wunder::vulkan