/**
 * @file RadioButtonExampleWindow.cpp
 * @brief Material Design 3 RadioButton Example - Implementation
 */

#include "RadioButtonExampleWindow.h"

#include "ui/widget/material/widget/radiobutton/radiobutton.h"

#include <QApplication>
#include <QButtonGroup>
#include <QFont>
#include <QStyle>

using namespace cf::ui::widget::material;

namespace cf::ui::example {

RadioButtonExampleWindow::RadioButtonExampleWindow(QWidget* parent) : QMainWindow(parent) {

    setupUI();
    createHeader();
    createBasicRadioSection();
    createRadioStatesSection();
    createButtonGroupSection();
    createErrorStateSection();
    createInteractionDemoSection();

    // Let window auto-adjust to content size
    adjustSize();
}

RadioButtonExampleWindow::~RadioButtonExampleWindow() = default;

void RadioButtonExampleWindow::setupUI() {
    // Window setup
    setWindowTitle("Material RadioButton Example");
    resize(700, 800);
    setMinimumSize(500, 600);

    // Central widget
    centralWidget_ = new QWidget(this);
    setCentralWidget(centralWidget_);

    mainLayout_ = new QVBoxLayout(centralWidget_);
    mainLayout_->setContentsMargins(24, 24, 24, 24);
    mainLayout_->setSpacing(16);

    // Scroll area for content
    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setFrameShape(QFrame::NoFrame);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollContent_ = new QWidget();
    scrollLayout_ = new QVBoxLayout(scrollContent_);
    scrollLayout_->setSpacing(24);
    scrollLayout_->setContentsMargins(0, 0, 0, 0);

    scrollArea_->setWidget(scrollContent_);
    mainLayout_->addWidget(scrollArea_);
}

void RadioButtonExampleWindow::createHeader() {
    // Title
    titleLabel_ = new QLabel("Material Design 3 Radio Buttons", this);
    QFont titleFont("Segoe UI", 24, QFont::Bold);
    titleLabel_->setFont(titleFont);
    scrollLayout_->addWidget(titleLabel_);

    // Subtitle
    subtitleLabel_ = new QLabel(
        "Explore Material Design 3 radio buttons with mutual exclusion, "
        "states, and error handling.",
        this);
    QFont subtitleFont("Segoe UI", 11);
    subtitleLabel_->setFont(subtitleFont);
    subtitleLabel_->setWordWrap(true);
    scrollLayout_->addWidget(subtitleLabel_);

    // Add some spacing
    scrollLayout_->addSpacing(16);
}

void RadioButtonExampleWindow::createBasicRadioSection() {
    QGroupBox* groupBox = new QGroupBox("Basic Radio Buttons (基本单选按钮)", this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);

    // Create a button group for mutual exclusion
    QButtonGroup* buttonGroup = new QButtonGroup(this);

    // Option 1
    RadioButton* option1 = new RadioButton("Option 1 - First choice", this);
    option1->setChecked(true); // Default checked
    buttonGroup->addButton(option1, 1);
    layout->addWidget(option1);

    // Option 2
    RadioButton* option2 = new RadioButton("Option 2 - Second choice", this);
    buttonGroup->addButton(option2, 2);
    layout->addWidget(option2);

    // Option 3
    RadioButton* option3 = new RadioButton("Option 3 - Third choice", this);
    buttonGroup->addButton(option3, 3);
    layout->addWidget(option3);

    scrollLayout_->addWidget(groupBox);
}

void RadioButtonExampleWindow::createRadioStatesSection() {
    QGroupBox* groupBox = createGroupBox("Radio Button States (按钮状态)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Normal (enabled) radio buttons
    QHBoxLayout* normalLayout = new QHBoxLayout();
    QLabel* normalLabel = new QLabel("Normal:", this);
    normalLabel->setMinimumWidth(120);

    QButtonGroup* normalGroup = new QButtonGroup(this);
    RadioButton* normalRadio1 = new RadioButton("Enabled A", this);
    RadioButton* normalRadio2 = new RadioButton("Enabled B", this);
    normalRadio1->setChecked(true);
    normalGroup->addButton(normalRadio1);
    normalGroup->addButton(normalRadio2);

    normalLayout->addWidget(normalLabel);
    normalLayout->addWidget(normalRadio1);
    normalLayout->addWidget(normalRadio2);
    normalLayout->addStretch();
    layout->addLayout(normalLayout);

    // Disabled radio buttons
    QHBoxLayout* disabledLayout = new QHBoxLayout();
    QLabel* disabledLabel = new QLabel("Disabled:", this);
    disabledLabel->setMinimumWidth(120);

    QButtonGroup* disabledGroup = new QButtonGroup(this);
    RadioButton* disabledRadio1 = new RadioButton("Disabled Option 1", this);
    RadioButton* disabledRadio2 = new RadioButton("Disabled Option 2", this);
    disabledRadio1->setChecked(true);
    disabledRadio1->setEnabled(false);
    disabledRadio2->setEnabled(false);
    disabledGroup->addButton(disabledRadio1);
    disabledGroup->addButton(disabledRadio2);

    disabledLayout->addWidget(disabledLabel);
    disabledLayout->addWidget(disabledRadio1);
    disabledLayout->addWidget(disabledRadio2);
    disabledLayout->addStretch();
    layout->addLayout(disabledLayout);

    scrollLayout_->addWidget(groupBox);
}

void RadioButtonExampleWindow::createButtonGroupSection() {
    QGroupBox* groupBox = createGroupBox("Independent Button Groups (独立按钮组)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Group 1: Color selection
    QLabel* group1Label = new QLabel("Select Color:", this);
    layout->addWidget(group1Label);

    QHBoxLayout* group1Layout = new QHBoxLayout();
    QButtonGroup* colorGroup = new QButtonGroup(this);
    RadioButton* redRadio = new RadioButton("Red", this);
    RadioButton* greenRadio = new RadioButton("Green", this);
    RadioButton* blueRadio = new RadioButton("Blue", this);
    redRadio->setChecked(true);
    colorGroup->addButton(redRadio);
    colorGroup->addButton(greenRadio);
    colorGroup->addButton(blueRadio);
    group1Layout->addWidget(redRadio);
    group1Layout->addWidget(greenRadio);
    group1Layout->addWidget(blueRadio);
    group1Layout->addStretch();
    layout->addLayout(group1Layout);

    // Add spacing between groups
    layout->addSpacing(16);

    // Group 2: Size selection
    QLabel* group2Label = new QLabel("Select Size:", this);
    layout->addWidget(group2Label);

    QHBoxLayout* group2Layout = new QHBoxLayout();
    QButtonGroup* sizeGroup = new QButtonGroup(this);
    RadioButton* smallRadio = new RadioButton("Small", this);
    RadioButton* mediumRadio = new RadioButton("Medium", this);
    RadioButton* largeRadio = new RadioButton("Large", this);
    mediumRadio->setChecked(true);
    sizeGroup->addButton(smallRadio);
    sizeGroup->addButton(mediumRadio);
    sizeGroup->addButton(largeRadio);
    group2Layout->addWidget(smallRadio);
    group2Layout->addWidget(mediumRadio);
    group2Layout->addWidget(largeRadio);
    group2Layout->addStretch();
    layout->addLayout(group2Layout);

    scrollLayout_->addWidget(groupBox);
}

void RadioButtonExampleWindow::createErrorStateSection() {
    QGroupBox* groupBox = createGroupBox("Error State (错误状态)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Error state radio buttons
    QHBoxLayout* errorLayout = new QHBoxLayout();
    QLabel* errorLabel = new QLabel("Error State:", this);
    errorLabel->setMinimumWidth(120);

    QButtonGroup* errorGroup = new QButtonGroup(this);
    RadioButton* errorRadio1 = new RadioButton("Invalid Choice", this);
    RadioButton* errorRadio2 = new RadioButton("Another Invalid", this);
    errorRadio1->setError(true);
    errorRadio2->setError(true);
    errorRadio1->setChecked(true);
    errorGroup->addButton(errorRadio1);
    errorGroup->addButton(errorRadio2);

    errorLayout->addWidget(errorLabel);
    errorLayout->addWidget(errorRadio1);
    errorLayout->addWidget(errorRadio2);
    errorLayout->addStretch();
    layout->addLayout(errorLayout);

    // Toggle button
    QHBoxLayout* toggleLayout = new QHBoxLayout();
    QLabel* toggleLabel = new QLabel("Toggle Error:", this);
    toggleLabel->setMinimumWidth(120);

    QButtonGroup* toggleGroup = new QButtonGroup(this);
    RadioButton* toggleRadio1 = new RadioButton("Valid Option", this);
    RadioButton* toggleRadio2 = new RadioButton("Error Option", this);
    toggleRadio1->setChecked(true);
    toggleGroup->addButton(toggleRadio1, 1);
    toggleGroup->addButton(toggleRadio2, 2);

    toggleLayout->addWidget(toggleLabel);
    toggleLayout->addWidget(toggleRadio1);
    toggleLayout->addWidget(toggleRadio2);
    toggleLayout->addStretch();
    layout->addLayout(toggleLayout);

    // Connect to toggle error state
    connect(toggleGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
            this, [this, toggleRadio1, toggleRadio2](QAbstractButton* button) {
                bool isError = (button == toggleRadio2);
                toggleRadio1->setError(isError);
                toggleRadio2->setError(isError);
            });

    scrollLayout_->addWidget(groupBox);
}

void RadioButtonExampleWindow::createInteractionDemoSection() {
    QGroupBox* groupBox = createGroupBox("Interaction Demo (交互演示)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Selection display
    selectionLabel_ = new QLabel("Selected: Option 1", this);
    selectionLabel_->setMinimumWidth(200);
    QFont labelFont = selectionLabel_->font();
    labelFont.setBold(true);
    selectionLabel_->setFont(labelFont);
    layout->addWidget(selectionLabel_);

    // Interactive radio buttons
    QButtonGroup* demoGroup = new QButtonGroup(this);

    RadioButton* demoOption1 = new RadioButton("Option 1", this);
    RadioButton* demoOption2 = new RadioButton("Option 2", this);
    RadioButton* demoOption3 = new RadioButton("Option 3", this);
    demoOption1->setChecked(true);

    demoGroup->addButton(demoOption1, 1);
    demoGroup->addButton(demoOption2, 2);
    demoGroup->addButton(demoOption3, 3);

    layout->addWidget(demoOption1);
    layout->addWidget(demoOption2);
    layout->addWidget(demoOption3);

    // Connect to update selection label
    connect(demoGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
            this, [this](QAbstractButton* button) {
                selectionLabel_->setText(QString("Selected: %1").arg(button->text()));
            });

    scrollLayout_->addWidget(groupBox);

    // Add stretch at the end
    scrollLayout_->addStretch();
}

void RadioButtonExampleWindow::onOption1Clicked() {
    // Reserved for future use
}

void RadioButtonExampleWindow::onOption2Clicked() {
    // Reserved for future use
}

void RadioButtonExampleWindow::onOption3Clicked() {
    // Reserved for future use
}

void RadioButtonExampleWindow::onErrorToggleClicked() {
    // Reserved for future use
}

QGroupBox* RadioButtonExampleWindow::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
