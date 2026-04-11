#include "wallpaper/wallpaper_setup.h"
#include "shell_layer_impl/WallpaperShellLayerStrategy.h"
#include "wallpaper/ImageWallPaperLayer.h"

namespace cf::desktop::wallpaper {

namespace {

std::unique_ptr<WallPaperLayer> make_layer() {
    return std::make_unique<ImageWallPaperLayer>();
}

} // namespace

std::unique_ptr<IShellLayerStrategy> create_wallpaper_strategy() {
    return std::make_unique<WallpaperShellLayerStrategy>(make_layer());
}

} // namespace cf::desktop::wallpaper
