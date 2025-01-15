//
// Created by christian on 1/14/25.
//

#ifndef WUNDER_VULKAN_BUFFER_FWD_H
#define WUNDER_VULKAN_BUFFER_FWD_H

namespace wunder {
namespace vulkan {

namespace shader_resource::instance{
struct storage_buffers;
struct uniform_buffer;
}

template <typename base_buffer_type>
class buffer;

using storage_buffer =
    buffer<wunder::vulkan::shader_resource::instance::storage_buffers>;

using uniform_buffer =
    buffer<wunder::vulkan::shader_resource::instance::uniform_buffer>;

template <typename base_buffer_type>
class device_buffer;

using storage_device_buffer =
    device_buffer<wunder::vulkan::shader_resource::instance::storage_buffers>;

using uniform_device_buffer =
    device_buffer<wunder::vulkan::shader_resource::instance::uniform_buffer>;

}  // namespace vulkan
}  // namespace wunder
#endif  // WUNDER_VULKAN_BUFFER_FWD_H
