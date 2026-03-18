/**
 * @file SpinBoxDemo.cpp
 * @brief Material Design 3 SpinBox Demo - Implementation
 */

#include "SpinBoxDemo.h"

#include "ui/widget/material/widget/spinbox/spinbox.h"

#include <QApplication>
#include <QFont>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QStyle>

using namespace cf::ui::widget::material;

namespace cf::ui::example {

SpinBoxDemo::SpinBoxDemo(QWidget* parent) : QWidget(parent) {

    setupUI();
    createSpinBoxVariantsSection();
    createSpinBoxStatesSection();
    createValueRangesSection();
    createStepSizesSection();
    createInteractionDemoSection();
}

void SpinBoxDemo::setupUI() {
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

void SpinBoxDemo::createSpinBoxVariantsSection() {
    QGroupBox* groupBox = new QGroupBox("SpinBox Variants (数字输入框变体)", this);
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(16);
    gridLayout->setContentsMargins(16, 24, 16, 16);

    // Row 0: Basic SpinBox
    QLabel* basicLabel = new QLabel("Basic", this);
    basicLabel->setAlignment(Qt::AlignCenter);
    SpinBox* basicSpinBox = new SpinBox(this);
    basicSpinBox->setRange(0, 100);
    basicSpinBox->setValue(50);
    gridLayout->addWidget(basicLabel, 0, 0);
    gridLayout->addWidget(basicSpinBox, 1, 0);

    // Row 1: SpinBox with minimum value
    QLabel* minLabel = new QLabel("Minimum", this);
    minLabel->setAlignment(Qt::AlignCenter);
    SpinBox* minSpinBox = new SpinBox(this);
    minSpinBox->setRange(-100, 100);
    minSpinBox->setValue(-50);
    gridLayout->addWidget(minLabel, 0, 1);
    gridLayout->addWidget(minSpinBox, 1, 1);

    // Row 2: SpinBox with maximum value
    QLabel* maxLabel = new QLabel("Maximum", this);
    maxLabel->setAlignment(Qt::AlignCenter);
    SpinBox* maxSpinBox = new SpinBox(this);
    maxSpinBox->setRange(0, 1000);
    maxSpinBox->setValue(999);
    gridLayout->addWidget(maxLabel, 0, 2);
    gridLayout->addWidget(maxSpinBox, 1, 2);

    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 1);

    scrollContent_->layout()->addWidget(groupBox);
}

void SpinBoxDemo::createSpinBoxStatesSection() {
    QGroupBox* groupBox = createGroupBox("SpinBox States (数字输入框状态)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Normal (enabled) spinbox
    QHBoxLayout* normalLayout = new QHBoxLayout();
    QLabel* normalLabel = new QLabel("Normal:", this);
    normalLabel->setMinimumWidth(120);
    SpinBox* normalSpinBox = new SpinBox(this);
    normalSpinBox->setRange(0, 100);
    normalSpinBox->setValue(42);
    normalLayout->addWidget(normalLabel);
    normalLayout->addWidget(normalSpinBox);
    normalLayout->addStretch();
    layout->addLayout(normalLayout);

    // Disabled spinbox
    QHBoxLayout* disabledLayout = new QHBoxLayout();
    QLabel* disabledLabel = new QLabel("Disabled:", this);
    disabledLabel->setMinimumWidth(120);
    SpinBox* disabledSpinBox = new SpinBox(this);
    disabledSpinBox->setRange(0, 100);
    disabledSpinBox->setValue(50);
    disabledSpinBox->setEnabled(false);
    disabledLayout->addWidget(disabledLabel);
    disabledLayout->addWidget(disabledSpinBox);
    disabledLayout->addStretch();
    layout->addLayout(disabledLayout);

    // Read-only spinbox
    QHBoxLayout* readOnlyLayout = new QHBoxLayout();
    QLabel* readOnlyLabel = new QLabel("Read-only:", this);
    readOnlyLabel->setMinimumWidth(120);
    SpinBox* readOnlySpinBox = new SpinBox(this);
    readOnlySpinBox->setRange(0, 100);
    readOnlySpinBox->setValue(75);
    readOnlySpinBox->setReadOnly(true);
    readOnlyLayout->addWidget(readOnlyLabel);
    readOnlyLayout->addWidget(readOnlySpinBox);
    readOnlyLayout->addStretch();
    layout->addLayout(readOnlyLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void SpinBoxDemo::createValueRangesSection() {
    QGroupBox* groupBox = createGroupBox("Value Ranges (数值范围)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Small range
    QHBoxLayout* smallLayout = new QHBoxLayout();
    QLabel* smallLabel = new QLabel("Small (0-10):", this);
    smallLabel->setMinimumWidth(150);
    SpinBox* smallSpinBox = new SpinBox(this);
    smallSpinBox->setRange(0, 10);
    smallSpinBox->setValue(5);
    smallLayout->addWidget(smallLabel);
    smallLayout->addWidget(smallSpinBox);
    smallLayout->addStretch();
    layout->addLayout(smallLayout);

    // Medium range
    QHBoxLayout* mediumLayout = new QHBoxLayout();
    QLabel* mediumLabel = new QLabel("Medium (0-100):", this);
    mediumLabel->setMinimumWidth(150);
    SpinBox* mediumSpinBox = new SpinBox(this);
    mediumSpinBox->setRange(0, 100);
    mediumSpinBox->setValue(50);
    mediumLayout->addWidget(mediumLabel);
    mediumLayout->addWidget(mediumSpinBox);
    mediumLayout->addStretch();
    layout->addLayout(mediumLayout);

    // Large range
    QHBoxLayout* largeLayout = new QHBoxLayout();
    QLabel* largeLabel = new QLabel("Large (0-1000):", this);
    largeLabel->setMinimumWidth(150);
    SpinBox* largeSpinBox = new SpinBox(this);
    largeSpinBox->setRange(0, 1000);
    largeSpinBox->setValue(500);
    largeLayout->addWidget(largeLabel);
    largeLayout->addWidget(largeSpinBox);
    largeLayout->addStretch();
    layout->addLayout(largeLayout);

    // Negative range
    QHBoxLayout* negativeLayout = new QHBoxLayout();
    QLabel* negativeLabel = new QLabel("Negative (-100 to 100):", this);
    negativeLabel->setMinimumWidth(150);
    SpinBox* negativeSpinBox = new SpinBox(this);
    negativeSpinBox->setRange(-100, 100);
    negativeSpinBox->setValue(0);
    negativeLayout->addWidget(negativeLabel);
    negativeLayout->addWidget(negativeSpinBox);
    negativeLayout->addStretch();
    layout->addLayout(negativeLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void SpinBoxDemo::createStepSizesSection() {
    QGroupBox* groupBox = createGroupBox("Step Sizes (步长)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Single step
    QHBoxLayout* singleLayout = new QHBoxLayout();
    QLabel* singleLabel = new QLabel("Step 1:", this);
    singleLabel->setMinimumWidth(150);
    SpinBox* singleSpinBox = new SpinBox(this);
    singleSpinBox->setRange(0, 100);
    singleSpinBox->setValue(50);
    singleSpinBox->setSingleStep(1);
    singleLayout->addWidget(singleLabel);
    singleLayout->addWidget(singleSpinBox);
    singleLayout->addStretch();
    layout->addLayout(singleLayout);

    // Step of 5
    QHBoxLayout* step5Layout = new QHBoxLayout();
    QLabel* step5Label = new QLabel("Step 5:", this);
    step5Label->setMinimumWidth(150);
    SpinBox* step5SpinBox = new SpinBox(this);
    step5SpinBox->setRange(0, 100);
    step5SpinBox->setValue(50);
    step5SpinBox->setSingleStep(5);
    step5Layout->addWidget(step5Label);
    step5Layout->addWidget(step5SpinBox);
    step5Layout->addStretch();
    layout->addLayout(step5Layout);

    // Step of 10
    QHBoxLayout* step10Layout = new QHBoxLayout();
    QLabel* step10Label = new QLabel("Step 10:", this);
    step10Label->setMinimumWidth(150);
    SpinBox* step10SpinBox = new SpinBox(this);
    step10SpinBox->setRange(0, 200);
    step10SpinBox->setValue(100);
    step10SpinBox->setSingleStep(10);
    step10Layout->addWidget(step10Label);
    step10Layout->addWidget(step10SpinBox);
    step10Layout->addStretch();
    layout->addLayout(step10Layout);

    // Step of 100
    QHBoxLayout* step100Layout = new QHBoxLayout();
    QLabel* step100Label = new QLabel("Step 100:", this);
    step100Label->setMinimumWidth(150);
    SpinBox* step100SpinBox = new SpinBox(this);
    step100SpinBox->setRange(0, 10000);
    step100SpinBox->setValue(5000);
    step100SpinBox->setSingleStep(100);
    step100Layout->addWidget(step100Label);
    step100Layout->addWidget(step100SpinBox);
    step100Layout->addStretch();
    layout->addLayout(step100Layout);

    scrollContent_->layout()->addWidget(groupBox);
}

void SpinBoxDemo::createInteractionDemoSection() {
    QGroupBox* groupBox = createGroupBox("SpinBox Interaction (数字输入框交互)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Value change demo
    QHBoxLayout* valueLayout = new QHBoxLayout();
    QLabel* hintLabel = new QLabel("Adjust value to see changes:", this);
    hintLabel->setMinimumWidth(200);
    SpinBox* demoSpinBox = new SpinBox(this);
    demoSpinBox->setRange(0, 100);
    demoSpinBox->setValue(50);
    connect(demoSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &SpinBoxDemo::onDemoSpinBoxValueChanged);
    valueLayout->addWidget(hintLabel);
    valueLayout->addWidget(demoSpinBox);
    valueLayout->addStretch();
    layout->addLayout(valueLayout);

    // Status label
    QHBoxLayout* statusLayout = new QHBoxLayout();
    QLabel* statusLabel = new QLabel("Current value:", this);
    statusLabel->setMinimumWidth(200);
    QLabel* valueDisplayLabel = new QLabel("50", this);
    valueDisplayLabel->setObjectName("valueDisplay");
    valueDisplayLabel->setStyleSheet("QLabel#valueDisplay { font-weight: bold; color: #6750A4; }");
    statusLayout->addWidget(statusLabel);
    statusLayout->addWidget(valueDisplayLabel);
    statusLayout->addStretch();
    layout->addLayout(statusLayout);

    // Store the label for updating
    demoSpinBox->setProperty("valueLabel", QVariant::fromValue(valueDisplayLabel));

    scrollContent_->layout()->addWidget(groupBox);
}

void SpinBoxDemo::onDemoSpinBoxValueChanged(int value) {
    SpinBox* spinBox = qobject_cast<SpinBox*>(sender());
    if (spinBox) {
        QLabel* valueLabel = spinBox->property("valueLabel").value<QLabel*>();
        if (valueLabel) {
            valueLabel->setText(QString::number(value));
        }
    }
}

QGroupBox* SpinBoxDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
