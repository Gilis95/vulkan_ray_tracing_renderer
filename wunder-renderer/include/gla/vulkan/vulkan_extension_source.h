#ifndef WUNDER_VULKAN_EXTENSION_SOURCE_H
#define WUNDER_VULKAN_EXTENSION_SOURCE_H

#include <string>
#include <vector>

namespace wunder::vulkan {
struct vulkan_extension_data;
class vulkan_extension_source {
 public:
  virtual ~vulkan_extension_source() = default;

 public:
  [[nodiscard]] virtual bool is_extension_supported(
      const std::string &extensionName) const = 0;
};

void add_supported_extensions(
    const vulkan_extension_source &source,
    const std::vector<vulkan_extension_data> &requested_extensions,
    std::vector<vulkan_extension_data> &out_used_extensions);

void extract_extensions_names(
    std::vector<vulkan_extension_data> &used_extensions,
    std::vector<char *> &out_extension_names);

void extract_used_features_from_extensions(
    const std::vector<vulkan_extension_data> &used_extensions,
    std::vector<void *> &out_used_features);

}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_EXTENSION_SOURCE_H
