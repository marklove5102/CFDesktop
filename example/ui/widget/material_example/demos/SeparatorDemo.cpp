/**
 * @file SeparatorDemo.cpp
 * @brief Material Design 3 Separator Demo - Implementation
 */

#include "SeparatorDemo.h"

#include "ui/widget/material/widget/separator/separator.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <qboxlayout.h>

using namespace cf::ui::widget::material;

namespace cf::ui::example {

SeparatorDemo::SeparatorDemo(QWidget* parent) : QWidget(parent) {

    setupUI();
    createHorizontalSeparatorSection();
    createVerticalSeparatorSection();
    createSeparatorModesSection();
    createInListSection();
}

void SeparatorDemo::setupUI() {
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

void SeparatorDemo::createHorizontalSeparatorSection() {
    QGroupBox* groupBox = createGroupBox("Horizontal Separator (水平分隔符)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Basic horizontal separator
    QLabel* label1 = new QLabel("Content above the separator", groupBox);
    layout->addWidget(label1);

    Separator* separator1 = new Separator(Qt::Horizontal, groupBox);
    layout->addWidget(separator1);

    QLabel* label2 = new QLabel("Content below the separator", groupBox);
    layout->addWidget(label2);

    scrollContent_->layout()->addWidget(groupBox);
}

void SeparatorDemo::createVerticalSeparatorSection() {
    QGroupBox* groupBox = new QGroupBox("Vertical Separator (垂直分隔符)", this);
    QHBoxLayout* layout = new QHBoxLayout(groupBox);

    layout->setSpacing(16);
    layout->setContentsMargins(16, 24, 16, 16);

    // Left content
    QLabel* leftLabel = new QLabel("Left Item", groupBox);
    layout->addWidget(leftLabel);

    // Vertical separator
    Separator* separator = new Separator(Qt::Vertical, groupBox);
    separator->setMinimumHeight(60);
    layout->addWidget(separator);

    // Right content
    QLabel* rightLabel = new QLabel("Right Item", groupBox);
    layout->addWidget(rightLabel);

    layout->addStretch();

    scrollContent_->layout()->addWidget(groupBox);
}

void SeparatorDemo::createSeparatorModesSection() {
    QGroupBox* groupBox = createGroupBox("Separator Modes (分隔符模式)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // FullBleed mode
    QLabel* fullBleedLabel = new QLabel("FullBleed Mode (full width/height):", groupBox);
    layout->addWidget(fullBleedLabel);

    Separator* fullBleedSep = new Separator(Qt::Horizontal, groupBox);
    fullBleedSep->setMode(SeparatorMode::FullBleed);
    layout->addWidget(fullBleedSep);

    // Inset mode
    QLabel* insetLabel = new QLabel("Inset Mode (16dp margin on both sides):", groupBox);
    layout->addWidget(insetLabel);

    Separator* insetSep = new Separator(Qt::Horizontal, groupBox);
    insetSep->setMode(SeparatorMode::Inset);
    layout->addWidget(insetSep);

    // MiddleInset mode
    QLabel* middleInsetLabel =
        new QLabel("MiddleInset Mode (16dp margin on leading side):", groupBox);
    layout->addWidget(middleInsetLabel);

    Separator* middleInsetSep = new Separator(Qt::Horizontal, groupBox);
    middleInsetSep->setMode(SeparatorMode::MiddleInset);
    layout->addWidget(middleInsetSep);

    scrollContent_->layout()->addWidget(groupBox);
}

void SeparatorDemo::createInListSection() {
    QGroupBox* groupBox = createGroupBox("Separators in List (列表中的分隔符)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Simulate a list with separators
    QLabel* item1 = new QLabel("List Item 1", groupBox);
    layout->addWidget(item1);

    Separator* sep1 = new Separator(Qt::Horizontal, groupBox);
    sep1->setMode(SeparatorMode::Inset);
    layout->addWidget(sep1);

    QLabel* item2 = new QLabel("List Item 2", groupBox);
    layout->addWidget(item2);

    Separator* sep2 = new Separator(Qt::Horizontal, groupBox);
    sep2->setMode(SeparatorMode::Inset);
    layout->addWidget(sep2);

    QLabel* item3 = new QLabel("List Item 3", groupBox);
    layout->addWidget(item3);

    Separator* sep3 = new Separator(Qt::Horizontal, groupBox);
    sep3->setMode(SeparatorMode::Inset);
    layout->addWidget(sep3);

    QLabel* item4 = new QLabel("List Item 4", groupBox);
    layout->addWidget(item4);

    scrollContent_->layout()->addWidget(groupBox);
}

QGroupBox* SeparatorDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
