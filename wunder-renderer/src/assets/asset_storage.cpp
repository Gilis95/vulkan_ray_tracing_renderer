#include "assets/asset_storage.h"

#include "assets/asset_types.h"
#include "event/asset_events.h"
#include "event/event_controller.h"

namespace wunder {

static asset_handle::type s_asset_counter = asset_handle::s_invalid + 1;

asset_handle asset_storage::add_asset(asset&& _asset) {
  auto current_asset_id = s_asset_counter++;
  const asset_handle handle = asset_handle(current_asset_id);

  m_assets.emplace(current_asset_id, std::forward<asset>(_asset));
  event_controller::on_event(asset_loaded{.m_asset_handle = handle});

  return handle;
}

}  // namespace wunder