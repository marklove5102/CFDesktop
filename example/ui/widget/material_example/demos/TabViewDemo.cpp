/**
 * @file TabViewDemo.cpp
 * @brief Material Design 3 TabView Demo - Implementation
 */

#include "TabViewDemo.h"

#include "ui/widget/material/widget/tabview/tabview.h"

#include <QApplication>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QStyle>
#include <QTextEdit>
#include <QVBoxLayout>

using namespace cf::ui::widget::material;

namespace cf::ui::example {

TabViewDemo::TabViewDemo(QWidget* parent) : QWidget(parent) {

    setupUI();
    createBasicTabsSection();
    createTabsWithIconsSection();
    createCloseableTabsSection();
    createTabPositionSection();
    createDisabledTabsSection();
}

void TabViewDemo::setupUI() {
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

void TabViewDemo::createBasicTabsSection() {
    QGroupBox* groupBox = new QGroupBox("Basic Tabs (基本选项卡)", this);
    QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);
    groupLayout->setSpacing(12);
    groupLayout->setContentsMargins(16, 24, 16, 16);

    // Basic TabView with text labels
    TabView* tabView = new TabView(this);

    // Add tabs with basic content
    QWidget* homeTab = new QWidget();
    QVBoxLayout* homeLayout = new QVBoxLayout(homeTab);
    homeLayout->addWidget(new QLabel("Home Tab Content\n\nThis is the home page content."));
    homeLayout->addStretch();
    tabView->addTab(homeTab, "Home");

    QWidget* profileTab = new QWidget();
    QVBoxLayout* profileLayout = new QVBoxLayout(profileTab);
    profileLayout->addWidget(
        new QLabel("Profile Tab Content\n\nUser profile information goes here."));
    profileLayout->addStretch();
    tabView->addTab(profileTab, "Profile");

    QWidget* settingsTab = new QWidget();
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsTab);
    settingsLayout->addWidget(new QLabel("Settings Tab Content\n\nConfigure your settings here."));
    settingsLayout->addStretch();
    tabView->addTab(settingsTab, "Settings");

    tabView->setMinimumHeight(200);
    groupLayout->addWidget(tabView);

    scrollContent_->layout()->addWidget(groupBox);
}

void TabViewDemo::createTabsWithIconsSection() {
    QGroupBox* groupBox = new QGroupBox("Tabs with Icons (带图标的选项卡)", this);
    QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);
    groupLayout->setSpacing(12);
    groupLayout->setContentsMargins(16, 24, 16, 16);

    // TabView with icons
    TabView* tabView = new TabView(this);

    // Add tabs with icons
    QWidget* filesTab = new QWidget();
    QVBoxLayout* filesLayout = new QVBoxLayout(filesTab);
    filesLayout->addWidget(new QLabel("Files Tab\n\nBrowse and manage your files."));
    filesLayout->addStretch();
    tabView->addTab(filesTab, QApplication::style()->standardIcon(QStyle::SP_FileIcon), "Files");

    QWidget* editTab = new QWidget();
    QVBoxLayout* editLayout = new QVBoxLayout(editTab);
    editLayout->addWidget(new QLabel("Edit Tab\n\nEdit your content here."));
    editLayout->addStretch();
    tabView->addTab(editTab, QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView),
                    "Edit");

    QWidget* viewTab = new QWidget();
    QVBoxLayout* viewLayout = new QVBoxLayout(viewTab);
    viewLayout->addWidget(new QLabel("View Tab\n\nAdjust view settings."));
    viewLayout->addStretch();
    tabView->addTab(viewTab, QApplication::style()->standardIcon(QStyle::SP_BrowserReload), "View");

    tabView->setMinimumHeight(200);
    groupLayout->addWidget(tabView);

    scrollContent_->layout()->addWidget(groupBox);
}

void TabViewDemo::createCloseableTabsSection() {
    QGroupBox* groupBox = new QGroupBox("Closeable Tabs (可关闭的选项卡)", this);
    QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);
    groupLayout->setSpacing(12);
    groupLayout->setContentsMargins(16, 24, 16, 16);

    // TabView with closeable tabs
    TabView* tabView = new TabView(this);

    // Helper lambda to create closeable tab content
    auto createCloseableTab = [](const QString& name) -> QWidget* {
        QWidget* tab = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(tab);
        layout->addWidget(new QLabel(QString("Tab: %1\n\nThis tab can be closed.").arg(name)));
        layout->addStretch();
        return tab;
    };

    // Add closeable tabs
    tabView->addTab(createCloseableTab("Document 1"), "Document 1");
    tabView->addTab(createCloseableTab("Document 2"), "Document 2");
    tabView->addTab(createCloseableTab("Document 3"), "Document 3");

    // Set tabs closeable using our custom close button
    for (int i = 0; i < tabView->count(); ++i) {
        tabView->setTabCloseable(i, true);
    }

    tabView->setMinimumHeight(200);
    groupLayout->addWidget(tabView);

    // Hint label
    QLabel* hintLabel = new QLabel("Note: Click the '×' button on each tab to close it.", this);
    hintLabel->setStyleSheet("color: gray; font-size: 11px;");
    groupLayout->addWidget(hintLabel);

    scrollContent_->layout()->addWidget(groupBox);
}

void TabViewDemo::createTabPositionSection() {
    QGroupBox* groupBox = new QGroupBox("Tab Position (选项卡位置)", this);
    QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);
    groupLayout->setSpacing(12);
    groupLayout->setContentsMargins(16, 24, 16, 16);

    // Top position (default)
    QHBoxLayout* topLayout = new QHBoxLayout();
    QLabel* topLabel = new QLabel("Top:", this);
    topLabel->setMinimumWidth(80);
    TabView* topTabView = new TabView(this);
    topTabView->addTab(new QLabel("Top Position Content"), "Tab 1");
    topTabView->addTab(new QLabel("Another Tab"), "Tab 2");
    topTabView->setTabPosition(QTabWidget::North);
    topTabView->setMinimumHeight(120);
    topLayout->addWidget(topLabel);
    topLayout->addWidget(topTabView);
    groupLayout->addLayout(topLayout);

    // Bottom position
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    QLabel* bottomLabel = new QLabel("Bottom:", this);
    bottomLabel->setMinimumWidth(80);
    TabView* bottomTabView = new TabView(this);
    bottomTabView->addTab(new QLabel("Bottom Position Content"), "Tab 1");
    bottomTabView->addTab(new QLabel("Another Tab"), "Tab 2");
    bottomTabView->setTabPosition(QTabWidget::South);
    bottomTabView->setMinimumHeight(120);
    bottomLayout->addWidget(bottomLabel);
    bottomLayout->addWidget(bottomTabView);
    groupLayout->addLayout(bottomLayout);

    // Left position
    QHBoxLayout* leftLayout = new QHBoxLayout();
    QLabel* leftLabel = new QLabel("Left:", this);
    leftLabel->setMinimumWidth(80);
    TabView* leftTabView = new TabView(this);
    leftTabView->addTab(new QLabel("Left\nPosition\nContent"), "Tab 1");
    leftTabView->addTab(new QLabel("Another\nTab"), "Tab 2");
    leftTabView->setTabPosition(QTabWidget::West);
    leftTabView->setMinimumHeight(120);
    leftLayout->addWidget(leftLabel);
    leftLayout->addWidget(leftTabView);
    groupLayout->addLayout(leftLayout);

    // Right position
    QHBoxLayout* rightLayout = new QHBoxLayout();
    QLabel* rightLabel = new QLabel("Right:", this);
    rightLabel->setMinimumWidth(80);
    TabView* rightTabView = new TabView(this);
    rightTabView->addTab(new QLabel("Right\nPosition\nContent"), "Tab 1");
    rightTabView->addTab(new QLabel("Another\nTab"), "Tab 2");
    rightTabView->setTabPosition(QTabWidget::East);
    rightTabView->setMinimumHeight(120);
    rightLayout->addWidget(rightLabel);
    rightLayout->addWidget(rightTabView);
    groupLayout->addLayout(rightLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void TabViewDemo::createDisabledTabsSection() {
    QGroupBox* groupBox = new QGroupBox("Disabled Tabs (禁用的选项卡)", this);
    QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);
    groupLayout->setSpacing(12);
    groupLayout->setContentsMargins(16, 24, 16, 16);

    // TabView with disabled tabs
    TabView* tabView = new TabView(this);

    // Add tabs
    QWidget* activeTab = new QWidget();
    QVBoxLayout* activeLayout = new QVBoxLayout(activeTab);
    activeLayout->addWidget(new QLabel("Active Tab\n\nThis tab is enabled."));
    activeLayout->addStretch();
    tabView->addTab(activeTab, "Active");

    QWidget* disabledTab = new QWidget();
    QVBoxLayout* disabledLayout = new QVBoxLayout(disabledTab);
    disabledLayout->addWidget(new QLabel("Disabled Tab\n\nThis tab is disabled."));
    disabledLayout->addStretch();
    int disabledIndex = tabView->addTab(disabledTab, "Disabled");

    QWidget* anotherActiveTab = new QWidget();
    QVBoxLayout* anotherLayout = new QVBoxLayout(anotherActiveTab);
    anotherLayout->addWidget(new QLabel("Another Active\n\nThis tab is also enabled."));
    anotherLayout->addStretch();
    tabView->addTab(anotherActiveTab, "Active 2");

    // Disable the second tab
    tabView->setTabEnabled(disabledIndex, false);

    tabView->setMinimumHeight(200);
    groupLayout->addWidget(tabView);

    // Hint label
    QLabel* hintLabel = new QLabel("Note: The 'Disabled' tab cannot be selected.", this);
    hintLabel->setStyleSheet("color: gray; font-size: 11px;");
    groupLayout->addWidget(hintLabel);

    scrollContent_->layout()->addWidget(groupBox);
}

QGroupBox* TabViewDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
