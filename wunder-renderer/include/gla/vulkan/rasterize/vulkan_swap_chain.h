#ifndef WUNDER_VULKAN_SWAP_CHAIN_H
#define WUNDER_VULKAN_SWAP_CHAIN_H

#include <glad/vulkan.h>

#include <cstdint>
#include <optional>
#include <vector>

#include "core/non_copyable.h"
#include "core/wunder_memory.h"

namespace wunder::vulkan {
class render_pass;

class swap_chain : public non_copyable {
 public:
  struct queue_element : public non_copyable {
   public:
    struct semaphore_entry {
      VkSemaphore read_semaphore = VK_NULL_HANDLE;
      VkSemaphore written_semaphore = VK_NULL_HANDLE;
    };

   public:
    queue_element();
    queue_element(queue_element&& other);
    ~queue_element();

   public:
    VkImage m_image = VK_NULL_HANDLE;
    VkImageView m_image_view = VK_NULL_HANDLE;
    VkCommandBuffer m_command_buffer = VK_NULL_HANDLE;
    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

    VkImageMemoryBarrier m_barrier{};
    VkFence m_fence = VK_NULL_HANDLE;
    semaphore_entry m_semaphore_entry;
  };

 public:
  swap_chain(std::uint32_t width, std::uint32_t height);
  ~swap_chain() override;

 public:
  void resize(uint32_t width, uint32_t height);

  void init();
  void shutdown();

 public:
  std::optional<std::uint32_t> acquire();

  void begin_command_buffer();
  VkCommandBuffer get_current_command_buffer();
  void flush_current_command_buffer();

 public:
  void begin_render_pass() const;
  void end_render_pass() const;

  render_pass& mutable_render_pass();

 private:
  void initialize_render_pass();

  void initialize_swap_chain();

  void initialize_queue_elements();
  void create_image_for_each_queue_element();
  void create_image_barrier_for_each_queue_element();
  void create_semaphores_for_each_queue_element();
  void create_frame_buffer_for_each_queue_element();
  void create_fence_for_each_queue_element();
  void create_command_buffer_for_each_queue_element();
  void initialize_depth_buffer();

  void update_barriers() const;
  void wait_idle() const;
  void wait_element_to_rendered(size_t element_idx);

 private:
  bool m_vsync_enabled;
  uint32_t m_width, m_height;

  uint32_t m_current_queue_element;

  std::vector<queue_element> m_queue_elements;

  unique_ptr<render_pass> m_render_pass;
  VkSurfaceKHR m_surface;
  VkSwapchainKHR m_swap_chain;
  VkCommandPool m_command_pool;

  VkImage m_depth_image;          // Depth/Stencil
  VkDeviceMemory m_depth_memory;  // Depth/Stencil
  VkImageView m_depth_view;       // Depth/Stencil

  VkFormat m_colour_format{VK_FORMAT_B8G8R8A8_UNORM};
  VkColorSpaceKHR m_color_space;
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_SWAP_CHAIN_H
