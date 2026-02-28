#pragma once

#include <QMarginsF>

#include "base/color.h"
#include "base/include/base/weak_ptr/weak_ptr.h"
#include "cfmaterial_animation_factory.h"
#include "components/animation_factory_manager.h"
#include "export.h"
#include <QIcon>
#include <QPushButton>
#include <QWidget>

namespace cf::ui::widget::material {

// Forward declarations
namespace base {
class StateMachine;
class RippleHelper;
class MdElevationController;
class MdFocusIndicator;
} // namespace base

using CFColor = cf::ui::base::CFColor;

class CF_UI_EXPORT Button : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(ButtonVariant variant READ variant WRITE setVariant)
    Q_PROPERTY(int elevation READ elevation WRITE setElevation)
    Q_PROPERTY(bool pressEffectEnabled READ pressEffectEnabled WRITE setPressEffectEnabled)

  public:
    enum class ButtonVariant { Filled, Tonal, Outlined, Text, Elevated };
    Q_ENUM(ButtonVariant);

    explicit Button(ButtonVariant variant = ButtonVariant::Filled, QWidget* parent = nullptr);
    explicit Button(const QString& text, ButtonVariant variant = ButtonVariant::Filled,
                    QWidget* parent = nullptr);
    ~Button() override;

    int elevation() const;
    void setElevation(int level);

    // Set light source angle for shadow (degrees: -90 to 90)
    // Default is 15 degrees (light from top-left)
    void setLightSourceAngle(float degrees);
    float lightSourceAngle() const;

    void setLeadingIcon(const QIcon& icon);

    // Redirect setIcon to setLeadingIcon for convenience
    using QPushButton::setIcon; // Hide base overload
    void setIcon(const QIcon& icon) { setLeadingIcon(icon); }
    QIcon icon() const { return leadingIcon_; }

    // Press effect control
    bool pressEffectEnabled() const;
    void setPressEffectEnabled(bool enabled);

    ButtonVariant variant() const;
    void setVariant(ButtonVariant variant);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void changeEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

  private:
    // Drawing helpers - 7-step paint pipeline
    QMarginsF shadowMargin() const;
    void drawShadow(QPainter& p, const QRectF& contentRect, const QPainterPath& shape);
    void drawBackground(QPainter& p, const QPainterPath& shape);
    void drawStateLayer(QPainter& p, const QPainterPath& shape);
    void drawRipple(QPainter& p, const QPainterPath& shape);
    void drawOutline(QPainter& p, const QPainterPath& shape);
    void drawContent(QPainter& p, const QRectF& contentRect);
    void drawFocusIndicator(QPainter& p, const QPainterPath& shape);

    // Color access methods
    CFColor containerColor() const;
    CFColor labelColor() const;
    CFColor stateLayerColor() const;
    CFColor outlineColor() const;
    float cornerRadius() const;
    QFont labelFont() const;

    // Behavior components
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> m_animationFactory;
    base::StateMachine* m_stateMachine;
    base::RippleHelper* m_ripple;
    base::MdElevationController* m_elevation;
    base::MdFocusIndicator* m_focusIndicator;

    ButtonVariant variant_;
    QIcon leadingIcon_;
    bool m_pressEffectEnabled = true;  // 默认开启按压效果
};

} // namespace cf::ui::widget::material
