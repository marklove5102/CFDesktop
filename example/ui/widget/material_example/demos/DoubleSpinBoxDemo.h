/**
 * @file DoubleSpinBoxDemo.h
 * @brief Material Design 3 DoubleSpinBox Demo - Widget Component
 */

#pragma once

#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
class QGroupBox;
class QVBoxLayout;
class QLabel;

namespace cf::ui::example {

class DoubleSpinBoxDemo : public QWidget {
    Q_OBJECT

  public:
    explicit DoubleSpinBoxDemo(QWidget* parent = nullptr);
    ~DoubleSpinBoxDemo() override = default;

    QString title() const { return "DoubleSpinBox"; }
    QString description() const { return "Material Design 3 DoubleSpinBox Component"; }

  private:
    void setupUI();
    void createVariantsSection();
    void createStatesSection();
    void createPrecisionSection();
    void createValueRangeSection();
    void createStepSizeSection();
    void createInteractionDemoSection();
    QGroupBox* createGroupBox(const QString& title);

    void onValueValueChanged(double value);
    void onPriceValueChanged(double value);
    void onTemperatureValueChanged(double value);

    QWidget* scrollContent_;
    QVBoxLayout* layout_;
    QLabel* valueDisplayLabel_;
    QLabel* priceDisplayLabel_;
    QLabel* temperatureDisplayLabel_;
};

} // namespace cf::ui::example
