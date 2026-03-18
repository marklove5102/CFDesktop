/**
 * @file SpinBoxDemo.h
 * @brief Material Design 3 SpinBox Demo - Widget Component
 */

#pragma once

#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
class QGroupBox;

namespace cf::ui::example {

class SpinBoxDemo : public QWidget {
    Q_OBJECT

  public:
    explicit SpinBoxDemo(QWidget* parent = nullptr);
    ~SpinBoxDemo() override = default;

    QString title() const { return "SpinBox"; }
    QString description() const { return "Material Design 3 SpinBox Component"; }

  private:
    void setupUI();
    void createSpinBoxVariantsSection();
    void createSpinBoxStatesSection();
    void createValueRangesSection();
    void createStepSizesSection();
    void createInteractionDemoSection();
    QGroupBox* createGroupBox(const QString& title);

    void onDemoSpinBoxValueChanged(int value);

    QWidget* scrollContent_;
    QVBoxLayout* layout_;
};

} // namespace cf::ui::example
