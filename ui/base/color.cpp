/**
 * @file color.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "color.h"
#include "math_helper.h"
#include <algorithm>
#include <cmath>

namespace cf::ui::base {

// ============================================================================
// Helper functions (internal)
// ============================================================================

namespace {
// sRGB to linear RGB conversion (gamma correction)
float toLinear(float c) {
    if (c <= 0.04045f) {
        return c / 12.92f;
    }
    return std::pow((c + 0.055f) / 1.055f, 2.4f);
}

// Linear RGB to sRGB conversion
float toGamma(float c) {
    if (c <= 0.0031308f) {
        return c * 12.92f;
    }
    return 1.055f * std::pow(c, 1.0f / 2.4f) - 0.055f;
}

// RGB to HSL conversion
struct HSL {
    float h; // 0-360
    float s; // 0-1
    float l; // 0-1
};

HSL rgbToHsl(float r, float g, float b) {
    float maxVal = std::max({r, g, b});
    float minVal = std::min({r, g, b});
    float delta = maxVal - minVal;

    HSL hsl;
    hsl.l = (maxVal + minVal) / 2.0f;

    if (delta < 0.0001f) {
        hsl.h = 0.0f;
        hsl.s = 0.0f;
    } else {
        if (hsl.l < 0.5f) {
            hsl.s = delta / (maxVal + minVal);
        } else {
            hsl.s = delta / (2.0f - maxVal - minVal);
        }

        if (maxVal == r) {
            hsl.h = 60.0f * std::fmod((g - b) / delta, 6.0f);
        } else if (maxVal == g) {
            hsl.h = 60.0f * ((b - r) / delta + 2.0f);
        } else {
            hsl.h = 60.0f * ((r - g) / delta + 4.0f);
        }
    }

    if (hsl.h < 0.0f)
        hsl.h += 360.0f;
    return hsl;
}

// HSL to RGB conversion
void hslToRgb(float h, float s, float l, float& outR, float& outG, float& outB) {
    if (s < 0.0001f) {
        outR = outG = outB = l;
        return;
    }

    float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
    float p = 2.0f * l - q;

    auto hueToRgb = [p, q](float t) -> float {
        if (t < 0.0f)
            t += 1.0f;
        if (t > 1.0f)
            t -= 1.0f;
        if (t < 1.0f / 6.0f)
            return p + (q - p) * 6.0f * t;
        if (t < 1.0f / 2.0f)
            return q;
        if (t < 2.0f / 3.0f)
            return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
        return p;
    };

    outR = hueToRgb((h / 360.0f) + 1.0f / 3.0f);
    outG = hueToRgb(h / 360.0f);
    outB = hueToRgb((h / 360.0f) - 1.0f / 3.0f);
}

// HCT to HSL approximation
// H and C are preserved, Tone maps to Lightness with adjustment
void hctToHsl(float h, float c, float tone, float& outH, float& outS, float& outL) {
    // Simplified: H is same, Tone affects L, C affects S
    outH = h;

    // Tone (0-100) to Lightness (0-1) - roughly linear but with adjustment
    float t = math::clamp(tone, 0.0f, 100.0f) / 100.0f;

    // Chroma affects saturation
    // Higher chroma = higher saturation for mid tones
    // At very low/high tones, saturation is naturally limited
    float chromaFactor = math::clamp(c / 100.0f, 0.0f, 1.5f);

    // Saturation peaks at mid tones
    float toneSaturationFactor = 1.0f - std::abs(t - 0.5f) * 2.0f; // 1 at mid, 0 at extremes
    toneSaturationFactor =
        toneSaturationFactor * toneSaturationFactor; // Square for smoother falloff

    outS = chromaFactor * (0.3f + 0.7f * toneSaturationFactor);
    outS = math::clamp(outS, 0.0f, 1.0f);

    // Lightness is primarily tone, but slightly affected by chroma
    // Higher chroma colors appear slightly darker at same tone
    float chromaDarkening = chromaFactor * 0.05f;
    outL = math::clamp(t - chromaDarkening, 0.0f, 1.0f);
}

// RGB to HCT approximation
void rgbToHct(float r, float g, float b, float& outH, float& outC, float& outT) {
    HSL hsl = rgbToHsl(r, g, b);

    // H is same as HSL hue
    outH = hsl.h;

    // Tone is primarily lightness, scaled to 0-100
    // Adjust for perceived brightness (green appears brighter than blue)
    float perceivedLightness = 0.299f * r + 0.587f * g + 0.114f * b;
    outT = perceivedLightness * 100.0f;

    // Chroma is derived from saturation, but adjusted
    // At extreme tones, same saturation gives lower perceived chroma
    float toneFactor = 1.0f - std::abs(hsl.l - 0.5f) * 1.5f;
    toneFactor = math::clamp(toneFactor, 0.2f, 1.0f);
    outC = (hsl.s / toneFactor) * 100.0f;
    outC = math::clamp(outC, 0.0f, 150.0f);
}

} // anonymous namespace

// ============================================================================
// CFColor Implementation
// ============================================================================

CFColor::CFColor(int r, int g, int b) : internal_color(r, g, b) {
    // Cache HCT values
    float rf = internal_color.redF();
    float gf = internal_color.greenF();
    float bf = internal_color.blueF();
    rgbToHct(rf, gf, bf, m_hue, m_chroma, m_tone);
}

CFColor::CFColor(const QColor& native) : internal_color(native) {
    // Cache HCT values
    float r = internal_color.redF();
    float g = internal_color.greenF();
    float b = internal_color.blueF();
    rgbToHct(r, g, b, m_hue, m_chroma, m_tone);
}

namespace {

// Helper: parse hex byte string to integer
int parseHexByte(const QString& s, int pos) {
    return s.mid(pos, 2).toInt(nullptr, 16);
}

} // anonymous namespace

CFColor::CFColor(const char* hex) {
    if (!hex) {
        // Null pointer, use black
        internal_color = Qt::black;
        m_hue = 0.0f;
        m_chroma = 0.0f;
        m_tone = 0.0f;
        return;
    }
    // Convert to QString and use that logic
    QString hexStr(hex);
    QString trimmed = hexStr.trimmed();
    if (!trimmed.startsWith('#')) {
        // Invalid format, use black
        internal_color = Qt::black;
        m_hue = 0.0f;
        m_chroma = 0.0f;
        m_tone = 0.0f;
        return;
    }

    trimmed.remove(0, 1); // Remove '#'

    bool ok;
    if (trimmed.length() == 6) {
        // #RRGGBB format
        internal_color =
            QColor(parseHexByte(trimmed, 0), parseHexByte(trimmed, 2), parseHexByte(trimmed, 4));
    } else if (trimmed.length() == 8) {
        // #AARRGGBB format
        internal_color = QColor(parseHexByte(trimmed, 2), parseHexByte(trimmed, 4),
                                parseHexByte(trimmed, 6), parseHexByte(trimmed, 0));
    } else {
        // Invalid format, use black
        internal_color = Qt::black;
    }

    // Cache HCT values
    float r = internal_color.redF();
    float g = internal_color.greenF();
    float b = internal_color.blueF();
    rgbToHct(r, g, b, m_hue, m_chroma, m_tone);
}

CFColor::CFColor(const QString& hex) {
    QString trimmed = hex.trimmed();
    if (!trimmed.startsWith('#')) {
        // Invalid format, use black
        internal_color = Qt::black;
        m_hue = 0.0f;
        m_chroma = 0.0f;
        m_tone = 0.0f;
        return;
    }

    trimmed.remove(0, 1); // Remove '#'

    bool ok;
    if (trimmed.length() == 6) {
        // #RRGGBB format
        internal_color =
            QColor(parseHexByte(trimmed, 0), parseHexByte(trimmed, 2), parseHexByte(trimmed, 4));
    } else if (trimmed.length() == 8) {
        // #AARRGGBB format
        internal_color = QColor(parseHexByte(trimmed, 2), parseHexByte(trimmed, 4),
                                parseHexByte(trimmed, 6), parseHexByte(trimmed, 0));
    } else {
        // Invalid format, use black
        internal_color = Qt::black;
    }

    // Cache HCT values
    float r = internal_color.redF();
    float g = internal_color.greenF();
    float b = internal_color.blueF();
    rgbToHct(r, g, b, m_hue, m_chroma, m_tone);
}

CFColor::CFColor(float hue, float chroma, float tone) {
    m_hue = math::clamp(hue, 0.0f, 360.0f);
    m_chroma = math::clamp(chroma, 0.0f, 150.0f);
    m_tone = math::clamp(tone, 0.0f, 100.0f);

    // Convert HCT to RGB via HSL
    float h, s, l;
    hctToHsl(m_hue, m_chroma, m_tone, h, s, l);

    float r, g, b;
    hslToRgb(h, s, l, r, g, b);

    internal_color = QColor::fromRgbF(math::clamp(r, 0.0f, 1.0f), math::clamp(g, 0.0f, 1.0f),
                                      math::clamp(b, 0.0f, 1.0f));
}

float CFColor::relativeLuminance() const {
    // WCAG 2.1 relative luminance formula
    float r = toLinear(internal_color.redF());
    float g = toLinear(internal_color.greenF());
    float b = toLinear(internal_color.blueF());

    return 0.2126f * r + 0.7152f * g + 0.0722f * b;
}

float CFColor::hue() const {
    return m_hue;
}

float CFColor::chroma() const {
    return m_chroma;
}

float CFColor::tone() const {
    return m_tone;
}

} // namespace cf::ui::base
