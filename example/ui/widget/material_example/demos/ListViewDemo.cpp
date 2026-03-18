/**
 * @file ListViewDemo.cpp
 * @brief Material Design 3 ListView Demo - Implementation
 */

#include "ListViewDemo.h"

#include "ui/widget/material/widget/listview/listview.h"

#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QStyle>

using namespace cf::ui::widget::material;
using ItemHeight = cf::ui::widget::material::ListView::ItemHeight;

namespace cf::ui::example {

ListViewDemo::ListViewDemo(QWidget* parent) : QWidget(parent) {

    setupUI();
    createBasicListSection();
    createSingleSelectionSection();
    createMultiSelectionSection();
    createWithIconsSection();
    createDifferentItemHeightsSection();
}

void ListViewDemo::setupUI() {
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

void ListViewDemo::createBasicListSection() {
    QGroupBox* groupBox = createGroupBox("Basic List (基础列表)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Basic list view
    ListView* listView = new ListView(this);
    listView->setMinimumHeight(200);
    listView->setShowSeparator(true);

    QStringListModel* model = new QStringListModel(this);
    model->setStringList({"Item 1", "Item 2", "Item 3", "Item 4", "Item 5"});
    listView->setModel(model);

    layout->addWidget(listView);
    scrollContent_->layout()->addWidget(groupBox);
}

void ListViewDemo::createSingleSelectionSection() {
    QGroupBox* groupBox = createGroupBox("Single Selection (单选列表)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Single selection list view
    ListView* listView = new ListView(this);
    listView->setMinimumHeight(200);
    listView->setShowSeparator(true);
    listView->setSelectionMode(QAbstractItemView::SingleSelection);

    QStringListModel* model = new QStringListModel(this);
    model->setStringList({"Option A", "Option B", "Option C", "Option D", "Option E"});
    listView->setModel(model);

    layout->addWidget(listView);

    // Info label
    QLabel* infoLabel =
        new QLabel("Click an item to select it (only one item can be selected)", this);
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);

    scrollContent_->layout()->addWidget(groupBox);
}

void ListViewDemo::createMultiSelectionSection() {
    QGroupBox* groupBox = createGroupBox("Multi Selection (多选列表)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Multi selection list view
    ListView* listView = new ListView(this);
    listView->setMinimumHeight(200);
    listView->setShowSeparator(true);
    listView->setSelectionMode(QAbstractItemView::MultiSelection);

    QStringListModel* model = new QStringListModel(this);
    model->setStringList(
        {"Selection 1", "Selection 2", "Selection 3", "Selection 4", "Selection 5", "Selection 6"});
    listView->setModel(model);

    layout->addWidget(listView);

    // Info label
    QLabel* infoLabel =
        new QLabel("Click multiple items to select them (Ctrl+Click for individual items)", this);
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);

    scrollContent_->layout()->addWidget(groupBox);
}

void ListViewDemo::createWithIconsSection() {
    QGroupBox* groupBox = createGroupBox("List with Icons (带图标的列表)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // List view with icons
    ListView* listView = new ListView(this);
    listView->setMinimumHeight(200);
    listView->setShowSeparator(true);

    QStandardItemModel* model = new QStandardItemModel(this);

    // Add items with icons
    QList<QStandardItem*> items;
    items << new QStandardItem(QApplication::style()->standardIcon(QStyle::SP_DirIcon),
                               "Documents");
    items << new QStandardItem(QApplication::style()->standardIcon(QStyle::SP_FileIcon), "Files");
    items << new QStandardItem(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon),
                               "Computer");
    items << new QStandardItem(QApplication::style()->standardIcon(QStyle::SP_DriveHDIcon),
                               "Hard Drive");
    items << new QStandardItem(QApplication::style()->standardIcon(QStyle::SP_DriveCDIcon),
                               "CD-ROM");

    model->appendColumn(items);
    listView->setModel(model);

    layout->addWidget(listView);
    scrollContent_->layout()->addWidget(groupBox);
}

void ListViewDemo::createDifferentItemHeightsSection() {
    QGroupBox* groupBox = new QGroupBox("Different Item Heights (不同项目高度)", this);
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(16);
    gridLayout->setContentsMargins(16, 24, 16, 16);

    // Single Line items (56dp)
    QLabel* singleLineLabel = new QLabel("Single Line (56dp):", this);
    ListView* singleLineList = new ListView(this);
    singleLineList->setMinimumHeight(180);
    singleLineList->setShowSeparator(true);
    singleLineList->setItemHeight(ItemHeight::SingleLine);

    QStringListModel* singleLineModel = new QStringListModel(this);
    singleLineModel->setStringList({"Item One", "Item Two", "Item Three"});
    singleLineList->setModel(singleLineModel);

    gridLayout->addWidget(singleLineLabel, 0, 0);
    gridLayout->addWidget(singleLineList, 1, 0);

    // Two Line items (72dp)
    QLabel* twoLineLabel = new QLabel("Two Line (72dp):", this);
    ListView* twoLineList = new ListView(this);
    twoLineList->setMinimumHeight(180);
    twoLineList->setShowSeparator(true);
    twoLineList->setItemHeight(ItemHeight::TwoLine);

    QStringListModel* twoLineModel = new QStringListModel(this);
    twoLineModel->setStringList({"Primary text\nSecondary text description",
                                 "Another primary\nMore secondary info",
                                 "Third primary\nFinal secondary line"});
    twoLineList->setModel(twoLineModel);

    gridLayout->addWidget(twoLineLabel, 0, 1);
    gridLayout->addWidget(twoLineList, 1, 1);

    // Three Line items (88dp)
    QLabel* threeLineLabel = new QLabel("Three Line (88dp):", this);
    ListView* threeLineList = new ListView(this);
    threeLineList->setMinimumHeight(180);
    threeLineList->setShowSeparator(true);
    threeLineList->setItemHeight(ItemHeight::ThreeLine);

    QStringListModel* threeLineModel = new QStringListModel(this);
    threeLineModel->setStringList({"Primary heading\nSecondary description\nThird line of text",
                                   "Another heading\nMore description here\nExtra detail line",
                                   "Last heading\nFinal description\nConcluding line"});
    threeLineList->setModel(threeLineModel);

    gridLayout->addWidget(threeLineLabel, 2, 0);
    gridLayout->addWidget(threeLineList, 3, 0);

    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);

    scrollContent_->layout()->addWidget(groupBox);
}

QGroupBox* ListViewDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
