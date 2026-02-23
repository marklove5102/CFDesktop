/**
 * @file device_pixel.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Device Pixel Ratio Helper for UI Units Conversion
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "device_pixel.h"

namespace cf::ui::base {
namespace device {

CanvasUnitHelper::CanvasUnitHelper(const qreal devicePixelRatio)
    : devicePixelRatio(devicePixelRatio) {}

qreal CanvasUnitHelper::dpToPx(qreal dp) const {
    // Density-independent pixel to physical pixel
    return dp * devicePixelRatio;
}

qreal CanvasUnitHelper::spToPx(qreal sp) const {
    // Scale-independent pixel to physical pixel (for fonts)
    return sp * devicePixelRatio;
}

qreal CanvasUnitHelper::pxToDp(qreal px) const {
    // Physical pixel to density-independent pixel
    return devicePixelRatio > 0 ? px / devicePixelRatio : px;
}

qreal CanvasUnitHelper::dpi() const {
    // Standard DPI is 96, scale by device pixel ratio
    return 96.0 * devicePixelRatio;
}

CanvasUnitHelper::BreakPoint CanvasUnitHelper::breakPoint(qreal widthDp) {
    if (widthDp < 600.0) {
        return BreakPoint::Compact;
    }
    if (widthDp < 840.0) {
        return BreakPoint::Medium;
    }
    return BreakPoint::Expanded;
}

} // namespace device
} // namespace cf::ui::base
