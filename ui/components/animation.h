/**
 * @file    animation.h
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Abstract Animation Interface
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Defines the base animation interface for all animation types.
 * Provides common animation states, direction control, and lifecycle methods.
 *
 * @ingroup ui_components
 */
#pragma once

#include "base/weak_ptr/weak_ptr.h"
#include "export.h"
#include <QObject>
#include <QTimer>

namespace cf::ui::components {

/**
 * @brief  Abstract animation base class.
 *
 * @details Provides the common interface for all animations including
 *          state management, direction control, and lifecycle methods.
 *
 * @since  0.1
 * @ingroup ui_components
 */
class CF_UI_EXPORT ICFAbstractAnimation : public QObject {
    Q_OBJECT
  public:
    friend class ICFAnimationManagerFactory;

    explicit ICFAbstractAnimation(QObject* parent = nullptr);

    enum class State { Idle, Running, Paused, Finished };
    Q_ENUM(State)

    enum class Direction { Forward, Backward };
    Q_ENUM(Direction)

    // start the animation
    virtual void start(Direction dir = Direction::Forward) = 0;

    // pause the animation
    virtual void pause() = 0;

    // stop the aniamtion
    virtual void stop() = 0;

    // reverse should stop the current session
    virtual void reverse() = 0;

    // Called every frame by MdAnimator, subclasses implement specific interpolation
    // dt: Time interval since the last call (milliseconds)
    virtual bool tick(int dt) = 0;

    // Get a weak pointer to this animation
    // Each concrete animation class must implement this using its WeakPtrFactory
    virtual cf::WeakPtr<ICFAbstractAnimation> GetWeakPtr() = 0;

    bool getEnabled() const { return enabled; }

  signals:
    void started();
    void paused();
    void stopped();
    void reversed();
    void finished();
    void progressChanged(float progress);

  protected:
    QTimer* driven_internal_timer{nullptr};
    float m_progress = 0.0f;
    State m_state = State::Idle;

    void setEnabled(bool enabled) { this->enabled = enabled; }

  private:
    bool enabled;
};

} // namespace cf::ui::components
