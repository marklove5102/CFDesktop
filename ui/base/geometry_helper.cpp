/**
 * @file geometry_helper.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Geometry Helpers for UI Widgets (Rounded Rects)
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "geometry_helper.h"
#include <QRectF>

namespace cf::ui::base {
namespace geometry {

// Shape scale to dp radius mapping (Material Design specification)
namespace {
constexpr float SHAPE_NONE_RADIUS = 0.0f;
constexpr float SHAPE_EXTRA_SMALL_RADIUS = 4.0f;
constexpr float SHAPE_SMALL_RADIUS = 8.0f;
constexpr float SHAPE_MEDIUM_RADIUS = 12.0f;
constexpr float SHAPE_LARGE_RADIUS = 16.0f;
constexpr float SHAPE_EXTRA_LARGE_RADIUS = 28.0f;
} // namespace

static float radiusForScale(ShapeScale scale) {
    switch (scale) {
        case ShapeScale::ShapeNone:
            return SHAPE_NONE_RADIUS;
        case ShapeScale::ShapeExtraSmall:
            return SHAPE_EXTRA_SMALL_RADIUS;
        case ShapeScale::ShapeSmall:
            return SHAPE_SMALL_RADIUS;
        case ShapeScale::ShapeMedium:
            return SHAPE_MEDIUM_RADIUS;
        case ShapeScale::ShapeLarge:
            return SHAPE_LARGE_RADIUS;
        case ShapeScale::ShapeExtraLarge:
            return SHAPE_EXTRA_LARGE_RADIUS;
        case ShapeScale::ShapeFull:
            return 0.0f; // Handled separately as percentage
    }
    return 0.0f;
}

QPainterPath roundedRect(const QRectF& rect, ShapeScale scale) {
    QPainterPath path;

    if (scale == ShapeScale::ShapeFull) {
        // Full rounded: capsule/circle shape (50% of height)
        float radius = static_cast<float>(rect.height()) * 0.5f;
        path.addRoundedRect(rect, radius, radius);
    } else {
        float radius = radiusForScale(scale);
        path.addRoundedRect(rect, radius, radius);
    }

    return path;
}

QPainterPath roundedRect(const QRectF& rect, float radius) {
    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);
    return path;
}

QPainterPath roundedRect(const QRectF& rect, float topLeft, float topRight, float bottomLeft,
                         float bottomRight) {
    // Custom rounded rect with asymmetric corner radii
    QPainterPath path;

    float w = static_cast<float>(rect.width());
    float h = static_cast<float>(rect.height());
    float x = static_cast<float>(rect.x());
    float y = static_cast<float>(rect.y());

    // Start from top-left edge (after corner arc)
    path.moveTo(x + topLeft, y);

    // Top edge
    path.lineTo(x + w - topRight, y);

    // Top-right corner
    if (topRight > 0) {
        path.quadTo(x + w, y, x + w, y + topRight);
    } else {
        path.lineTo(x + w, y);
    }

    // Right edge
    path.lineTo(x + w, y + h - bottomRight);

    // Bottom-right corner
    if (bottomRight > 0) {
        path.quadTo(x + w, y + h, x + w - bottomRight, y + h);
    } else {
        path.lineTo(x + w, y + h);
    }

    // Bottom edge
    path.lineTo(x + bottomLeft, y + h);

    // Bottom-left corner
    if (bottomLeft > 0) {
        path.quadTo(x, y + h, x, y + h - bottomLeft);
    } else {
        path.lineTo(x, y + h);
    }

    // Left edge
    path.lineTo(x, y + topLeft);

    // Top-left corner
    if (topLeft > 0) {
        path.quadTo(x, y, x + topLeft, y);
    } else {
        path.lineTo(x, y);
    }

    path.closeSubpath();
    return path;
}

} // namespace geometry
} // namespace cf::ui::base
