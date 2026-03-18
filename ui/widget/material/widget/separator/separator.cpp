/**
 * @file separator.cpp
 * @brief Material Design 3 Separator Implementation
 *
 * Implements a Material Design 3 separator (divider) with support for
 * horizontal and vertical orientations. The separator is drawn as a 1dp
 * line using OUTLINE color with 40% opacity for better visibility. Supports three
 * spacing modes: Full-bleed, Inset, and Middle-inset.
 *
 * @author Material Design Framework Team
 * @date 2026-03-18
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "separator.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"

#include <QApplication>
#include <QPaintEvent>
#include <QPainter>

namespace cf::ui::widget::material {

using namespace cf::ui::base;
using namespace cf::ui::base::device;
using namespace cf::ui::core;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::application_support;

// ============================================================================
// Constructor / Destructor
// ============================================================================

Separator::Separator(Qt::Orientation orientation, QWidget* parent)
    : QFrame(parent), mode_(SeparatorMode::FullBleed), orientation_(orientation) {
    setFrameShape(QFrame::NoFrame);
    setLineWidth(0);
    setMidLineWidth(0);
}

Separator::~Separator() {
    // Qt will handle cleanup of child objects
}

// ============================================================================
// Property Getters/Setters
// ============================================================================

SeparatorMode Separator::mode() const {
    return mode_;
}

void Separator::setMode(SeparatorMode mode) {
    if (mode_ != mode) {
        mode_ = mode;
        update();
    }
}

Qt::Orientation Separator::orientation() const {
    return orientation_;
}

void Separator::setOrientation(Qt::Orientation orientation) {
    if (orientation_ != orientation) {
        orientation_ = orientation;
        updateGeometry();
        update();
    }
}

// ============================================================================
// Size Hints
// ============================================================================

QSize Separator::sizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Material Design separator thickness is 1dp
    float thickness = helper.dpToPx(1.0f);

    if (orientation() == Qt::Horizontal) {
        // Horizontal separator: fixed height (1dp), flexible width
        return QSize(1, static_cast<int>(std::ceil(thickness)));
    } else {
        // Vertical separator: fixed width (1dp), flexible height
        return QSize(static_cast<int>(std::ceil(thickness)), 1);
    }
}

// ============================================================================
// Color Access
// ============================================================================

namespace {
// Fallback color when theme is not available
inline CFColor fallbackOutlineVariant() {
    return CFColor(120, 124, 132);
} // Outline gray

} // namespace

CFColor Separator::separatorColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackOutlineVariant();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        CFColor color = CFColor(colorScheme.queryColor(OUTLINE));
        QColor result = color.native_color();
        result.setAlphaF(0.90f);
        return CFColor(result);
    } catch (...) {
        QColor color = fallbackOutlineVariant().native_color();
        color.setAlphaF(0.90f);
        return CFColor(color);
    }
}

// ============================================================================
// Drawing Helpers
// ============================================================================

QRectF Separator::separatorRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    const float thickness = helper.dpToPx(1.0f);
    const float insetDp = helper.dpToPx(16.0f);

    QRectF widgetRect = QRectF(rect());

    if (orientation() == Qt::Horizontal) {
        // Horizontal separator
        float yPos = widgetRect.center().y() - thickness / 2.0f;
        float xPos = widgetRect.left();
        float width = widgetRect.width();

        switch (mode_) {
            case SeparatorMode::FullBleed:
                // Span entire width
                break;
            case SeparatorMode::Inset:
                // 16dp margin on both sides
                xPos += insetDp;
                width -= insetDp * 2.0f;
                break;
            case SeparatorMode::MiddleInset:
                // 16dp margin on leading (left) side
                xPos += insetDp;
                width -= insetDp;
                break;
        }

        return QRectF(xPos, yPos, std::max(0.0f, width), thickness);

    } else {
        // Vertical separator
        float xPos = widgetRect.center().x() - thickness / 2.0f;
        float yPos = widgetRect.top();
        float height = widgetRect.height();

        switch (mode_) {
            case SeparatorMode::FullBleed:
                // Span entire height
                break;
            case SeparatorMode::Inset:
                // 16dp margin on both sides
                yPos += insetDp;
                height -= insetDp * 2.0f;
                break;
            case SeparatorMode::MiddleInset:
                // 16dp margin on leading (top) side
                yPos += insetDp;
                height -= insetDp;
                break;
        }

        return QRectF(xPos, yPos, thickness, std::max(0.0f, height));
    }
}

// ============================================================================
// Paint Event
// ============================================================================

void Separator::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter p(this);
    // No antialiasing needed for a straight line
    p.setRenderHint(QPainter::Antialiasing, false);

    QRectF lineRect = separatorRect();
    if (lineRect.width() <= 0 || lineRect.height() <= 0) {
        return; // Nothing to draw
    }

    CFColor color = separatorColor();
    p.fillRect(lineRect, color.native_color());
}

} // namespace cf::ui::widget::material
