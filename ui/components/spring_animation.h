/**
 * @file    spring_animation.h
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Spring-based Animation Interface
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Defines the spring-based animation interface that uses spring physics
 * for natural motion. This provides animations that follow spring dynamics
 * rather than fixed timing curves.
 *
 * @ingroup ui_components
 */
#pragma once

#include "animation.h"
#include "base/easing.h"
#include <QObject>

namespace cf::ui::components {

/**
 * @brief  Spring-based animation interface.
 *
 * @details Base class for animations that use spring physics for
 *          natural, bouncy motion following Material Design 3.
 *
 * @since  0.1
 * @ingroup ui_components
 */
class CF_UI_EXPORT ICFSpringAnimation : public ICFAbstractAnimation {
    Q_OBJECT
  public:
    ICFSpringAnimation(const base::Easing::SpringPreset& easing, QObject* parent = nullptr)
        : ICFAbstractAnimation(parent) {
        easing_ = easing;
    }

    virtual void setTarget(float target) { m_target = target; }
    virtual void setInitialVelocity(float velocity) { m_velocity = velocity; }
    virtual float currentValue() const = 0;

    bool tick(int dt) override; // Using springStep

  protected:
    base::Easing::SpringPreset easing_;
    float m_position = 0.0f;
    float m_velocity = 0.0f;
    float m_target = 1.0f;
};

} // namespace cf::ui::components
