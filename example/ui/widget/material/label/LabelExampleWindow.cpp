/**
 * @file LabelExampleWindow.cpp
 * @brief Material Design 3 Label Example - Implementation
 */

#include "LabelExampleWindow.h"

#include "ui/widget/material/widget/label/label.h"

#include <QApplication>
#include <QFont>
#include <QLabel>

using namespace cf::ui::widget::material;
using TypographyStyle = cf::ui::widget::material::TypographyStyle;
using LabelColorVariant = cf::ui::widget::material::LabelColorVariant;

namespace cf::ui::example {

LabelExampleWindow::LabelExampleWindow(QWidget* parent) : QMainWindow(parent) {

    setupUI();
    createHeader();
    createDisplaySection();
    createHeadlineSection();
    createTitleSection();
    createBodySection();
    createLabelSection();
    createColorVariantsSection();
    createStatesSection();
    createFeaturesSection();

    adjustSize();
}

LabelExampleWindow::~LabelExampleWindow() = default;

void LabelExampleWindow::setupUI() {
    // Window setup
    setWindowTitle("Material Label Example");
    resize(900, 800);
    setMinimumSize(700, 600);

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

void LabelExampleWindow::createHeader() {
    // Title
    titleLabel_ = new QLabel("Material Design 3 Typography", this);
    QFont titleFont("Segoe UI", 24, QFont::Bold);
    titleLabel_->setFont(titleFont);
    scrollLayout_->addWidget(titleLabel_);

    // Subtitle
    subtitleLabel_ = new QLabel(
        "Explore all 15 Material Design 3 typography styles with various color variants and features.",
        this);
    QFont subtitleFont("Segoe UI", 11);
    subtitleLabel_->setFont(subtitleFont);
    subtitleLabel_->setWordWrap(true);
    scrollLayout_->addWidget(subtitleLabel_);

    scrollLayout_->addSpacing(16);
}

void LabelExampleWindow::createDisplaySection() {
    QGroupBox* groupBox = createGroupBox("Display Styles (展示样式)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Display Large - 57sp
    Label* displayLarge =
        new Label("Display Large", TypographyStyle::DisplayLarge, this);
    displayLarge->setColorVariant(LabelColorVariant::OnSurface);
    layout->addWidget(displayLarge);

    // Display Medium - 45sp
    Label* displayMedium =
        new Label("Display Medium", TypographyStyle::DisplayMedium, this);
    displayMedium->setColorVariant(LabelColorVariant::OnSurface);
    layout->addWidget(displayMedium);

    // Display Small - 36sp
    Label* displaySmall =
        new Label("Display Small", TypographyStyle::DisplaySmall, this);
    displaySmall->setColorVariant(LabelColorVariant::OnSurface);
    layout->addWidget(displaySmall);

    scrollLayout_->addWidget(groupBox);
}

void LabelExampleWindow::createHeadlineSection() {
    QGroupBox* groupBox = createGroupBox("Headline Styles (标题样式)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Headline Large - 32sp
    Label* headlineLarge =
        new Label("Headline Large", TypographyStyle::HeadlineLarge, this);
    headlineLarge->setColorVariant(LabelColorVariant::OnSurface);
    layout->addWidget(headlineLarge);

    // Headline Medium - 28sp
    Label* headlineMedium =
        new Label("Headline Medium", TypographyStyle::HeadlineMedium, this);
    headlineMedium->setColorVariant(LabelColorVariant::OnSurface);
    layout->addWidget(headlineMedium);

    // Headline Small - 24sp
    Label* headlineSmall =
        new Label("Headline Small", TypographyStyle::HeadlineSmall, this);
    headlineSmall->setColorVariant(LabelColorVariant::OnSurface);
    layout->addWidget(headlineSmall);

    scrollLayout_->addWidget(groupBox);
}

void LabelExampleWindow::createTitleSection() {
    QGroupBox* groupBox = createGroupBox("Title Styles (标题样式)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Title Large - 22sp
    Label* titleLarge = new Label("Title Large", TypographyStyle::TitleLarge, this);
    titleLarge->setColorVariant(LabelColorVariant::OnSurface);
    layout->addWidget(titleLarge);

    // Title Medium - 16sp
    Label* titleMedium =
        new Label("Title Medium", TypographyStyle::TitleMedium, this);
    titleMedium->setColorVariant(LabelColorVariant::OnSurface);
    layout->addWidget(titleMedium);

    // Title Small - 14sp
    Label* titleSmall = new Label("Title Small", TypographyStyle::TitleSmall, this);
    titleSmall->setColorVariant(LabelColorVariant::OnSurface);
    layout->addWidget(titleSmall);

    scrollLayout_->addWidget(groupBox);
}

void LabelExampleWindow::createBodySection() {
    QGroupBox* groupBox = createGroupBox("Body Styles (正文样式)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Body Large - 16sp
    Label* bodyLarge = new Label("Body Large - Main content text", TypographyStyle::BodyLarge, this);
    bodyLarge->setColorVariant(LabelColorVariant::OnSurface);
    layout->addWidget(bodyLarge);

    // Body Medium - 14sp
    Label* bodyMedium =
        new Label("Body Medium - Default content text", TypographyStyle::BodyMedium, this);
    bodyMedium->setColorVariant(LabelColorVariant::OnSurface);
    layout->addWidget(bodyMedium);

    // Body Small - 12sp
    Label* bodySmall = new Label("Body Small - Secondary content text", TypographyStyle::BodySmall,
                                 this);
    bodySmall->setColorVariant(LabelColorVariant::OnSurfaceVariant);
    layout->addWidget(bodySmall);

    scrollLayout_->addWidget(groupBox);
}

void LabelExampleWindow::createLabelSection() {
    QGroupBox* groupBox = createGroupBox("Label Styles (标签样式)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Label Large - 14sp
    Label* labelLarge = new Label("Label Large - Button text", TypographyStyle::LabelLarge, this);
    labelLarge->setColorVariant(LabelColorVariant::Primary);
    layout->addWidget(labelLarge);

    // Label Medium - 12sp
    Label* labelMedium =
        new Label("Label Medium - Caption text", TypographyStyle::LabelMedium, this);
    labelMedium->setColorVariant(LabelColorVariant::OnSurfaceVariant);
    layout->addWidget(labelMedium);

    // Label Small - 11sp
    Label* labelSmall = new Label("Label Small - Helper text", TypographyStyle::LabelSmall, this);
    labelSmall->setColorVariant(LabelColorVariant::OnSurfaceVariant);
    layout->addWidget(labelSmall);

    scrollLayout_->addWidget(groupBox);
}

void LabelExampleWindow::createColorVariantsSection() {
    QGroupBox* groupBox = createGroupBox("Color Variants (颜色变体)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Create a container widget for the grid
    QWidget* gridContainer = new QWidget(groupBox);
    QGridLayout* gridLayout = new QGridLayout(gridContainer);
    gridLayout->setSpacing(12);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    // Row 1: OnSurface variants
    gridLayout->addWidget(new QLabel("OnSurface:", gridContainer), 0, 0);
    Label* onSurface =
        new Label("Default Text", TypographyStyle::BodyMedium, gridContainer);
    onSurface->setColorVariant(LabelColorVariant::OnSurface);
    gridLayout->addWidget(onSurface, 0, 1);

    gridLayout->addWidget(new QLabel("OnSurfaceVariant:", gridContainer), 0, 2);
    Label* onSurfaceVariant =
        new Label("Secondary Text", TypographyStyle::BodyMedium, gridContainer);
    onSurfaceVariant->setColorVariant(LabelColorVariant::OnSurfaceVariant);
    gridLayout->addWidget(onSurfaceVariant, 0, 3);

    // Row 2: Primary variants
    gridLayout->addWidget(new QLabel("Primary:", gridContainer), 1, 0);
    Label* primary = new Label("Primary Color", TypographyStyle::BodyMedium, gridContainer);
    primary->setColorVariant(LabelColorVariant::Primary);
    gridLayout->addWidget(primary, 1, 1);

    gridLayout->addWidget(new QLabel("OnPrimary:", gridContainer), 1, 2);
    Label* onPrimary = new Label("On Primary", TypographyStyle::BodyMedium, gridContainer);
    onPrimary->setColorVariant(LabelColorVariant::OnPrimary);
    gridLayout->addWidget(onPrimary, 1, 3);

    // Row 3: Secondary variants
    gridLayout->addWidget(new QLabel("Secondary:", gridContainer), 2, 0);
    Label* secondary = new Label("Secondary", TypographyStyle::BodyMedium, gridContainer);
    secondary->setColorVariant(LabelColorVariant::Secondary);
    gridLayout->addWidget(secondary, 2, 1);

    gridLayout->addWidget(new QLabel("OnSecondary:", gridContainer), 2, 2);
    Label* onSecondary = new Label("On Secondary", TypographyStyle::BodyMedium, gridContainer);
    onSecondary->setColorVariant(LabelColorVariant::OnSecondary);
    gridLayout->addWidget(onSecondary, 2, 3);

    // Row 4: Error variants
    gridLayout->addWidget(new QLabel("Error:", gridContainer), 3, 0);
    Label* error = new Label("Error Message", TypographyStyle::BodyMedium, gridContainer);
    error->setColorVariant(LabelColorVariant::Error);
    gridLayout->addWidget(error, 3, 1);

    gridLayout->addWidget(new QLabel("OnError:", gridContainer), 3, 2);
    Label* onError = new Label("On Error", TypographyStyle::BodyMedium, gridContainer);
    onError->setColorVariant(LabelColorVariant::OnError);
    gridLayout->addWidget(onError, 3, 3);

    layout->addWidget(gridContainer);
    scrollLayout_->addWidget(groupBox);
}

void LabelExampleWindow::createStatesSection() {
    QGroupBox* groupBox = createGroupBox("States (状态)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Enabled state
    QHBoxLayout* enabledLayout = new QHBoxLayout();
    QLabel* enabledLabel = new QLabel("Enabled:", this);
    enabledLabel->setMinimumWidth(100);
    Label* enabledLabel2 = new Label("This label is enabled", TypographyStyle::BodyMedium, this);
    enabledLabel2->setColorVariant(LabelColorVariant::OnSurface);
    enabledLayout->addWidget(enabledLabel);
    enabledLayout->addWidget(enabledLabel2);
    enabledLayout->addStretch();
    layout->addLayout(enabledLayout);

    // Disabled state
    QHBoxLayout* disabledLayout = new QHBoxLayout();
    QLabel* disabledLabel = new QLabel("Disabled:", this);
    disabledLabel->setMinimumWidth(100);
    Label* disabledLabel2 = new Label("This label is disabled", TypographyStyle::BodyMedium, this);
    disabledLabel2->setColorVariant(LabelColorVariant::OnSurface);
    disabledLabel2->setEnabled(false);
    disabledLayout->addWidget(disabledLabel);
    disabledLayout->addWidget(disabledLabel2);
    disabledLayout->addStretch();
    layout->addLayout(disabledLayout);

    scrollLayout_->addWidget(groupBox);
}

void LabelExampleWindow::createFeaturesSection() {
    QGroupBox* groupBox = createGroupBox("Features (特性)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Auto-hiding demo
    QHBoxLayout* autoHideLayout = new QHBoxLayout();
    QLabel* autoHideLabel = new QLabel("Auto-Hiding:", this);
    autoHideLabel->setMinimumWidth(120);
    Label* autoHideLabel2 = new Label("Visible (has text)", TypographyStyle::BodyMedium, this);
    autoHideLabel2->setAutoHiding(true);
    autoHideLayout->addWidget(autoHideLabel);
    autoHideLayout->addWidget(autoHideLabel2);
    autoHideLayout->addStretch();
    layout->addLayout(autoHideLayout);

    // Empty label with auto-hiding
    QHBoxLayout* emptyLayout = new QHBoxLayout();
    QLabel* emptyLabel = new QLabel("Empty (Hidden):", this);
    emptyLabel->setMinimumWidth(120);
    Label* emptyAutoHideLabel = new Label("", TypographyStyle::BodyMedium, this);
    emptyAutoHideLabel->setAutoHiding(true);
    emptyAutoHideLabel->setColorVariant(LabelColorVariant::OnSurfaceVariant);
    emptyLayout->addWidget(emptyLabel);
    emptyLayout->addWidget(emptyAutoHideLabel);
    emptyLayout->addStretch();
    layout->addLayout(emptyLayout);

    // Word wrap demo
    QHBoxLayout* wrapLayout = new QHBoxLayout();
    QLabel* wrapLabel = new QLabel("Word Wrap:", this);
    wrapLabel->setMinimumWidth(120);
    Label* wrapLabel2 =
        new Label("This is a long text that will wrap when the label width is constrained",
                  TypographyStyle::BodyMedium, this);
    wrapLabel2->setColorVariant(LabelColorVariant::OnSurface);
    wrapLabel2->setWordWrap(true);
    wrapLabel2->setMaximumWidth(300);
    wrapLayout->addWidget(wrapLabel);
    wrapLayout->addWidget(wrapLabel2);
    wrapLayout->addStretch();
    layout->addLayout(wrapLayout);

    scrollLayout_->addWidget(groupBox);

    // Add stretch at the end
    scrollLayout_->addStretch();
}

QGroupBox* LabelExampleWindow::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(8);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
