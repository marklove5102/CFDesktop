#pragma once
#include "color.h"
#include "components/material/cfmaterial_animation_factory.h"
#include <QObject>
#include <QPointF>
class QPainterPath;

namespace cf::ui::widget::material::base {

class CF_UI_EXPORT RippleHelper : public QObject {
    Q_OBJECT
  public:
    explicit RippleHelper(cf::WeakPtr<components::material::CFMaterialAnimationFactory> factory,
                          QObject* parent);
    struct MdRipple {
        QPointF center;
        float radius;
        float opacity;
        bool releasing;
        float maxRadius; // Calculated maximum radius for this ripple
    };

    enum class Mode {
        Bounded,  // Clipped By Widgets
        Unbounded // And not :)
    };
    Q_ENUM(Mode)

    void setMode(Mode mode);
    void setColor(const cf::ui::base::CFColor& color); // 涟漪颜色，通常为 stateColor

    // 事件接口
    void onPress(const QPoint& pos, const QRectF& widgetRect);
    void onRelease();
    void onCancel(); // 如鼠标移出时取消未释放的 ripple

    // paintEvent 中调用，clipPath 用于 Bounded 模式
    void paint(QPainter* painter, const QPainterPath& clipPath);

    // 是否有活跃 ripple（供 paintEvent 判断是否需要 update()）
    bool hasActiveRipple() const;

  signals:
    void repaintNeeded(); // 连接到 QWidget::update()

  private:
    QList<MdRipple> m_ripples;
    Mode m_mode = Mode::Bounded;
    cf::ui::base::CFColor m_color;
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> m_animator;

    // 计算 ripple 最终半径（控件对角线长度）
    float maxRadius(const QRectF& rect, const QPointF& center) const;
};

} // namespace cf::ui::widget::material::base