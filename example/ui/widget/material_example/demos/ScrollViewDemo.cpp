/**
 * @file ScrollViewDemo.cpp
 * @brief Material Design 3 ScrollView Demo - Implementation
 */

#include "ScrollViewDemo.h"

#include "ui/widget/material/widget/scrollview/scrollview.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

using namespace cf::ui::widget::material;

namespace cf::ui::example {

ScrollViewDemo::ScrollViewDemo(QWidget* parent) : QWidget(parent) {
    setupUI();
    createVerticalScrollSection();
    createHorizontalScrollSection();
    createBothDirectionsSection();
    createScrollBarPoliciesSection();
    createContentExamplesSection();

    // Add stretch at the end of scroll content to push sections to top
    static_cast<QVBoxLayout*>(scrollContent_->layout())->addStretch();
}

void ScrollViewDemo::setupUI() {
    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(0);

    // Scroll area for demo content
    ScrollView* scrollArea = new ScrollView(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollContent_ = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent_);
    scrollLayout->setSpacing(24);
    scrollLayout->setContentsMargins(24, 24, 24, 24);
    scrollLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    scrollArea->setWidget(scrollContent_);
    layout_->addWidget(scrollArea, 1);
}

void ScrollViewDemo::createVerticalScrollSection() {
    QGroupBox* groupBox = new QGroupBox("Vertical Scroll (垂直滚动)", this);
    QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);
    groupLayout->setSpacing(12);
    groupLayout->setContentsMargins(16, 24, 16, 16);

    // Create a ScrollView with vertical scrolling
    ScrollView* scrollView = new ScrollView(this);
    scrollView->setWidgetResizable(true);
    scrollView->setFixedHeight(200);
    scrollView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Create content that requires vertical scrolling
    QWidget* content = new QWidget();
    QVBoxLayout* contentLayout = new QVBoxLayout(content);
    contentLayout->setSpacing(8);

    for (int i = 1; i <= 20; ++i) {
        QLabel* label = new QLabel(QString("Vertical Content Item %1").arg(i), this);
        label->setStyleSheet("padding: 8px; background-color: #f0f0f0; border-radius: 4px;");
        contentLayout->addWidget(label);
    }
    contentLayout->addStretch();

    scrollView->setWidget(content);
    groupLayout->addWidget(scrollView);

    scrollContent_->layout()->addWidget(groupBox);
}

void ScrollViewDemo::createHorizontalScrollSection() {
    QGroupBox* groupBox = new QGroupBox("Horizontal Scroll (水平滚动)", this);
    QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);
    groupLayout->setSpacing(12);
    groupLayout->setContentsMargins(16, 24, 16, 16);

    // Create a ScrollView with horizontal scrolling
    ScrollView* scrollView = new ScrollView(this);
    scrollView->setWidgetResizable(true);
    scrollView->setFixedHeight(120);
    scrollView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Create content that requires horizontal scrolling
    QWidget* content = new QWidget();
    content->setFixedHeight(80);
    QHBoxLayout* contentLayout = new QHBoxLayout(content);
    contentLayout->setSpacing(8);

    for (int i = 1; i <= 15; ++i) {
        QLabel* label = new QLabel(QString("H%1").arg(i), this);
        label->setAlignment(Qt::AlignCenter);
        label->setFixedSize(60, 60);
        label->setStyleSheet("background-color: #e0e0e0; border-radius: 8px; font-weight: bold;");
        contentLayout->addWidget(label);
    }

    scrollView->setWidget(content);
    groupLayout->addWidget(scrollView);

    scrollContent_->layout()->addWidget(groupBox);
}

void ScrollViewDemo::createBothDirectionsSection() {
    QGroupBox* groupBox = new QGroupBox("Both Directions (双向滚动)", this);
    QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);
    groupLayout->setSpacing(12);
    groupLayout->setContentsMargins(16, 24, 16, 16);

    // Create a ScrollView with both horizontal and vertical scrolling
    ScrollView* scrollView = new ScrollView(this);
    scrollView->setWidgetResizable(true);
    scrollView->setFixedSize(400, 200);
    scrollView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Create a grid of content
    QWidget* content = new QWidget();
    QGridLayout* contentLayout = new QGridLayout(content);
    contentLayout->setSpacing(8);
    contentLayout->setContentsMargins(8, 8, 8, 8);

    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 8; ++col) {
            QLabel* label = new QLabel(QString("%1,%2").arg(row + 1).arg(col + 1), this);
            label->setAlignment(Qt::AlignCenter);
            label->setFixedSize(80, 50);
            label->setStyleSheet("background-color: #d0d0d0; border-radius: 4px;");
            contentLayout->addWidget(label, row, col);
        }
    }

    scrollView->setWidget(content);
    groupLayout->addWidget(scrollView);

    scrollContent_->layout()->addWidget(groupBox);
}

void ScrollViewDemo::createScrollBarPoliciesSection() {
    QGroupBox* groupBox = new QGroupBox("Scroll Bar Policies (滚动条策略)", this);
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(16);
    gridLayout->setContentsMargins(16, 24, 16, 16);

    // Always On
    QLabel* alwaysOnLabel = new QLabel("ScrollBarAlwaysOn:", this);
    ScrollView* alwaysOnScroll = new ScrollView(this);
    alwaysOnScroll->setWidgetResizable(true);
    alwaysOnScroll->setFixedSize(180, 100);
    alwaysOnScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    alwaysOnScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    alwaysOnScroll->setWidget(createLargeContentWidget(Qt::Vertical));

    // As Needed
    QLabel* asNeededLabel = new QLabel("ScrollBarAsNeeded:", this);
    ScrollView* asNeededScroll = new ScrollView(this);
    asNeededScroll->setWidgetResizable(true);
    asNeededScroll->setFixedSize(180, 100);
    asNeededScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    asNeededScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    asNeededScroll->setWidget(createLargeContentWidget(Qt::Vertical));

    // Always Off
    QLabel* alwaysOffLabel = new QLabel("ScrollBarAlwaysOff:", this);
    ScrollView* alwaysOffScroll = new ScrollView(this);
    alwaysOffScroll->setWidgetResizable(true);
    alwaysOffScroll->setFixedSize(180, 100);
    alwaysOffScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    alwaysOffScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    alwaysOffScroll->setWidget(createLargeContentWidget(Qt::Vertical));

    // Fade effect enabled
    QLabel* fadeEnabledLabel = new QLabel("Fade Effect:", this);
    ScrollView* fadeScroll = new ScrollView(this);
    fadeScroll->setWidgetResizable(true);
    fadeScroll->setFixedSize(180, 100);
    fadeScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    fadeScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    fadeScroll->setScrollbarFadeEnabled(true);
    fadeScroll->setWidget(createLargeContentWidget(Qt::Vertical));

    // Layout arrangement
    gridLayout->addWidget(alwaysOnLabel, 0, 0);
    gridLayout->addWidget(alwaysOnScroll, 1, 0);
    gridLayout->addWidget(asNeededLabel, 0, 1);
    gridLayout->addWidget(asNeededScroll, 1, 1);
    gridLayout->addWidget(alwaysOffLabel, 0, 2);
    gridLayout->addWidget(alwaysOffScroll, 1, 2);
    gridLayout->addWidget(fadeEnabledLabel, 0, 3);
    gridLayout->addWidget(fadeScroll, 1, 3);

    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 1);
    gridLayout->setColumnStretch(3, 1);

    scrollContent_->layout()->addWidget(groupBox);
}

void ScrollViewDemo::createContentExamplesSection() {
    QGroupBox* groupBox = new QGroupBox("Content Examples (内容示例)", this);
    QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);
    groupLayout->setSpacing(12);
    groupLayout->setContentsMargins(16, 24, 16, 16);

    // Text content example
    QHBoxLayout* textLayout = new QHBoxLayout();
    QLabel* textLabel = new QLabel("Text Content:", this);
    textLabel->setMinimumWidth(120);

    ScrollView* textScroll = new ScrollView(this);
    textScroll->setWidgetResizable(true);
    textScroll->setFixedSize(400, 120);

    QWidget* textContent = new QWidget();
    QVBoxLayout* textContentLayout = new QVBoxLayout(textContent);
    QLabel* longTextLabel = new QLabel(
        "This is an example of long text content in a ScrollView. "
        "ScrollViews are perfect for displaying content that exceeds the available screen space. "
        "Material Design 3 ScrollViews feature smooth scrolling, custom-styled scrollbars, "
        "and optional fade effects. The scrollbar automatically appears when content overflows "
        "and can be configured with different policies. You can scroll vertically, horizontally, "
        "or in both directions depending on your content requirements.",
        this);
    longTextLabel->setWordWrap(true);
    longTextLabel->setStyleSheet("padding: 12px;");
    textContentLayout->addWidget(longTextLabel);
    textScroll->setWidget(textContent);

    textLayout->addWidget(textLabel);
    textLayout->addWidget(textScroll);
    textLayout->addStretch();
    groupLayout->addLayout(textLayout);

    // Widget grid example
    QHBoxLayout* widgetLayout = new QHBoxLayout();
    QLabel* widgetLabel = new QLabel("Widget Grid:", this);
    widgetLabel->setMinimumWidth(120);

    ScrollView* widgetScroll = new ScrollView(this);
    widgetScroll->setWidgetResizable(true);
    widgetScroll->setFixedSize(400, 120);

    QWidget* widgetContent = new QWidget();
    QGridLayout* widgetContentLayout = new QGridLayout(widgetContent);
    widgetContentLayout->setSpacing(8);

    for (int i = 0; i < 12; ++i) {
        QPushButton* btn = new QPushButton(QString("Button %1").arg(i + 1), this);
        btn->setFixedSize(100, 36);
        int row = i / 4;
        int col = i % 4;
        widgetContentLayout->addWidget(btn, row, col);
    }

    widgetScroll->setWidget(widgetContent);

    widgetLayout->addWidget(widgetLabel);
    widgetLayout->addWidget(widgetScroll);
    widgetLayout->addStretch();
    groupLayout->addLayout(widgetLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

QWidget* ScrollViewDemo::createLargeContentWidget(Qt::Orientation orientation) {
    QWidget* content = new QWidget();

    if (orientation == Qt::Vertical) {
        QVBoxLayout* layout = new QVBoxLayout(content);
        layout->setSpacing(4);
        for (int i = 1; i <= 10; ++i) {
            QLabel* label = new QLabel(QString("Item %1").arg(i), this);
            label->setStyleSheet("padding: 6px; background-color: #e8e8e8; border-radius: 3px;");
            layout->addWidget(label);
        }
        layout->addStretch();
    } else {
        content->setFixedHeight(50);
        QHBoxLayout* layout = new QHBoxLayout(content);
        layout->setSpacing(4);
        for (int i = 1; i <= 10; ++i) {
            QLabel* label = new QLabel(QString("%1").arg(i), this);
            label->setFixedSize(40, 40);
            label->setAlignment(Qt::AlignCenter);
            label->setStyleSheet("background-color: #e8e8e8; border-radius: 3px;");
            layout->addWidget(label);
        }
    }

    return content;
}

QGroupBox* ScrollViewDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
