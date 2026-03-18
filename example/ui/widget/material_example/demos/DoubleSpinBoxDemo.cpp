/**
 * @file DoubleSpinBoxDemo.cpp
 * @brief Material Design 3 DoubleSpinBox Demo - Implementation
 */

#include "DoubleSpinBoxDemo.h"

#include "ui/widget/material/widget/doublespinbox/doublespinbox.h"
#include <QApplication>
#include <QFont>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QStyle>
#include <QVBoxLayout>

using namespace cf::ui::widget::material;

namespace cf::ui::example {

DoubleSpinBoxDemo::DoubleSpinBoxDemo(QWidget* parent) : QWidget(parent) {

    setupUI();
    createVariantsSection();
    createStatesSection();
    createPrecisionSection();
    createValueRangeSection();
    createStepSizeSection();
    createInteractionDemoSection();
}

void DoubleSpinBoxDemo::setupUI() {
    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(24, 24, 24, 24);
    layout_->setSpacing(16);

    // Scroll area for content
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollContent_ = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent_);
    scrollLayout->setSpacing(24);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidget(scrollContent_);
    layout_->addWidget(scrollArea, 1);
}

void DoubleSpinBoxDemo::createVariantsSection() {
    QGroupBox* groupBox = new QGroupBox("Basic Variants (基本变体)", this);
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(16);
    gridLayout->setContentsMargins(16, 24, 16, 16);

    // Default DoubleSpinBox
    QLabel* defaultLabel = new QLabel("Default:", this);
    DoubleSpinBox* defaultSpinBox = new DoubleSpinBox(this);
    defaultSpinBox->setRange(0.0, 100.0);
    defaultSpinBox->setValue(50.0);
    gridLayout->addWidget(defaultLabel, 0, 0);
    gridLayout->addWidget(defaultSpinBox, 0, 1);

    // With prefix
    QLabel* prefixLabel = new QLabel("With Prefix:", this);
    DoubleSpinBox* prefixSpinBox = new DoubleSpinBox(this);
    prefixSpinBox->setRange(0.0, 1000.0);
    prefixSpinBox->setValue(100.0);
    prefixSpinBox->setPrefix("$");
    gridLayout->addWidget(prefixLabel, 1, 0);
    gridLayout->addWidget(prefixSpinBox, 1, 1);

    // With suffix
    QLabel* suffixLabel = new QLabel("With Suffix:", this);
    DoubleSpinBox* suffixSpinBox = new DoubleSpinBox(this);
    suffixSpinBox->setRange(0.0, 100.0);
    suffixSpinBox->setValue(25.5);
    suffixSpinBox->setSuffix(" %");
    gridLayout->addWidget(suffixLabel, 2, 0);
    gridLayout->addWidget(suffixSpinBox, 2, 1);

    // With both prefix and suffix
    QLabel* bothLabel = new QLabel("Prefix + Suffix:", this);
    DoubleSpinBox* bothSpinBox = new DoubleSpinBox(this);
    bothSpinBox->setRange(-100.0, 100.0);
    bothSpinBox->setValue(0.0);
    bothSpinBox->setPrefix("+/- ");
    bothSpinBox->setSuffix(" deg");
    gridLayout->addWidget(bothLabel, 3, 0);
    gridLayout->addWidget(bothSpinBox, 3, 1);

    gridLayout->setColumnStretch(1, 1);
    scrollContent_->layout()->addWidget(groupBox);
}

void DoubleSpinBoxDemo::createStatesSection() {
    QGroupBox* groupBox = createGroupBox("States (状态)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Normal (enabled) state
    QHBoxLayout* normalLayout = new QHBoxLayout();
    QLabel* normalLabel = new QLabel("Normal (Enabled):", this);
    normalLabel->setMinimumWidth(150);
    DoubleSpinBox* normalSpinBox = new DoubleSpinBox(this);
    normalSpinBox->setRange(0.0, 100.0);
    normalSpinBox->setValue(50.0);
    normalLayout->addWidget(normalLabel);
    normalLayout->addWidget(normalSpinBox);
    normalLayout->addStretch();
    layout->addLayout(normalLayout);

    // Disabled state
    QHBoxLayout* disabledLayout = new QHBoxLayout();
    QLabel* disabledLabel = new QLabel("Disabled:", this);
    disabledLabel->setMinimumWidth(150);
    DoubleSpinBox* disabledSpinBox = new DoubleSpinBox(this);
    disabledSpinBox->setRange(0.0, 100.0);
    disabledSpinBox->setValue(50.0);
    disabledSpinBox->setEnabled(false);
    disabledLayout->addWidget(disabledLabel);
    disabledLayout->addWidget(disabledSpinBox);
    disabledLayout->addStretch();
    layout->addLayout(disabledLayout);

    // Read-only state
    QHBoxLayout* readOnlyLayout = new QHBoxLayout();
    QLabel* readOnlyLabel = new QLabel("Read-only:", this);
    readOnlyLabel->setMinimumWidth(150);
    DoubleSpinBox* readOnlySpinBox = new DoubleSpinBox(this);
    readOnlySpinBox->setRange(0.0, 100.0);
    readOnlySpinBox->setValue(75.0);
    readOnlySpinBox->setReadOnly(true);
    readOnlyLayout->addWidget(readOnlyLabel);
    readOnlyLayout->addWidget(readOnlySpinBox);
    readOnlyLayout->addStretch();
    layout->addLayout(readOnlyLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void DoubleSpinBoxDemo::createPrecisionSection() {
    QGroupBox* groupBox = new QGroupBox("Decimal Precision (小数精度)", this);
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(12);
    gridLayout->setContentsMargins(16, 24, 16, 16);

    // No decimals (0)
    QLabel* intLabel = new QLabel("Integer (0 decimals):", this);
    DoubleSpinBox* intSpinBox = new DoubleSpinBox(this);
    intSpinBox->setRange(0.0, 1000.0);
    intSpinBox->setValue(50);
    intSpinBox->setDecimals(0);
    gridLayout->addWidget(intLabel, 0, 0);
    gridLayout->addWidget(intSpinBox, 0, 1);

    // 1 decimal place
    QLabel* oneDecimalLabel = new QLabel("1 decimal place:", this);
    DoubleSpinBox* oneDecimalSpinBox = new DoubleSpinBox(this);
    oneDecimalSpinBox->setRange(0.0, 100.0);
    oneDecimalSpinBox->setValue(50.5);
    oneDecimalSpinBox->setDecimals(1);
    gridLayout->addWidget(oneDecimalLabel, 1, 0);
    gridLayout->addWidget(oneDecimalSpinBox, 1, 1);

    // 2 decimal places (default)
    QLabel* twoDecimalLabel = new QLabel("2 decimal places:", this);
    DoubleSpinBox* twoDecimalSpinBox = new DoubleSpinBox(this);
    twoDecimalSpinBox->setRange(0.0, 100.0);
    twoDecimalSpinBox->setValue(50.25);
    twoDecimalSpinBox->setDecimals(2);
    gridLayout->addWidget(twoDecimalLabel, 2, 0);
    gridLayout->addWidget(twoDecimalSpinBox, 2, 1);

    // 3 decimal places
    QLabel* threeDecimalLabel = new QLabel("3 decimal places:", this);
    DoubleSpinBox* threeDecimalSpinBox = new DoubleSpinBox(this);
    threeDecimalSpinBox->setRange(0.0, 100.0);
    threeDecimalSpinBox->setValue(50.125);
    threeDecimalSpinBox->setDecimals(3);
    gridLayout->addWidget(threeDecimalLabel, 3, 0);
    gridLayout->addWidget(threeDecimalSpinBox, 3, 1);

    // 4 decimal places
    QLabel* fourDecimalLabel = new QLabel("4 decimal places:", this);
    DoubleSpinBox* fourDecimalSpinBox = new DoubleSpinBox(this);
    fourDecimalSpinBox->setRange(0.0, 100.0);
    fourDecimalSpinBox->setValue(50.0625);
    fourDecimalSpinBox->setDecimals(4);
    gridLayout->addWidget(fourDecimalLabel, 4, 0);
    gridLayout->addWidget(fourDecimalSpinBox, 4, 1);

    gridLayout->setColumnStretch(1, 1);
    scrollContent_->layout()->addWidget(groupBox);
}

void DoubleSpinBoxDemo::createValueRangeSection() {
    QGroupBox* groupBox = new QGroupBox("Value Ranges (数值范围)", this);
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(12);
    gridLayout->setContentsMargins(16, 24, 16, 16);

    // 0 to 1 range (normalized)
    QLabel* normalizedLabel = new QLabel("Normalized [0, 1]:", this);
    DoubleSpinBox* normalizedSpinBox = new DoubleSpinBox(this);
    normalizedSpinBox->setRange(0.0, 1.0);
    normalizedSpinBox->setValue(0.5);
    normalizedSpinBox->setDecimals(3);
    normalizedSpinBox->setSingleStep(0.01);
    gridLayout->addWidget(normalizedLabel, 0, 0);
    gridLayout->addWidget(normalizedSpinBox, 0, 1);

    // Negative to positive range
    QLabel* centeredLabel = new QLabel("Centered [-100, 100]:", this);
    DoubleSpinBox* centeredSpinBox = new DoubleSpinBox(this);
    centeredSpinBox->setRange(-100.0, 100.0);
    centeredSpinBox->setValue(0.0);
    gridLayout->addWidget(centeredLabel, 1, 0);
    gridLayout->addWidget(centeredSpinBox, 1, 1);

    // Large range
    QLabel* largeLabel = new QLabel("Large range [0, 10000]:", this);
    DoubleSpinBox* largeSpinBox = new DoubleSpinBox(this);
    largeSpinBox->setRange(0.0, 10000.0);
    largeSpinBox->setValue(5000.0);
    largeSpinBox->setSingleStep(100.0);
    gridLayout->addWidget(largeLabel, 2, 0);
    gridLayout->addWidget(largeSpinBox, 2, 1);

    // Small fractional range
    QLabel* smallLabel = new QLabel("Fractional [0, 0.1]:", this);
    DoubleSpinBox* smallSpinBox = new DoubleSpinBox(this);
    smallSpinBox->setRange(0.0, 0.1);
    smallSpinBox->setValue(0.05);
    smallSpinBox->setDecimals(4);
    smallSpinBox->setSingleStep(0.001);
    gridLayout->addWidget(smallLabel, 3, 0);
    gridLayout->addWidget(smallSpinBox, 3, 1);

    gridLayout->setColumnStretch(1, 1);
    scrollContent_->layout()->addWidget(groupBox);
}

void DoubleSpinBoxDemo::createStepSizeSection() {
    QGroupBox* groupBox = new QGroupBox("Step Sizes (步长)", this);
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(12);
    gridLayout->setContentsMargins(16, 24, 16, 16);

    // Small step (0.01)
    QLabel* smallStepLabel = new QLabel("Small step (0.01):", this);
    DoubleSpinBox* smallStepSpinBox = new DoubleSpinBox(this);
    smallStepSpinBox->setRange(0.0, 10.0);
    smallStepSpinBox->setValue(5.0);
    smallStepSpinBox->setDecimals(2);
    smallStepSpinBox->setSingleStep(0.01);
    gridLayout->addWidget(smallStepLabel, 0, 0);
    gridLayout->addWidget(smallStepSpinBox, 0, 1);

    // Medium step (0.5)
    QLabel* mediumStepLabel = new QLabel("Medium step (0.5):", this);
    DoubleSpinBox* mediumStepSpinBox = new DoubleSpinBox(this);
    mediumStepSpinBox->setRange(0.0, 100.0);
    mediumStepSpinBox->setValue(50.0);
    mediumStepSpinBox->setDecimals(1);
    mediumStepSpinBox->setSingleStep(0.5);
    gridLayout->addWidget(mediumStepLabel, 1, 0);
    gridLayout->addWidget(mediumStepSpinBox, 1, 1);

    // Large step (10.0)
    QLabel* largeStepLabel = new QLabel("Large step (10.0):", this);
    DoubleSpinBox* largeStepSpinBox = new DoubleSpinBox(this);
    largeStepSpinBox->setRange(0.0, 1000.0);
    largeStepSpinBox->setValue(500.0);
    largeStepSpinBox->setSingleStep(10.0);
    gridLayout->addWidget(largeStepLabel, 2, 0);
    gridLayout->addWidget(largeStepSpinBox, 2, 1);

    // Very large step (100.0)
    QLabel* veryLargeStepLabel = new QLabel("Very large step (100.0):", this);
    DoubleSpinBox* veryLargeStepSpinBox = new DoubleSpinBox(this);
    veryLargeStepSpinBox->setRange(0.0, 10000.0);
    veryLargeStepSpinBox->setValue(5000.0);
    veryLargeStepSpinBox->setSingleStep(100.0);
    gridLayout->addWidget(veryLargeStepLabel, 3, 0);
    gridLayout->addWidget(veryLargeStepSpinBox, 3, 1);

    gridLayout->setColumnStretch(1, 1);
    scrollContent_->layout()->addWidget(groupBox);
}

void DoubleSpinBoxDemo::createInteractionDemoSection() {
    QGroupBox* groupBox = createGroupBox("Interaction Demo (交互演示)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Value tracking demo
    QHBoxLayout* valueLayout = new QHBoxLayout();
    QLabel* valueLabel = new QLabel("Adjust value:", this);
    valueLabel->setMinimumWidth(150);
    DoubleSpinBox* valueSpinBox = new DoubleSpinBox(this);
    valueSpinBox->setRange(0.0, 100.0);
    valueSpinBox->setValue(50.0);
    valueSpinBox->setSuffix(" units");
    connect(valueSpinBox, QOverload<double>::of(&DoubleSpinBox::valueChanged), this,
            &DoubleSpinBoxDemo::onValueValueChanged);
    valueDisplayLabel_ = new QLabel("Current value: 50.00 units", this);
    valueDisplayLabel_->setMinimumWidth(150);
    valueLayout->addWidget(valueLabel);
    valueLayout->addWidget(valueSpinBox);
    valueLayout->addWidget(valueDisplayLabel_);
    valueLayout->addStretch();
    layout->addLayout(valueLayout);

    // Price demo
    QHBoxLayout* priceLayout = new QHBoxLayout();
    QLabel* priceLabel = new QLabel("Enter price:", this);
    priceLabel->setMinimumWidth(150);
    DoubleSpinBox* priceSpinBox = new DoubleSpinBox(this);
    priceSpinBox->setRange(0.0, 999999.99);
    priceSpinBox->setValue(99.99);
    priceSpinBox->setPrefix("$");
    priceSpinBox->setDecimals(2);
    connect(priceSpinBox, QOverload<double>::of(&DoubleSpinBox::valueChanged), this,
            &DoubleSpinBoxDemo::onPriceValueChanged);
    priceDisplayLabel_ = new QLabel("Price: $99.99", this);
    priceDisplayLabel_->setMinimumWidth(150);
    priceLayout->addWidget(priceLabel);
    priceLayout->addWidget(priceSpinBox);
    priceLayout->addWidget(priceDisplayLabel_);
    priceLayout->addStretch();
    layout->addLayout(priceLayout);

    // Temperature demo
    QHBoxLayout* tempLayout = new QHBoxLayout();
    QLabel* tempLabel = new QLabel("Temperature:", this);
    tempLabel->setMinimumWidth(150);
    DoubleSpinBox* tempSpinBox = new DoubleSpinBox(this);
    tempSpinBox->setRange(-273.15, 1000.0);
    tempSpinBox->setValue(20.0);
    tempSpinBox->setDecimals(1);
    tempSpinBox->setSuffix(" °C");
    connect(tempSpinBox, QOverload<double>::of(&DoubleSpinBox::valueChanged), this,
            &DoubleSpinBoxDemo::onTemperatureValueChanged);
    temperatureDisplayLabel_ = new QLabel("20.0 °C - Room temperature", this);
    temperatureDisplayLabel_->setMinimumWidth(200);
    tempLayout->addWidget(tempLabel);
    tempLayout->addWidget(tempSpinBox);
    tempLayout->addWidget(temperatureDisplayLabel_);
    tempLayout->addStretch();
    layout->addLayout(tempLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void DoubleSpinBoxDemo::onValueValueChanged(double value) {
    valueDisplayLabel_->setText(QString("Current value: %1 units").arg(value, 0, 'f', 2));
}

void DoubleSpinBoxDemo::onPriceValueChanged(double value) {
    priceDisplayLabel_->setText(QString("Price: $%1").arg(value, 0, 'f', 2));
}

void DoubleSpinBoxDemo::onTemperatureValueChanged(double value) {
    QString status;
    if (value < 0) {
        status = "Below freezing";
    } else if (value < 15) {
        status = "Cold";
    } else if (value < 25) {
        status = "Room temperature";
    } else if (value < 35) {
        status = "Warm";
    } else {
        status = "Hot";
    }
    temperatureDisplayLabel_->setText(QString("%1 °C - %2").arg(value, 0, 'f', 1).arg(status));
}

QGroupBox* DoubleSpinBoxDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
