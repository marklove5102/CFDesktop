/**
 * @file    timing_animation.h
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Timing-based Animation Interface
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Defines the timing-based animation interface that uses motion spec
 * for duration and easing values. This is the base class for animations
 * that follow Material Design 3 motion specifications.
 *
 * @ingroup ui_components
 */
#pragma once

#include "animation.h"
#include "core/motion_spec.h"
#include <qobject.h>

namespace cf::ui::components {

// Forward declaration
namespace core {
struct IMotionSpec;
}

/**
 * @brief  Timing-based animation interface.
 *
 * @details Base class for animations that use MotionSpec for timing
 *          and easing configuration following Material Design 3.
 *
 *          NOTE: The motion_spec pointer must remain valid for the
 *          lifetime of this animation. This is guaranteed when the
 *          animation is created by CFMaterialAnimationFactory, which
 *          holds a reference to the theme that owns the motion spec.
 *
 * @since  0.1
 * @ingroup ui_components
 */
class CF_UI_EXPORT ICFTimingAnimation : public ICFAbstractAnimation {
    Q_OBJECT
  public:
    explicit ICFTimingAnimation(cf::ui::core::IMotionSpec* spec, QObject* parent = nullptr);

    // Assigned Only
    virtual void setRange(float from, float to) {
        m_from = from;
        m_to = to;
    }

    virtual float currentValue() const = 0;

  protected:
    cf::ui::core::IMotionSpec* motion_spec_ = nullptr;
    float m_from = 0.0f;
    float m_to = 1.0f;
    int m_elapsed = 0;
};

} // namespace cf::ui::components
