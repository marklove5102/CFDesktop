/**
 * @file    animation_group.h
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Animation Group Interface
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Defines the animation group interface for running multiple animations
 * together either in parallel or sequentially.
 *
 * @ingroup ui_components
 */
#pragma once

#include "animation.h"
#include "base/weak_ptr/weak_ptr.h"
#include "export.h"
#include <QObject>
#include <unordered_set>

namespace cf::ui::components {

/**
 * @brief  Animation group for combining multiple animations.
 *
 * @details Groups can run animations in parallel or sequential mode.
 *
 * @since  0.1
 * @ingroup ui_components
 */
class CF_UI_EXPORT ICFAnimationGroup : public ICFAbstractAnimation {
    Q_OBJECT
  public:
    enum class Mode { Parallel, Sequential };
    Q_ENUM(Mode);

    ICFAnimationGroup(QObject* parent = nullptr) : ICFAbstractAnimation(parent) {}

    void addAnimation(cf::WeakPtr<ICFAbstractAnimation> animation) {
        animations.insert(animation);
    }

    void removeAnimation(cf::WeakPtr<ICFAbstractAnimation> animation) {
        animations.erase(animation);
    }

  private:
    std::unordered_set<cf::WeakPtr<ICFAbstractAnimation>> animations;
};

} // namespace cf::ui::components
