#pragma once
#include "color.h"
#include "export.h"
#include <QObject>
class QPainter;
class QPainterPath;

namespace cf::ui::widget::material::base {

// Adaptes To Layers
class CF_UI_EXPORT PainterLayer : public QObject {
    Q_OBJECT
  public:
    explicit PainterLayer(QObject* parent);
    void setColor(const cf::ui::base::CFColor& color) { cached_color_ = color; }
    void setOpacity(float opacity) { opacity_ = opacity; }

    // using in paintEvent
    void paint(QPainter* painter, const QPainterPath& clipPath);

  protected:
    cf::ui::base::CFColor cached_color_;
    float opacity_;
};

} // namespace cf::ui::widget::material::base