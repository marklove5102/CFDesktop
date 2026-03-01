/**
 * @file    cfmaterial_property_animation.cpp
 * @brief   Material Design 3 Property Animation Implementation
 * @version 0.1
 * @date    2026-03-01
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements the CFMaterialPropertyAnimation class for float property
 * animations following Material Design 3 motion specifications.
 */

#include "cfmaterial_property_animation.h"
#include "base/easing.h"
#include <QWidget>

namespace cf::ui::components::material {

// =============================================================================
// Constructor / Destructor
// =============================================================================

CFMaterialPropertyAnimation::CFMaterialPropertyAnimation(
    float* value, float from, float to, int durationMs, base::Easing::Type easing, QObject* parent)
    : ICFAbstractAnimation(parent)
    , m_value(value)
    , m_from(from)
    , m_to(to)
    , m_durationMs(durationMs)
    , m_easing(easing)
    , m_elapsed(0)
    , m_targetWidget(nullptr)
    , m_timer(new QTimer(this)) {

    // Setup timer with dynamic interval
    m_timer->setInterval(calculateInterval());
    connect(m_timer, &QTimer::timeout, this, &CFMaterialPropertyAnimation::onTimerTick);

    // Set initial value
    if (m_value) {
        *m_value = from;
    }
}

CFMaterialPropertyAnimation::~CFMaterialPropertyAnimation() {
    if (m_timer) {
        m_timer->stop();
    }
}

// =============================================================================
// ICFAbstractAnimation Interface
// =============================================================================

void CFMaterialPropertyAnimation::start(Direction dir) {
    if (m_state == State::Running) {
        return; // Already running
    }

    m_elapsed = 0;
    m_state = State::Running;

    // Set initial value based on direction
    if (m_value) {
        if (dir == Direction::Forward) {
            *m_value = m_from;
        } else {
            *m_value = m_to;
        }
    }

    emit started();

    // Update target widget
    if (m_targetWidget) {
        m_targetWidget->update();
    }

    // Start the internal timer (60 FPS ~ 16ms)
    if (m_timer) {
        m_timer->start();
    }
}

void CFMaterialPropertyAnimation::pause() {
    if (m_state == State::Running) {
        m_state = State::Paused;
        if (m_timer) {
            m_timer->stop();
        }
        emit paused();
    }
}

void CFMaterialPropertyAnimation::stop() {
    m_state = State::Idle;
    if (m_timer) {
        m_timer->stop();
    }
    m_elapsed = 0;

    // Reset to initial value
    if (m_value) {
        *m_value = m_from;
    }

    if (m_targetWidget) {
        m_targetWidget->update();
    }

    emit stopped();
}

void CFMaterialPropertyAnimation::reverse() {
    if (m_state == State::Running) {
        // Calculate remaining progress and reverse from there
        float progress = static_cast<float>(m_elapsed) / m_durationMs;
        progress = 1.0f - progress; // Reverse progress

        m_elapsed = static_cast<int>(progress * m_durationMs);

        // Swap from/to for reverse
        std::swap(m_from, m_to);
    } else {
        // Start in reverse direction
        start(Direction::Backward);
    }

    emit reversed();
}

bool CFMaterialPropertyAnimation::tick(int dt) {
    if (m_state != State::Running) {
        return false;
    }

    m_elapsed += dt;

    // Calculate progress
    float progress = static_cast<float>(m_elapsed) / m_durationMs;

    // Clamp progress
    if (progress > 1.0f) {
        progress = 1.0f;
    }

    // Apply easing
    QEasingCurve easingCurve = base::Easing::fromEasingType(m_easing);
    float easedProgress = static_cast<float>(easingCurve.valueForProgress(progress));

    // Calculate current value based on range
    if (m_value) {
        *m_value = m_from + (m_to - m_from) * easedProgress;
    }

    // Emit progress signal
    emit progressChanged(easedProgress);

    // Update target widget
    if (m_targetWidget) {
        m_targetWidget->update();
    }

    // Check if animation is complete
    if (progress >= 1.0f) {
        m_state = State::Finished;
        if (m_timer) {
            m_timer->stop();
        }
        emit finished();
        return false;
    }

    return true;
}

// =============================================================================
// Property-Specific Methods
// =============================================================================

void CFMaterialPropertyAnimation::setTargetWidget(QWidget* widget) {
    m_targetWidget = widget;
}

// =============================================================================
// Private Slots
// =============================================================================

void CFMaterialPropertyAnimation::onTimerTick() {
    tick(calculateInterval());
}

} // namespace cf::ui::components::material
