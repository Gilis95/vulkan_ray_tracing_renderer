#ifndef WUNDER_VULKAN_TEXTURE_FWD_H
#define WUNDER_VULKAN_TEXTURE_FWD_H

namespace wunder::vulkan {

namespace shader_resource::instance {
struct sampled_image;
struct storage_image;
}

template <typename base_texture>
class texture;

using sampled_texture =
    texture<wunder::vulkan::shader_resource::instance::sampled_image>;

using storage_texture =
    texture<wunder::vulkan::shader_resource::instance::storage_image>;
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_TEXTURE_FWD_H
