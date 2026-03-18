/**
 * @file TreeViewDemo.cpp
 * @brief Material Design 3 TreeView Demo - Implementation
 */

#include "TreeViewDemo.h"

#include "ui/widget/material/widget/treeview/treeview.h"

#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
#include <QVBoxLayout>

using namespace cf::ui::widget::material;

namespace cf::ui::example {

TreeViewDemo::TreeViewDemo(QWidget* parent) : QWidget(parent) {

    setupUI();
    createBasicTreeSection();
    createNestedItemsSection();
    createExpandCollapseSection();
    createSelectionModesSection();
    createIconsInTreeSection();
}

void TreeViewDemo::setupUI() {
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

void TreeViewDemo::createBasicTreeSection() {
    QGroupBox* groupBox = new QGroupBox("Basic Tree (基本树)", this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);

    basicTreeModel_ = new QStandardItemModel(this);
    basicTreeModel_->setHorizontalHeaderLabels(QStringList() << "Name" << "Type");

    // Root items
    QStandardItem* root1 = new QStandardItem("Documents");
    QStandardItem* root1Type = new QStandardItem("Folder");
    root1->appendRow(QList<QStandardItem*>()
                     << new QStandardItem("Report.pdf") << new QStandardItem("PDF"));
    root1->appendRow(QList<QStandardItem*>()
                     << new QStandardItem("Presentation.pptx") << new QStandardItem("PowerPoint"));
    basicTreeModel_->appendRow(QList<QStandardItem*>() << root1 << root1Type);

    QStandardItem* root2 = new QStandardItem("Pictures");
    QStandardItem* root2Type = new QStandardItem("Folder");
    root2->appendRow(QList<QStandardItem*>()
                     << new QStandardItem("Photo1.jpg") << new QStandardItem("JPEG"));
    root2->appendRow(QList<QStandardItem*>()
                     << new QStandardItem("Photo2.png") << new QStandardItem("PNG"));
    basicTreeModel_->appendRow(QList<QStandardItem*>() << root2 << root2Type);

    QStandardItem* root3 = new QStandardItem("Music");
    QStandardItem* root3Type = new QStandardItem("Folder");
    basicTreeModel_->appendRow(QList<QStandardItem*>() << root3 << root3Type);

    TreeView* treeView = new TreeView(this);
    treeView->setModel(basicTreeModel_);
    treeView->setMinimumHeight(200);

    layout->addWidget(treeView);
    scrollContent_->layout()->addWidget(groupBox);
}

void TreeViewDemo::createNestedItemsSection() {
    QGroupBox* groupBox = createGroupBox("Nested Items (嵌套项目)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    nestedTreeModel_ = new QStandardItemModel(this);
    nestedTreeModel_->setHorizontalHeaderLabels(QStringList() << "Project Structure");

    // Create deeply nested structure
    QStandardItem* projectRoot = new QStandardItem("MyProject");
    nestedTreeModel_->appendRow(projectRoot);

    QStandardItem* srcFolder = new QStandardItem("src");
    projectRoot->appendRow(srcFolder);

    QStandardItem* coreFolder = new QStandardItem("core");
    srcFolder->appendRow(coreFolder);
    coreFolder->appendRow(new QStandardItem("engine.cpp"));
    coreFolder->appendRow(new QStandardItem("renderer.cpp"));

    QStandardItem* uiFolder = new QStandardItem("ui");
    srcFolder->appendRow(uiFolder);
    uiFolder->appendRow(new QStandardItem("main_window.cpp"));
    uiFolder->appendRow(new QStandardItem("widgets.cpp"));

    QStandardItem* componentsFolder = new QStandardItem("components");
    uiFolder->appendRow(componentsFolder);
    componentsFolder->appendRow(new QStandardItem("button.cpp"));
    componentsFolder->appendRow(new QStandardItem("treeview.cpp"));

    QStandardItem* resourcesFolder = new QStandardItem("resources");
    projectRoot->appendRow(resourcesFolder);
    resourcesFolder->appendRow(new QStandardItem("icons/"));
    resourcesFolder->appendRow(new QStandardItem("styles/"));

    QStandardItem* buildFolder = new QStandardItem("build");
    projectRoot->appendRow(buildFolder);
    buildFolder->appendRow(new QStandardItem("debug/"));
    buildFolder->appendRow(new QStandardItem("release/"));

    TreeView* treeView = new TreeView(this);
    treeView->setModel(nestedTreeModel_);
    treeView->setMinimumHeight(250);
    treeView->expandAll();

    layout->addWidget(treeView);
    scrollContent_->layout()->addWidget(groupBox);
}

void TreeViewDemo::createExpandCollapseSection() {
    QGroupBox* groupBox = createGroupBox("Expand/Collapse (展开/折叠)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    expandTreeModel_ = new QStandardItemModel(this);
    expandTreeModel_->setHorizontalHeaderLabels(QStringList() << "Categories");

    // Create expandable categories
    QStandardItem* electronics = new QStandardItem("Electronics (电子产品)");
    electronics->appendRow(new QStandardItem("Laptops (笔记本电脑)"));
    electronics->appendRow(new QStandardItem("Smartphones (智能手机)"));
    electronics->appendRow(new QStandardItem("Tablets (平板电脑)"));
    expandTreeModel_->appendRow(electronics);

    QStandardItem* clothing = new QStandardItem("Clothing (服装)");
    clothing->appendRow(new QStandardItem("Shirts (衬衫)"));
    clothing->appendRow(new QStandardItem("Pants (裤子)"));
    clothing->appendRow(new QStandardItem("Shoes (鞋子)"));
    expandTreeModel_->appendRow(clothing);

    QStandardItem* books = new QStandardItem("Books (书籍)");
    books->appendRow(new QStandardItem("Fiction (小说)"));
    books->appendRow(new QStandardItem("Non-Fiction (非小说)"));
    books->appendRow(new QStandardItem("Technical (技术)"));
    expandTreeModel_->appendRow(books);

    TreeView* treeView = new TreeView(this);
    treeView->setModel(expandTreeModel_);
    treeView->setMinimumHeight(200);

    // Control buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* expandAllBtn = new QPushButton("Expand All (全部展开)", this);
    QPushButton* collapseAllBtn = new QPushButton("Collapse All (全部折叠)", this);

    connect(expandAllBtn, &QPushButton::clicked, this, &TreeViewDemo::onExpandAll);
    connect(collapseAllBtn, &QPushButton::clicked, this, &TreeViewDemo::onCollapseAll);

    buttonLayout->addWidget(expandAllBtn);
    buttonLayout->addWidget(collapseAllBtn);
    buttonLayout->addStretch();

    layout->addWidget(treeView);
    layout->addLayout(buttonLayout);
    scrollContent_->layout()->addWidget(groupBox);
}

void TreeViewDemo::createSelectionModesSection() {
    QGroupBox* groupBox = createGroupBox("Selection Modes (选择模式)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    selectionTreeModel_ = new QStandardItemModel(this);
    selectionTreeModel_->setHorizontalHeaderLabels(QStringList() << "Items");

    // Add sample items
    QStandardItem* item1 = new QStandardItem("Item 1");
    item1->appendRow(new QStandardItem("Subitem 1.1"));
    item1->appendRow(new QStandardItem("Subitem 1.2"));
    selectionTreeModel_->appendRow(item1);

    QStandardItem* item2 = new QStandardItem("Item 2");
    item2->appendRow(new QStandardItem("Subitem 2.1"));
    selectionTreeModel_->appendRow(item2);

    QStandardItem* item3 = new QStandardItem("Item 3");
    selectionTreeModel_->appendRow(item3);

    QGridLayout* gridLayout = new QGridLayout();

    // Single Selection
    TreeView* singleSelectTree = new TreeView(this);
    singleSelectTree->setModel(selectionTreeModel_);
    singleSelectTree->setSelectionMode(QAbstractItemView::SingleSelection);
    singleSelectTree->setMinimumHeight(150);

    QLabel* singleLabel = new QLabel("Single Selection:\n(单选)", this);
    singleLabel->setAlignment(Qt::AlignCenter);

    // Multi Selection
    TreeView* multiSelectTree = new TreeView(this);
    multiSelectTree->setModel(selectionTreeModel_);
    multiSelectTree->setSelectionMode(QAbstractItemView::MultiSelection);
    multiSelectTree->setMinimumHeight(150);

    QLabel* multiLabel = new QLabel("Multi Selection:\n(多选)", this);
    multiLabel->setAlignment(Qt::AlignCenter);

    // Extended Selection
    TreeView* extendedSelectTree = new TreeView(this);
    extendedSelectTree->setModel(selectionTreeModel_);
    extendedSelectTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    extendedSelectTree->setMinimumHeight(150);

    QLabel* extendedLabel = new QLabel("Extended Selection:\n(扩展选择)", this);
    extendedLabel->setAlignment(Qt::AlignCenter);

    gridLayout->addWidget(singleLabel, 0, 0);
    gridLayout->addWidget(singleSelectTree, 1, 0);
    gridLayout->addWidget(multiLabel, 0, 1);
    gridLayout->addWidget(multiSelectTree, 1, 1);
    gridLayout->addWidget(extendedLabel, 0, 2);
    gridLayout->addWidget(extendedSelectTree, 1, 2);

    layout->addLayout(gridLayout);
    scrollContent_->layout()->addWidget(groupBox);
}

void TreeViewDemo::createIconsInTreeSection() {
    QGroupBox* groupBox = createGroupBox("Icons in Tree (带图标的树)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    iconTreeModel_ = new QStandardItemModel(this);
    iconTreeModel_->setHorizontalHeaderLabels(QStringList() << "File Explorer");

    // Get standard icons
    QIcon folderIcon = QApplication::style()->standardIcon(QStyle::SP_DirIcon);
    QIcon fileIcon = QApplication::style()->standardIcon(QStyle::SP_FileIcon);
    QIcon computerIcon = QApplication::style()->standardIcon(QStyle::SP_ComputerIcon);
    QIcon driveIcon = QApplication::style()->standardIcon(QStyle::SP_DriveHDIcon);

    // Create items with icons
    QStandardItem* myComputer = new QStandardItem(computerIcon, "My Computer");
    iconTreeModel_->appendRow(myComputer);

    QStandardItem* driveC = new QStandardItem(driveIcon, "Local Disk (C:)");
    myComputer->appendRow(driveC);

    QStandardItem* programFiles = new QStandardItem(folderIcon, "Program Files");
    driveC->appendRow(programFiles);
    programFiles->appendRow(new QStandardItem(fileIcon, "Application.exe"));
    programFiles->appendRow(new QStandardItem(fileIcon, "Tool.exe"));

    QStandardItem* users = new QStandardItem(folderIcon, "Users");
    driveC->appendRow(users);
    users->appendRow(new QStandardItem(folderIcon, "Documents"));
    users->appendRow(new QStandardItem(folderIcon, "Pictures"));

    QStandardItem* driveD = new QStandardItem(driveIcon, "Data Disk (D:)");
    myComputer->appendRow(driveD);

    QStandardItem* projects = new QStandardItem(folderIcon, "Projects");
    driveD->appendRow(projects);
    projects->appendRow(new QStandardItem(fileIcon, "project1.cpp"));
    projects->appendRow(new QStandardItem(fileIcon, "project2.cpp"));
    projects->appendRow(new QStandardItem(fileIcon, "README.md"));

    TreeView* treeView = new TreeView(this);
    treeView->setModel(iconTreeModel_);
    treeView->setMinimumHeight(250);
    treeView->expandAll();

    layout->addWidget(treeView);
    scrollContent_->layout()->addWidget(groupBox);
}

void TreeViewDemo::onSelectionChanged() {
    // Handle selection changes if needed
}

void TreeViewDemo::onExpandAll() {
    QObject* senderObj = sender();
    if (senderObj) {
        QWidget* widget = qobject_cast<QWidget*>(senderObj->parent());
        if (widget) {
            TreeView* treeView = widget->findChild<TreeView*>();
            if (treeView) {
                treeView->expandAll();
            }
        }
    }
}

void TreeViewDemo::onCollapseAll() {
    QObject* senderObj = sender();
    if (senderObj) {
        QWidget* widget = qobject_cast<QWidget*>(senderObj->parent());
        if (widget) {
            TreeView* treeView = widget->findChild<TreeView*>();
            if (treeView) {
                treeView->collapseAll();
            }
        }
    }
}

void TreeViewDemo::onAddItem() {
    // Placeholder for adding items
}

void TreeViewDemo::onRemoveItem() {
    // Placeholder for removing items
}

QGroupBox* TreeViewDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
