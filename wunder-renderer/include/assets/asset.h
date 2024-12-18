#ifndef WUNDER_ASSET_H
#define WUNDER_ASSET_H

#include "assets/scene_asset.h"
#include "camera_asset.h"
#include "light_asset.h"
#include "material_asset.h"
#include "mesh_asset.h"
#include "texture_asset.h"

namespace wunder {
using asset = std::variant<scene_asset, camera_asset, light_asset, material_asset,
                           mesh_asset, texture_asset>;
}
#endif  // WUNDER_ASSET_H
