#include "gla/vulkan/vulkan_command_pool.h"

#include "core/wunder_macros.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_physical_device.h"

namespace wunder {

vulkan_command_pool::vulkan_command_pool() {
  vulkan_context& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& physical_device = vulkan_context.get_physical_device();
  auto& logical_device = vulkan_context.get_device();

  VkCommandPoolCreateInfo cmd_pool_info = {};
  cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  cmd_pool_info.queueFamilyIndex =
      physical_device.get_queue_family_indices().Graphics;
  cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  VkDevice vulkan_logical_device = logical_device.get_vulkan_logical_device();
  AssertReturnUnless(vkCreateCommandPool(vulkan_logical_device, &cmd_pool_info,
                                         nullptr, &m_graphics_command_pool) ==
                     VkResult::VK_SUCCESS);

  cmd_pool_info.queueFamilyIndex =
      physical_device.get_queue_family_indices().Compute;
  AssertReturnUnless(vkCreateCommandPool(vulkan_logical_device, &cmd_pool_info,
                                         nullptr, &m_compute_command_pool) ==
                     VkResult::VK_SUCCESS);
}

vulkan_command_pool::~vulkan_command_pool() {
  auto& logical_device = vulkan_layer_abstraction_factory::instance()
                             .get_vulkan_context()
                             .get_device();

  auto vulkanDevice = logical_device.get_vulkan_logical_device();

  vkDestroyCommandPool(vulkanDevice, m_graphics_command_pool, nullptr);
  vkDestroyCommandPool(vulkanDevice, m_compute_command_pool, nullptr);
}

VkCommandBuffer vulkan_command_pool::allocate_command_buffer(bool begin,
                                                             VkCommandPool& out_pool, VkCommandBuffer& out_buffer) {
  auto& logical_device = vulkan_layer_abstraction_factory::instance()
                             .get_vulkan_context()
                             .get_device();

  auto vulkan_logical_device = logical_device.get_vulkan_logical_device();

  VkCommandBuffer command_buffer;

  VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
  command_buffer_allocate_info.sType =
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_allocate_info.commandPool = out_pool;
  command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_allocate_info.commandBufferCount = 1;

  AssertReturnUnless(
      vkAllocateCommandBuffers(
          vulkan_logical_device, &command_buffer_allocate_info,&out_buffer) == VkResult::VK_SUCCESS,
      nullptr);

  // If requested, also start the new command buffer
  ReturnUnless(begin, command_buffer);

  VkCommandBufferBeginInfo command_buffer_begin_info{};
  command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  AssertReturnUnless(
      vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info) ==
          VkResult::VK_SUCCESS,
      nullptr);

  return command_buffer;
}

VkCommandBuffer vulkan_command_pool::allocate_graphics_command_buffer(bool begin)
{

}
VkCommandBuffer vulkan_command_pool::allocate_compute_command_buffer(bool begin)
{

}

void vulkan_command_pool::flush_graphics_command_buffer() {
  auto& logical_device = vulkan_layer_abstraction_factory::instance()
                             .get_vulkan_context()
                             .get_device();

  flush_command_buffer(m_current_graphics_command_buffer,
                       logical_device.get_graphics_queue());
}

void vulkan_command_pool::flush_compute_command_buffer() {
  auto& logical_device = vulkan_layer_abstraction_factory::instance()
                             .get_vulkan_context()
                             .get_device();

  flush_command_buffer(m_current_compute_command_buffer,
                       logical_device.get_compute_queue());
}

void vulkan_command_pool::flush_command_buffer(VkCommandBuffer command_buffer,
                                               VkQueue queue) {
  auto& logical_device = vulkan_layer_abstraction_factory::instance()
                             .get_vulkan_context()
                             .get_device();

  auto vulkan_logical_device = logical_device.get_vulkan_logical_device();

  const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

  AssertReturnIf(command_buffer == VK_NULL_HANDLE);
  AssertReturnIf(vkEndCommandBuffer(command_buffer) != VkResult::VK_SUCCESS);

  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer;

  // Create fence to ensure that the command buffer has finished executing
  VkFenceCreateInfo fenceCreateInfo = {};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = 0;
  VkFence fence;
  AssertReturnIf(vkCreateFence(vulkan_logical_device, &fenceCreateInfo, nullptr,
                               &fence) != VkResult::VK_SUCCESS);

  {
    // Submit to the queue
    AssertReturnIf(vkQueueSubmit(queue, 1, &submit_info, fence) !=
                   VkResult::VK_SUCCESS);
  }
  // Wait for the fence to signal that command buffer has finished executing
  AssertReturnIf(vkWaitForFences(vulkan_logical_device, 1, &fence, VK_TRUE,
                                 DEFAULT_FENCE_TIMEOUT) !=
                 VkResult::VK_SUCCESS);

  vkDestroyFence(vulkan_logical_device, fence, nullptr);
  vkFreeCommandBuffers(vulkan_logical_device, m_graphics_command_pool, 1,
                       &command_buffer);
}

}  // namespace wunder