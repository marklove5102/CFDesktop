#pragma once
#include "base/include/base/weak_ptr/weak_ptr.h"
#include "components/material/cfmaterial_animation_factory.h"
#include "export.h"
#include <QObject>

namespace cf::ui::widget::material::base {
class CF_UI_EXPORT StateMachine : public QObject {
    Q_OBJECT
  public:
    enum class State {
        StateNormal = 0x00,
        StateHovered = 0x01,
        StatePressed = 0x02,
        StateFocused = 0x04,
        StateDisabled = 0x08,
        StateChecked = 0x10,
        StateDragged = 0x20
    };
    Q_DECLARE_FLAGS(States, State)

    explicit StateMachine(cf::WeakPtr<components::material::CFMaterialAnimationFactory> factory,
                          QObject* parent);
    void onHoverEnter();
    void onHoverLeave();
    void onPress(const QPoint& pos);
    void onRelease();
    void onFocusIn();
    void onFocusOut();
    void onEnable();
    void onDisable();
    void onCheckedChanged(bool checked);
    States currentState() const;
    bool hasState(State s) const;
    float stateLayerOpacity() const;

  signals:
    void stateChanged(States newState, States oldState);
    void stateLayerOpacityChanged(float opacity);

  private:
    void animateOpacityTo(float from, float to);
    float targetOpacityForState(States s) const;

    States m_state = State::StateNormal;
    float m_opacity = 0.0f;
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> m_animator;
};
} // namespace cf::ui::widget::material::base