#ifndef WUNDER_ASSET_EVENTS_H
#define WUNDER_ASSET_EVENTS_H

#include "assets/asset_types.h"

namespace wunder::event {
struct asset_loaded {
  asset_handle m_asset_handle;
};
}
#endif  // WUNDER_ASSET_EVENTS_H
