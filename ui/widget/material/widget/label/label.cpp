/**
 * @file label.cpp
 * @brief Material Design 3 Label Implementation
 *
 * Implements a Material Design 3 label with 15 typography styles,
 * 9 color variants, and full theme integration.
 *
 * @author CFDesktop Team
 * @date 2026-03-01
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "label.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "core/token/typography/cfmaterial_typography_token_literals.h"

#include <QApplication>
#include <QFontMetrics>
#include <QPainter>

namespace cf::ui::widget::material {

using namespace cf::ui::base::device;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::application_support;

namespace {
// Fallback colors when theme is not available
inline CFColor fallbackOnSurface() {
    return CFColor(27, 27, 31);
} // On Surface
inline CFColor fallbackOnSurfaceVariant() {
    return CFColor(74, 69, 78);
} // On Surface Variant
inline CFColor fallbackPrimary() {
    return CFColor(103, 80, 164);
} // Purple 700
inline CFColor fallbackOnPrimary() {
    return CFColor(255, 255, 255);
} // White
inline CFColor fallbackSecondary() {
    return CFColor(101, 163, 207);
} // Light Blue
inline CFColor fallbackOnSecondary() {
    return CFColor(255, 255, 255);
} // White
inline CFColor fallbackError() {
    return CFColor(186, 26, 26);
} // Error
inline CFColor fallbackOnError() {
    return CFColor(255, 255, 255);
} // White
inline CFColor fallbackInverseSurface() {
    return CFColor(49, 48, 51);
} // Inverse Surface
inline CFColor fallbackInverseOnSurface() {
    return CFColor(236, 236, 240);
} // Inverse On Surface
} // namespace

// ============================================================================
// Constructor / Destructor
// ============================================================================

Label::Label(const QString& text, TypographyStyle style, QWidget* parent)
    : QLabel(text, parent), typographyStyle_(style), colorVariant_(LabelColorVariant::OnSurface),
      autoHiding_(false), colorCacheValid_(false) {
    updateAppearance();
}

Label::~Label() = default;

// ============================================================================
// Property Getters/Setters
// ============================================================================

TypographyStyle Label::typographyStyle() const {
    return typographyStyle_;
}

void Label::setTypographyStyle(TypographyStyle style) {
    if (typographyStyle_ != style) {
        typographyStyle_ = style;
        colorCacheValid_ = false;  // Invalidate color cache
        updateAppearance();
        updateGeometry();
    }
}

LabelColorVariant Label::colorVariant() const {
    return colorVariant_;
}

void Label::setColorVariant(LabelColorVariant variant) {
    if (colorVariant_ != variant) {
        colorVariant_ = variant;
        colorCacheValid_ = false;  // Invalidate color cache
        updateAppearance();
    }
}

bool Label::autoHiding() const {
    return autoHiding_;
}

void Label::setAutoHiding(bool enabled) {
    if (autoHiding_ != enabled) {
        autoHiding_ = enabled;
        updateAppearance();
    }
}

// ============================================================================
// Size Hints
// ============================================================================

QSize Label::sizeHint() const {
    QFontMetrics fm(font());
    QString txt = text();

    // Handle empty text
    if (txt.isEmpty()) {
        return QLabel::sizeHint();
    }

    // Calculate based on word wrap setting
    if (wordWrap()) {
        int width = fm.horizontalAdvance(txt);
        // Approximate height with line wrapping
        int height = fm.height() * 2 + fm.leading();
        // Use a reasonable default width constraint
        int maxWidth = 300;
        if (width > maxWidth) {
            int lines = (width + maxWidth - 1) / maxWidth;
            height = fm.height() * lines + fm.leading() * (lines - 1);
            width = maxWidth;
        }
        return QSize(width, height);
    }

    return QSize(fm.horizontalAdvance(txt), fm.height());
}

QSize Label::minimumSizeHint() const {
    QFontMetrics fm(font());
    // Minimum: at least one character or empty placeholder
    int minWidth = fm.horizontalAdvance("M");
    return QSize(minWidth, fm.height());
}

// ============================================================================
// Event Handlers
// ============================================================================

void Label::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QString txt = text();
    if (autoHiding_ && txt.isEmpty()) {
        return; // Don't render anything if auto-hiding and empty
    }

    // Draw text manually with theme-aware color
    QPainter p(this);
    p.setPen(textColor().native_color());
    p.setFont(font());

    QRect contentRect = contentsRect();
    Qt::Alignment align = alignment();

    // Draw text with proper alignment and word wrap
    int flags = align;
    if (wordWrap()) {
        flags |= Qt::TextWordWrap;
    }

    p.drawText(contentRect, flags, txt);
}

void Label::changeEvent(QEvent* event) {
    QLabel::changeEvent(event);

    if (event->type() == QEvent::EnabledChange) {
        // Update appearance when enabled state changes
        colorCacheValid_ = false;  // Invalidate color cache
        updateAppearance();
    }
}

// ============================================================================
// Private Helpers
// ============================================================================

void Label::updateAppearance() {
    // Update font from theme
    setFont(typographyFont());

    // Update stylesheet is not used - we handle color in paintEvent
    // Update auto-hiding state
    if (autoHiding_) {
        setVisible(!text().isEmpty());
    } else {
        setVisible(true);
    }

    update();
}

CFColor Label::textColor() const {
    // Return cached color if valid
    if (colorCacheValid_) {
        return cachedColor_;
    }

    auto* app = Application::instance();
    if (!app) {
        cachedColor_ = fallbackOnSurface();
        colorCacheValid_ = true;
        return cachedColor_;
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();

        const char* token = nullptr;
        switch (colorVariant_) {
            case LabelColorVariant::OnSurface:
                token = ON_SURFACE;
                break;
            case LabelColorVariant::OnSurfaceVariant:
                token = ON_SURFACE_VARIANT;
                break;
            case LabelColorVariant::Primary:
                token = PRIMARY;
                break;
            case LabelColorVariant::OnPrimary:
                token = ON_PRIMARY;
                break;
            case LabelColorVariant::Secondary:
                token = SECONDARY;
                break;
            case LabelColorVariant::OnSecondary:
                token = ON_SECONDARY;
                break;
            case LabelColorVariant::Error:
                token = ERROR;
                break;
            case LabelColorVariant::OnError:
                token = ON_ERROR;
                break;
            case LabelColorVariant::InverseSurface:
                token = INVERSE_SURFACE;
                break;
            case LabelColorVariant::InverseOnSurface:
                token = INVERSE_ON_SURFACE;
                break;
        }

        if (token) {
            CFColor color = CFColor(colorScheme.queryColor(token));

            // Apply disabled state opacity
            if (!isEnabled()) {
                QColor c = color.native_color();
                c.setAlphaF(0.38f); // 38% opacity for disabled
                cachedColor_ = CFColor(c);
            } else {
                cachedColor_ = color;
            }
            colorCacheValid_ = true;
            return cachedColor_;
        }
    } catch (...) {
        // Fallback if theme access fails
    }

    // Fallback colors
    CFColor fallback;
    switch (colorVariant_) {
        case LabelColorVariant::OnSurface:
            fallback = fallbackOnSurface();
            break;
        case LabelColorVariant::OnSurfaceVariant:
            fallback = fallbackOnSurfaceVariant();
            break;
        case LabelColorVariant::Primary:
            fallback = fallbackPrimary();
            break;
        case LabelColorVariant::OnPrimary:
            fallback = fallbackOnPrimary();
            break;
        case LabelColorVariant::Secondary:
            fallback = fallbackSecondary();
            break;
        case LabelColorVariant::OnSecondary:
            fallback = fallbackOnSecondary();
            break;
        case LabelColorVariant::Error:
            fallback = fallbackError();
            break;
        case LabelColorVariant::OnError:
            fallback = fallbackOnError();
            break;
        case LabelColorVariant::InverseSurface:
            fallback = fallbackInverseSurface();
            break;
        case LabelColorVariant::InverseOnSurface:
            fallback = fallbackInverseOnSurface();
            break;
    }

    // Apply disabled state to fallback
    if (!isEnabled()) {
        QColor c = fallback.native_color();
        c.setAlphaF(0.38f);
        cachedColor_ = CFColor(c);
    } else {
        cachedColor_ = fallback;
    }
    colorCacheValid_ = true;
    return cachedColor_;
}

QFont Label::typographyFont() const {
    auto* app = Application::instance();
    if (!app) {
        // Fallback to system font with reasonable size
        QFont font = QLabel::font();
        font.setPixelSize(14);
        font.setWeight(QFont::Normal);
        return font;
    }

    try {
        const auto& theme = app->currentTheme();
        auto& fontType = theme.font_type();
        const char* tokenName = typographyTokenName(typographyStyle_);
        return fontType.queryTargetFont(tokenName);
    } catch (...) {
        QFont font = QLabel::font();
        font.setPixelSize(14);
        font.setWeight(QFont::Normal);
        return font;
    }
}

const char* Label::typographyTokenName(TypographyStyle style) {
    switch (style) {
        case TypographyStyle::DisplayLarge:
            return TYPOGRAPHY_DISPLAY_LARGE;
        case TypographyStyle::DisplayMedium:
            return TYPOGRAPHY_DISPLAY_MEDIUM;
        case TypographyStyle::DisplaySmall:
            return TYPOGRAPHY_DISPLAY_SMALL;

        case TypographyStyle::HeadlineLarge:
            return TYPOGRAPHY_HEADLINE_LARGE;
        case TypographyStyle::HeadlineMedium:
            return TYPOGRAPHY_HEADLINE_MEDIUM;
        case TypographyStyle::HeadlineSmall:
            return TYPOGRAPHY_HEADLINE_SMALL;

        case TypographyStyle::TitleLarge:
            return TYPOGRAPHY_TITLE_LARGE;
        case TypographyStyle::TitleMedium:
            return TYPOGRAPHY_TITLE_MEDIUM;
        case TypographyStyle::TitleSmall:
            return TYPOGRAPHY_TITLE_SMALL;

        case TypographyStyle::BodyLarge:
            return TYPOGRAPHY_BODY_LARGE;
        case TypographyStyle::BodyMedium:
            return TYPOGRAPHY_BODY_MEDIUM;
        case TypographyStyle::BodySmall:
            return TYPOGRAPHY_BODY_SMALL;

        case TypographyStyle::LabelLarge:
            return TYPOGRAPHY_LABEL_LARGE;
        case TypographyStyle::LabelMedium:
            return TYPOGRAPHY_LABEL_MEDIUM;
        case TypographyStyle::LabelSmall:
            return TYPOGRAPHY_LABEL_SMALL;
    }
    return TYPOGRAPHY_BODY_MEDIUM;
}

} // namespace cf::ui::widget::material
