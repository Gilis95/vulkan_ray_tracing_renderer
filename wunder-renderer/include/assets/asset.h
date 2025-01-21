#ifndef WUNDER_ASSET_H
#define WUNDER_ASSET_H

#include "assets/scene_asset.h"
#include "assets/camera_asset.h"
#include "assets/light_asset.h"
#include "assets/material_asset.h"
#include "assets/mesh_asset.h"
#include "assets/texture_asset.h"

namespace wunder {
using asset = std::variant<scene_asset, camera_asset, light_asset, material_asset,
                           mesh_asset, texture_asset, environment_texture_asset>;
}
#endif  // WUNDER_ASSET_H
