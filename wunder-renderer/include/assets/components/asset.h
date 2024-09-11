#ifndef WUNDER_ASSET_H
#define WUNDER_ASSET_H

#include "assets/components/camera_asset.h"
#include "assets/components/light_asset.h"
#include "assets/components/material_asset.h"
#include "assets/components/mesh_asset.h"
#include "assets/components/texture_asset.h"
#include "assets/scene_asset.h"

namespace wunder {
using asset = std::variant<scene_asset, camera_asset, light_asset, material_asset,
                           mesh_asset, texture_asset>;
}
#endif  // WUNDER_ASSET_H
