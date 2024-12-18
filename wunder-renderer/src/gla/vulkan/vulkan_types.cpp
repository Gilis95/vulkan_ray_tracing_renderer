#include "gla/vulkan/vulkan_types.h"

#include "core/wunder_macros.h"

namespace wunder::vulkan {

namespace {

template <typename resource_type, typename vulkan_resource_type>
void add_resource_template(
    const resource_type& resource,
    wunder::vulkan::shader_resource::instance::resource_list& resource_list) {
  AssertReturnUnless(
      holds_alternative<std::vector<vulkan_resource_type>>(resource_list));

  auto& images_descriptors =
      std::get<std::vector<vulkan_resource_type>>(resource_list);
  images_descriptors.emplace_back(resource.m_descriptor);
}
}  // namespace

namespace shader_resource::instance {

acceleration_structure::acceleration_structure() = default;
acceleration_structure::acceleration_structure(
    acceleration_structure&& other) noexcept
    : m_descriptor(other.m_descriptor) {}

}  // namespace shader_resource::instance

void vulkan_descriptor_binding::emplace_resource(
    const shader_resource::instance::element& resource) {
  std::visit(
      overloaded{
          [this](const std::reference_wrapper<
                 shader_resource::instance::sampled_images>& image) {
            initialize_if_empty(std::vector<VkDescriptorImageInfo>(),
                                VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);

            AssertReturnUnless(m_descriptor_type ==
                               VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);

            add_resource_template<shader_resource::instance::sampled_images,
                                  VkDescriptorImageInfo>(image.get(),
                                                         m_resources);
          },
          [this](const std::reference_wrapper<
                 shader_resource::instance::acceleration_structure>&
                     acceleration_structure) {
            initialize_if_empty(std::vector<VkAccelerationStructureKHR>(),
                                VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR);

            AssertReturnUnless(m_descriptor_type ==
                               VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR);

            add_resource_template<
                shader_resource::instance::acceleration_structure,
                VkAccelerationStructureKHR>(acceleration_structure.get(),
                                            m_resources);
          },

          [this](const std::reference_wrapper<
                 shader_resource::instance::uniform_buffer>&) {},

          [this](const std::reference_wrapper<
                 shader_resource::instance::storage_buffers>& buffer) {
            initialize_if_empty(std::vector<VkDescriptorBufferInfo>(),
                                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

            AssertReturnUnless(m_descriptor_type ==
                               VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

            add_resource_template<shader_resource::instance::storage_buffers,
                                  VkDescriptorBufferInfo>(buffer.get(),
                                                          m_resources);
          },

          [this](const std::reference_wrapper<
                 shader_resource::instance::separate_images>&) {},

          [this](const std::reference_wrapper<
                 shader_resource::instance::separate_samplers>&) {},

          [this](const std::reference_wrapper<
                 shader_resource::instance::storage_images>&) {}},
      resource);
}
void vulkan_descriptor_binding::initialize_if_empty(
    shader_resource::instance::resource_list default_value,
    VkDescriptorType descriptor_type) {
  ReturnUnless(m_descriptor_type == VK_DESCRIPTOR_TYPE_MAX_ENUM);

  m_resources = std::move(default_value);
  m_descriptor_type = descriptor_type;
}
void vulkan_descriptor_binding::clear_resources() {
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
          [](std::vector<VkAccelerationStructureKHR>& descriptors) {
            // @formatter:off
            descriptors.clear();
          }},
      m_resources);
  // @formatter:on

  m_descriptor_type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

void vulkan_descriptor_bindings::emplace_resource(
    vulkan_descriptor_set_bind_identifier bind_identifier,
    const shader_resource::instance::element& resource) {
  auto& binding = m_bindings[bind_identifier];
  binding.emplace_resource(resource);
}

void vulkan_descriptor_bindings::clear_resource(
    vulkan_descriptor_set_bind_identifier bind_identifier) {
  auto found_binding_descriptors = m_bindings.find(bind_identifier);
  ReturnIf(found_binding_descriptors == m_bindings.end());

  found_binding_descriptors->second.clear_resources();
}

}  // namespace wunder::vulkan