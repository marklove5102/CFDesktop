#pragma once
#include "color.h"
#include "components/material/cfmaterial_animation_factory.h"
#include "core/material/cfmaterial_motion.h"
#include <QObject>
class QPainter;
class QPainterPath;

namespace cf::ui::widget::material::base {
class CF_UI_EXPORT MdFocusIndicator : public QObject {
    Q_OBJECT
  public:
    explicit MdFocusIndicator(cf::WeakPtr<components::material::CFMaterialAnimationFactory> factory,
                              QObject* parent = nullptr);

    void onFocusIn();
    void onFocusOut();

    // 在 paintEvent 中调用（在最顶层绘制）
    void paint(QPainter* painter, const QPainterPath& shape,
               const cf::ui::base::CFColor& indicatorColor);

  private:
    float m_progress = 0.0f;
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> m_animator;
};
} // namespace cf::ui::widget::material::base