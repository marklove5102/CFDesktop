/**
 * @file color_helper.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Color helper functions for Material Design 3
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "color_helper.h"
#include "math_helper.h"

namespace cf::ui::base {

// ============================================================================
// Helper functions (internal)
// ============================================================================

namespace {
// Elevation overlay alpha values for elevation levels 0-5
// Material Design 3 specification
constexpr float ELEVATION_ALPHA[] = {
    0.00f, // Level 0
    0.05f, // Level 1
    0.08f, // Level 2
    0.11f, // Level 3
    0.14f, // Level 4
    0.17f  // Level 5
};

// Tonal palette tone values (13 tones from key color)
constexpr float TONAL_VALUES[] = {
    0.0f,  // Near black
    10.0f, // Very dark
    20.0f, // Dark
    30.0f, // Dark medium
    40.0f, // Medium dark
    50.0f, // Mid
    60.0f, // Mid light
    70.0f, // Medium light
    80.0f, // Light
    90.0f, // Very light
    95.0f, // Pale
    99.0f, // Near white
    100.0f // Pure white
};
constexpr int TONAL_COUNT = sizeof(TONAL_VALUES) / sizeof(TONAL_VALUES[0]);

} // anonymous namespace

// ============================================================================
// Color Helper Functions
// ============================================================================

CFColor blend(const CFColor& base, CFColor& overlay, float ratio) {
    // Clamp ratio to [0, 1]
    float t = math::clamp(ratio, 0.0f, 1.0f);

    // Get RGB components
    float baseR = base.native_color().redF();
    float baseG = base.native_color().greenF();
    float baseB = base.native_color().blueF();
    float baseA = base.native_color().alphaF();

    float overlayR = overlay.native_color().redF();
    float overlayG = overlay.native_color().greenF();
    float overlayB = overlay.native_color().blueF();
    float overlayA = overlay.native_color().alphaF();

    // Linear interpolation for each component
    float r = math::lerp(baseR, overlayR, t);
    float g = math::lerp(baseG, overlayG, t);
    float b = math::lerp(baseB, overlayB, t);
    float a = math::lerp(baseA, overlayA, t);

    // Convert to integer RGB for consistent comparison
    int rInt = static_cast<int>(r * 255.0f + 0.5f);
    int gInt = static_cast<int>(g * 255.0f + 0.5f);
    int bInt = static_cast<int>(b * 255.0f + 0.5f);
    int aInt = static_cast<int>(a * 255.0f + 0.5f);

    return CFColor(QColor(rInt, gInt, bInt, aInt));
}

CFColor elevationOverlay(CFColor& surface, CFColor& primary, int elevation) {
    // Clamp elevation to [0, 5]
    int level = math::clamp(elevation, 0, 5);
    float alpha = ELEVATION_ALPHA[level];

    // Blend primary color over surface with elevation alpha
    return blend(surface, primary, alpha);
}

float contrastRatio(CFColor& a, CFColor& b) {
    // WCAG 2.1 contrast ratio formula
    // ratio = (L1 + 0.05) / (L2 + 0.05)
    // where L1 is the lighter (higher luminance) and L2 is the darker

    float lumA = a.relativeLuminance();
    float lumB = b.relativeLuminance();

    float lighter = std::max(lumA, lumB);
    float darker = std::min(lumA, lumB);

    return (lighter + 0.05f) / (darker + 0.05f);
}

QList<CFColor> tonalPalette(CFColor keyColor) {
    QList<CFColor> palette;

    // Get HCT values from key color
    float hue = keyColor.hue();
    float chroma = keyColor.chroma();

    // Generate tonal palette by varying tone while keeping hue and chroma constant
    for (int i = 0; i < TONAL_COUNT; ++i) {
        palette.append(CFColor(hue, chroma, TONAL_VALUES[i]));
    }

    return palette;
}

} // namespace cf::ui::base
