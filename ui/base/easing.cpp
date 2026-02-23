/**
 * @file easing.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design Easing Curves
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "easing.h"
#include <QPointF>

namespace cf::ui::base {
namespace Easing {

QEasingCurve fromEasingType(const Type t) {
    switch (t) {
        case Type::Emphasized:
            // cubic-bezier(0.2, 0, 0, 1.0)   进出均平滑
            return custom(0.2f, 0.0f, 0.0f, 1.0f);

        case Type::EmphasizedDecelerate:
            // cubic-bezier(0.05, 0.7, 0.1, 1.0)  入场动画
            return custom(0.05f, 0.7f, 0.1f, 1.0f);

        case Type::EmphasizedAccelerate:
            // cubic-bezier(0.3, 0, 0.8, 0.15)    离场动画
            return custom(0.3f, 0.0f, 0.8f, 0.15f);

        case Type::Standard:
            // cubic-bezier(0.2, 0, 0, 1.0)
            return custom(0.2f, 0.0f, 0.0f, 1.0f);

        case Type::StandardDecelerate:
            // cubic-bezier(0, 0, 0, 1.0)
            return custom(0.0f, 0.0f, 0.0f, 1.0f);

        case Type::StandardAccelerate:
            // cubic-bezier(0.3, 0, 1, 1)
            return custom(0.3f, 0.0f, 1.0f, 1.0f);

        case Type::Linear:
            // cubic-bezier(0, 0, 1, 1)
            return custom(0.0f, 0.0f, 1.0f, 1.0f);

        default:
            return custom(0.2f, 0.0f, 0.0f, 1.0f);
    }
}

QEasingCurve custom(float x1, float y1, float x2, float y2) {
    // QEasingCurve::BezierType requires control points as std::pair<qreal, qreal>
    // The cubic bezier curve is defined with P0=(0,0), P1=(x1,y1), P2=(x2,y2), P3=(1,1)
    QEasingCurve curve(QEasingCurve::BezierSpline);
    curve.addCubicBezierSegment(QPointF(x1, y1), QPointF(x2, y2), QPointF(1.0, 1.0));
    return curve;
}

SpringPreset springGentle() {
    // Gentle spring: 低刚度，中等阻尼，用于轻微的弹性效果
    return {120.0f, 20.0f};
}

SpringPreset springBouncy() {
    // Bouncy spring: 中等刚度，低阻尼，产生明显的回弹效果
    return {200.0f, 10.0f};
}

SpringPreset springStiff() {
    // Stiff spring: 高刚度，高阻尼，快速响应无回弹
    return {400.0f, 30.0f};
}

} // namespace Easing

} // namespace cf::ui::base
