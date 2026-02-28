#pragma once
#include "color.h"
#include "components/material/cfmaterial_animation_factory.h"
#include "core/material/cfmaterial_motion.h"
#include <QObject>

class QPainter;
class QPainterPath;
class QApplication;

namespace cf::ui::widget::material::base {
using CFColor = cf::ui::base::CFColor;

class CF_UI_EXPORT MdElevationController : public QObject {
    Q_OBJECT
  public:
    explicit MdElevationController(
        cf::WeakPtr<components::material::CFMaterialAnimationFactory> factory,
        QObject* parent = nullptr);

    // 设置基础海拔级别（0-5）
    void setElevation(int level);
    int elevation() const;

    // 设置光源偏移角度（度数，正值为向右，负值为向左）
    // Material Design 默认约 15 度（从左上照射）
    void setLightSourceAngle(float degrees);
    float lightSourceAngle() const { return m_lightSourceAngle; }

    // 动态切换海拔（带动画过渡，如 FAB press）
    void animateTo(int level, const core::MotionSpec& spec);

    // 在 paintEvent 中调用（在背景之前绘制阴影）
    void paintShadow(QPainter* painter, const QPainterPath& shape);

    // 获取当前 tonal surface overlay 颜色（用于 Dark Theme 海拔叠色）
    CFColor tonalOverlay(CFColor surface, CFColor primary) const;

    // 按压效果 - 设置/获取按压状态
    void setPressed(bool pressed);
    bool isPressed() const { return m_isPressed; }

    // 获取按压偏移量（基于当前 elevation 级别）
    float pressOffset() const;

  private:
    float m_currentLevel = 0.0f;
    int m_targetLevel = 0;
    float m_lightSourceAngle = 15.0f;  // 默认15度，模拟左上斜射光
    bool m_isPressed = false;          // 按压状态
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> m_animator;

    // 根据海拔值计算阴影参数
    struct ShadowParams {
        float blurRadius;
        float offsetX;
        float offsetY;
        float opacity;
    };
    ShadowParams paramsForLevel(float level) const;
};
} // namespace cf::ui::widget::material::base