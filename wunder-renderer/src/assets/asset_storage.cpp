#include "assets/asset_storage.h"

#include "assets/asset_types.h"
#include "event/asset_events.h"
#include "event/event_controller.h"

namespace wunder {

static asset_handle s_asset_counter = 0;

asset_handle asset_storage::add_asset(asset&& _asset) {
  auto current_asset_id = s_asset_counter++;

  m_assets.emplace(current_asset_id, std::forward<asset>(_asset));
  event_controller::on_event(asset_loaded{.m_asset_handle = current_asset_id});

  return current_asset_id;
}

}  // namespace wunder