#include "gla/vulkan/vulkan_extension_source.h"

#include <algorithm>
#include <vector>

#include "core/wunder_logger.h"
#include "core/wunder_macros.h"
#include "gla/vulkan/vulkan_types.h"

namespace wunder {
void add_supported_extensions(
    const vulkan_extension_source& source,
    const std::vector<vulkan_extension_data>& requested_extensions,
    std::vector<vulkan_extension_data>& used_extensions) {
  unsigned long requested_extensions_count = requested_extensions.size();
  used_extensions.reserve(requested_extensions_count);

  for (size_t i = 0; i < requested_extensions_count; ++i) {
    const auto& requested_extension = requested_extensions[i];
    if (source.is_extension_supported(requested_extension.m_name)) {
      used_extensions.push_back(requested_extension);
      continue;
    }

    if (requested_extension.m_optional == true) {
      continue;
    }

    WUNDER_ERROR_TAG("Renderer",
                     "Mandatory requested extension is unavailable {0}",
                     requested_extension.m_name);
    CrashIf(true);
  }
}

void extract_extensions_names(
    std::vector<vulkan_extension_data>& used_extensions,
    std::vector<char*>& out_extension_names) {
  out_extension_names.reserve(used_extensions.size());
  std::transform(
      used_extensions.begin(), used_extensions.end(),
      std::back_inserter(out_extension_names),
      [](vulkan_extension_data& data) { return data.m_name.data(); });
}

void extract_used_features_from_extensions(
    const std::vector<vulkan_extension_data>& used_extensions,
    std::vector<void*>& out_used_features) {
  std::vector<vulkan_extension_data> used_extensions_with_features;
  std::copy_if(
      used_extensions.begin(), used_extensions.end(),
      std::back_inserter(used_extensions_with_features),
      [](const vulkan_extension_data& data) { return data.m_feature_struct; });

  out_used_features.reserve(used_extensions_with_features.size());
  std::transform(
      used_extensions_with_features.begin(),
      used_extensions_with_features.end(),
      std::back_inserter(out_used_features),
      [](const vulkan_extension_data& data) { return data.m_feature_struct; });
}

}  // namespace wunder