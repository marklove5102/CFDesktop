/**
 * @file state_machine.cpp
 * @brief Material Design State Machine Implementation
 *
 * Manages visual state transitions for Material widgets, driving
 * StateLayer opacity animations following Material Design 3 specifications.
 *
 * State opacity values (Material Design 3):
 * - Normal:   0.00
 * - Hovered:  0.08
 * - Pressed:  0.12
 * - Focused:  0.12
 * - Dragged:  0.16
 * - Disabled: 0.00
 *
 * @author Material Design Framework Team
 * @date 2026-02-28
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "state_machine.h"
#include "components/material/cfmaterial_animation_factory.h"
#include "components/timing_animation.h"

namespace cf::ui::widget::material::base {

using namespace cf::ui::components::material;
using namespace cf::ui::components;

/**
 * @brief Constructor - initializes state machine with animation factory.
 *
 * @param factory WeakPtr to animation factory for creating transitions.
 * @param parent QObject parent for memory management.
 */
StateMachine::StateMachine(cf::WeakPtr<components::material::CFMaterialAnimationFactory> factory,
                           QObject* parent)
    : QObject(parent), m_state(State::StateNormal), m_opacity(0.0f) {
    // WeakPtr is stored but not locked here to avoid circular dependency
    // Store the WeakPtr in a member variable for later use
    m_animator = factory;
}

/**
 * @brief Calculates target opacity for a given state.
 *
 * Follows Material Design 3 state layer opacity specifications.
 *
 * @param s The state to calculate opacity for.
 * @return Target opacity value [0.0, 1.0].
 */
float StateMachine::targetOpacityForState(States s) const {
    // Priority order: Disabled > Pressed > Dragged > Focused > Hovered > Normal
    // When multiple states are active, use the highest priority

    if (s & State::StateDisabled) {
        return 0.0f;
    }
    if (s & State::StatePressed) {
        return 0.12f;
    }
    if (s & State::StateDragged) {
        return 0.16f;
    }
    if (s & State::StateFocused) {
        return 0.12f;
    }
    if (s & State::StateHovered) {
        return 0.08f;
    }
    if (s & State::StateChecked) {
        return 0.08f; // Checked state uses same as hovered
    }

    return 0.0f; // StateNormal
}

/**
 * @brief Starts opacity transition animation.
 *
 * Creates a fade animation using the animation factory.
 *
 * @param from Starting opacity value.
 * @param to Target opacity value.
 */
void StateMachine::animateOpacityTo(float from, float to) {
    // Performance mode check: if animations are disabled, set directly
    auto* factory = m_animator.Get();
    if (!factory || !factory->isAllEnabled()) {
        m_opacity = to;
        emit stateLayerOpacityChanged(m_opacity);
        return;
    }

    // Get fade animation from factory
    auto anim = factory->getAnimation("md.animation.fadeIn");
    if (!anim) {
        // Fallback: direct set if animation creation fails
        m_opacity = to;
        emit stateLayerOpacityChanged(m_opacity);
        return;
    }

    // Get raw pointer and try to set range if it's a timing animation
    auto* rawAnim = anim.Get();
    auto* timingAnim = static_cast<components::ICFTimingAnimation*>(rawAnim);
    if (timingAnim) {
        timingAnim->setRange(from, to);
    }

    // Connect progress signal
    connect(rawAnim, &components::ICFAbstractAnimation::progressChanged, this,
            [this](float progress) {
                m_opacity = progress;
                emit stateLayerOpacityChanged(m_opacity);
            });

    // Start animation
    rawAnim->start(components::ICFAbstractAnimation::Direction::Forward);
}

// ============================================================================
// Event Handlers
// ============================================================================

void StateMachine::onHoverEnter() {
    if (m_state & State::StateDisabled)
        return;

    States oldState = m_state;
    m_state |= State::StateHovered;

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(m_opacity, targetOpacityForState(m_state));
    }
}

void StateMachine::onHoverLeave() {
    States oldState = m_state;
    m_state &= ~static_cast<States>(State::StateHovered);

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(m_opacity, targetOpacityForState(m_state));
    }
}

void StateMachine::onPress(const QPoint& pos) {
    Q_UNUSED(pos)
    if (m_state & State::StateDisabled)
        return;

    States oldState = m_state;
    m_state |= State::StatePressed;

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(m_opacity, targetOpacityForState(m_state));
    }
}

void StateMachine::onRelease() {
    States oldState = m_state;
    m_state &= ~static_cast<States>(State::StatePressed);

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(m_opacity, targetOpacityForState(m_state));
    }
}

void StateMachine::onFocusIn() {
    if (m_state & State::StateDisabled)
        return;

    States oldState = m_state;
    m_state |= State::StateFocused;

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(m_opacity, targetOpacityForState(m_state));
    }
}

void StateMachine::onFocusOut() {
    States oldState = m_state;
    m_state &= ~static_cast<States>(State::StateFocused);

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(m_opacity, targetOpacityForState(m_state));
    }
}

void StateMachine::onEnable() {
    States oldState = m_state;
    m_state &= ~static_cast<States>(State::StateDisabled);

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(m_opacity, targetOpacityForState(m_state));
    }
}

void StateMachine::onDisable() {
    States oldState = m_state;
    m_state |= State::StateDisabled;

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(m_opacity, targetOpacityForState(m_state));
    }
}

void StateMachine::onCheckedChanged(bool checked) {
    if (m_state & State::StateDisabled)
        return;

    States oldState = m_state;

    if (checked) {
        m_state |= State::StateChecked;
    } else {
        m_state &= ~static_cast<States>(State::StateChecked);
    }

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(m_opacity, targetOpacityForState(m_state));
    }
}

// ============================================================================
// State Queries
// ============================================================================

StateMachine::States StateMachine::currentState() const {
    return m_state;
}

bool StateMachine::hasState(State s) const {
    return (m_state & s) != States();
}

float StateMachine::stateLayerOpacity() const {
    return m_opacity;
}

} // namespace cf::ui::widget::material::base
