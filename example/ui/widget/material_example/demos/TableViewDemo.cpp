/**
 * @file TableViewDemo.cpp
 * @brief Material Design 3 TableView Demo - Implementation
 */

#include "TableViewDemo.h"

#include "ui/widget/material/widget/tableview/tableview.h"

#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QStandardItemModel>
#include <QStyle>

using namespace cf::ui::widget::material;

namespace cf::ui::example {

TableViewDemo::TableViewDemo(QWidget* parent) : QWidget(parent) {

    setupUI();
    createBasicTableSection();
    createSortingSection();
    createColumnHeadersSection();
    createSelectionModesSection();
    createEditableCellsSection();
}

void TableViewDemo::setupUI() {
    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(24, 24, 24, 24);
    layout_->setSpacing(16);

    // Scroll area for content
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    scrollContent_ = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent_);
    scrollLayout->setSpacing(24);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidget(scrollContent_);
    layout_->addWidget(scrollArea, 1);
}

void TableViewDemo::createBasicTableSection() {
    QGroupBox* groupBox = createGroupBox("Basic Table (基础表格)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Basic table view
    TableView* tableView = new TableView(this);
    tableView->setMinimumHeight(250);
    tableView->setShowHeader(true);
    tableView->setGridStyle(TableGridStyle::Both);
    tableView->setAlternatingRowColors(true);
    tableView->setRowHeight(TableRowHeight::Standard);

    basicTableModel_ = new QStandardItemModel(this);
    basicTableModel_->setColumnCount(4);
    basicTableModel_->setHorizontalHeaderLabels({"Name", "Age", "Department", "Status"});

    // Add sample data
    QList<QStandardItem*> row1;
    row1 << new QStandardItem("Alice Johnson") << new QStandardItem("28")
         << new QStandardItem("Engineering") << new QStandardItem("Active");
    basicTableModel_->appendRow(row1);

    QList<QStandardItem*> row2;
    row2 << new QStandardItem("Bob Smith") << new QStandardItem("35")
         << new QStandardItem("Marketing") << new QStandardItem("Active");
    basicTableModel_->appendRow(row2);

    QList<QStandardItem*> row3;
    row3 << new QStandardItem("Carol Williams") << new QStandardItem("42")
         << new QStandardItem("Finance") << new QStandardItem("On Leave");
    basicTableModel_->appendRow(row3);

    QList<QStandardItem*> row4;
    row4 << new QStandardItem("David Brown") << new QStandardItem("31")
         << new QStandardItem("Engineering") << new QStandardItem("Active");
    basicTableModel_->appendRow(row4);

    QList<QStandardItem*> row5;
    row5 << new QStandardItem("Eve Davis") << new QStandardItem("26") << new QStandardItem("Sales")
         << new QStandardItem("Active");
    basicTableModel_->appendRow(row5);

    tableView->setModel(basicTableModel_);

    // Adjust column widths
    tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    layout->addWidget(tableView);
    scrollContent_->layout()->addWidget(groupBox);
}

void TableViewDemo::createSortingSection() {
    QGroupBox* groupBox = createGroupBox("Sortable Columns (可排序列)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Sortable table view
    TableView* tableView = new TableView(this);
    tableView->setMinimumHeight(250);
    tableView->setShowHeader(true);
    tableView->setGridStyle(TableGridStyle::Horizontal);
    tableView->setAlternatingRowColors(true);

    sortableModel_ = new QStandardItemModel(this);
    sortableModel_->setColumnCount(4);
    sortableModel_->setHorizontalHeaderLabels({"Product", "Price", "Stock", "Rating"});

    sortableModel_->setSortRole(Qt::DisplayRole);

    // Add sample data with sortable values
    QList<QStandardItem*> row1;
    row1 << new QStandardItem("Laptop") << new QStandardItem("999") << new QStandardItem("45")
         << new QStandardItem("4.5");
    sortableModel_->appendRow(row1);

    QList<QStandardItem*> row2;
    row2 << new QStandardItem("Mouse") << new QStandardItem("25") << new QStandardItem("120")
         << new QStandardItem("4.2");
    sortableModel_->appendRow(row2);

    QList<QStandardItem*> row3;
    row3 << new QStandardItem("Keyboard") << new QStandardItem("75") << new QStandardItem("85")
         << new QStandardItem("4.7");
    sortableModel_->appendRow(row3);

    QList<QStandardItem*> row4;
    row4 << new QStandardItem("Monitor") << new QStandardItem("350") << new QStandardItem("30")
         << new QStandardItem("4.3");
    sortableModel_->appendRow(row4);

    QList<QStandardItem*> row5;
    row5 << new QStandardItem("Headphones") << new QStandardItem("150") << new QStandardItem("60")
         << new QStandardItem("4.6");
    sortableModel_->appendRow(row5);

    tableView->setModel(sortableModel_);
    tableView->setSortingEnabled(true);

    // Enable sortable indicator
    QHeaderView* header = tableView->horizontalHeader();
    header->setSectionsClickable(true);
    header->setSortIndicatorShown(true);

    QLabel* infoLabel =
        new QLabel("Click column headers to sort by that column (按列标题排序)", this);
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);

    layout->addWidget(tableView);
    scrollContent_->layout()->addWidget(groupBox);
}

void TableViewDemo::createColumnHeadersSection() {
    QGroupBox* groupBox = new QGroupBox("Column Headers (列标题配置)", this);
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(16);
    gridLayout->setContentsMargins(16, 24, 16, 16);

    // Table with custom header styling
    QLabel* customHeaderLabel = new QLabel("Custom Header Style:", this);
    TableView* customHeaderTable = new TableView(this);
    customHeaderTable->setMinimumHeight(200);
    customHeaderTable->setShowHeader(true);
    customHeaderTable->setGridStyle(TableGridStyle::Both);

    headerModel_ = new QStandardItemModel(this);
    headerModel_->setColumnCount(3);
    headerModel_->setHorizontalHeaderLabels({"ID", "Task Name", "Priority"});

    QList<QStandardItem*> row1;
    row1 << new QStandardItem("001") << new QStandardItem("Design Review")
         << new QStandardItem("High");
    headerModel_->appendRow(row1);

    QList<QStandardItem*> row2;
    row2 << new QStandardItem("002") << new QStandardItem("Code Review")
         << new QStandardItem("Medium");
    headerModel_->appendRow(row2);

    QList<QStandardItem*> row3;
    row3 << new QStandardItem("003") << new QStandardItem("Testing") << new QStandardItem("Low");
    headerModel_->appendRow(row3);

    customHeaderTable->setModel(headerModel_);

    gridLayout->addWidget(customHeaderLabel, 0, 0);
    gridLayout->addWidget(customHeaderTable, 1, 0);

    // Table without header
    QLabel* noHeaderLabel = new QLabel("No Header (无标题):", this);
    TableView* noHeaderTable = new TableView(this);
    noHeaderTable->setMinimumHeight(200);
    noHeaderTable->setShowHeader(false);
    noHeaderTable->setGridStyle(TableGridStyle::Horizontal);

    QStandardItemModel* noHeaderModel = new QStandardItemModel(this);
    noHeaderModel->setColumnCount(2);

    QList<QStandardItem*> nhRow1;
    nhRow1 << new QStandardItem("Apple") << new QStandardItem("$1.50");
    noHeaderModel->appendRow(nhRow1);

    QList<QStandardItem*> nhRow2;
    nhRow2 << new QStandardItem("Banana") << new QStandardItem("$0.75");
    noHeaderModel->appendRow(nhRow2);

    QList<QStandardItem*> nhRow3;
    nhRow3 << new QStandardItem("Orange") << new QStandardItem("$1.25");
    noHeaderModel->appendRow(nhRow3);

    noHeaderTable->setModel(noHeaderModel);

    gridLayout->addWidget(noHeaderLabel, 0, 1);
    gridLayout->addWidget(noHeaderTable, 1, 1);

    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);

    scrollContent_->layout()->addWidget(groupBox);
}

void TableViewDemo::createSelectionModesSection() {
    QGroupBox* groupBox = new QGroupBox("Selection Modes (选择模式)", this);
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(16);
    gridLayout->setContentsMargins(16, 24, 16, 16);

    // Single selection
    QLabel* singleLabel = new QLabel("Single Selection (单选):", this);
    TableView* singleSelectTable = new TableView(this);
    singleSelectTable->setMinimumHeight(200);
    singleSelectTable->setSelectionMode(QAbstractItemView::SingleSelection);
    singleSelectTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    singleSelectTable->setShowHeader(true);
    singleSelectTable->setGridStyle(TableGridStyle::Both);

    singleSelectModel_ = new QStandardItemModel(this);
    singleSelectModel_->setColumnCount(3);
    singleSelectModel_->setHorizontalHeaderLabels({"First Name", "Last Name", "Email"});

    QList<QStandardItem*> ssRow1;
    ssRow1 << new QStandardItem("John") << new QStandardItem("Doe")
           << new QStandardItem("john@example.com");
    singleSelectModel_->appendRow(ssRow1);

    QList<QStandardItem*> ssRow2;
    ssRow2 << new QStandardItem("Jane") << new QStandardItem("Smith")
           << new QStandardItem("jane@example.com");
    singleSelectModel_->appendRow(ssRow2);

    QList<QStandardItem*> ssRow3;
    ssRow3 << new QStandardItem("Bob") << new QStandardItem("Jones")
           << new QStandardItem("bob@example.com");
    singleSelectModel_->appendRow(ssRow3);

    singleSelectTable->setModel(singleSelectModel_);

    gridLayout->addWidget(singleLabel, 0, 0);
    gridLayout->addWidget(singleSelectTable, 1, 0);

    // Multi selection
    QLabel* multiLabel = new QLabel("Multi Selection (多选):", this);
    TableView* multiSelectTable = new TableView(this);
    multiSelectTable->setMinimumHeight(200);
    multiSelectTable->setSelectionMode(QAbstractItemView::MultiSelection);
    multiSelectTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    multiSelectTable->setShowHeader(true);
    multiSelectTable->setGridStyle(TableGridStyle::Both);

    multiSelectModel_ = new QStandardItemModel(this);
    multiSelectModel_->setColumnCount(3);
    multiSelectModel_->setHorizontalHeaderLabels({"Item", "Quantity", "Location"});

    QList<QStandardItem*> msRow1;
    msRow1 << new QStandardItem("Widget A") << new QStandardItem("10")
           << new QStandardItem("Warehouse 1");
    multiSelectModel_->appendRow(msRow1);

    QList<QStandardItem*> msRow2;
    msRow2 << new QStandardItem("Widget B") << new QStandardItem("25")
           << new QStandardItem("Warehouse 2");
    multiSelectModel_->appendRow(msRow2);

    QList<QStandardItem*> msRow3;
    msRow3 << new QStandardItem("Widget C") << new QStandardItem("15")
           << new QStandardItem("Warehouse 1");
    multiSelectModel_->appendRow(msRow3);

    QList<QStandardItem*> msRow4;
    msRow4 << new QStandardItem("Widget D") << new QStandardItem("30")
           << new QStandardItem("Warehouse 3");
    multiSelectModel_->appendRow(msRow4);

    multiSelectTable->setModel(multiSelectModel_);

    gridLayout->addWidget(multiLabel, 2, 0);
    gridLayout->addWidget(multiSelectTable, 3, 0);

    // Extended selection (Ctrl+Click, Shift+Click)
    QLabel* extendedLabel = new QLabel("Extended Selection (扩展选择):", this);
    TableView* extendedSelectTable = new TableView(this);
    extendedSelectTable->setMinimumHeight(200);
    extendedSelectTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    extendedSelectTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    extendedSelectTable->setShowHeader(true);
    extendedSelectTable->setGridStyle(TableGridStyle::Both);

    QStandardItemModel* extendedModel = new QStandardItemModel(this);
    extendedModel->setColumnCount(2);
    extendedModel->setHorizontalHeaderLabels({"Project", "Status"});

    QList<QStandardItem*> esRow1;
    esRow1 << new QStandardItem("Alpha") << new QStandardItem("Complete");
    extendedModel->appendRow(esRow1);

    QList<QStandardItem*> esRow2;
    esRow2 << new QStandardItem("Beta") << new QStandardItem("In Progress");
    extendedModel->appendRow(esRow2);

    QList<QStandardItem*> esRow3;
    esRow3 << new QStandardItem("Gamma") << new QStandardItem("Planning");
    extendedModel->appendRow(esRow3);

    extendedSelectTable->setModel(extendedModel);

    gridLayout->addWidget(extendedLabel, 0, 1);
    gridLayout->addWidget(extendedSelectTable, 1, 1);

    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);

    scrollContent_->layout()->addWidget(groupBox);
}

void TableViewDemo::createEditableCellsSection() {
    QGroupBox* groupBox = createGroupBox("Editable Cells (可编辑单元格)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Editable table view
    TableView* tableView = new TableView(this);
    tableView->setMinimumHeight(250);
    tableView->setShowHeader(true);
    tableView->setGridStyle(TableGridStyle::Both);
    tableView->setAlternatingRowColors(true);

    editableModel_ = new QStandardItemModel(this);
    editableModel_->setColumnCount(4);
    editableModel_->setHorizontalHeaderLabels({"First Name", "Last Name", "Role", "Notes"});

    // Add sample data
    QList<QStandardItem*> row1;
    row1 << new QStandardItem("Sarah") << new QStandardItem("Connor")
         << new QStandardItem("Developer") << new QStandardItem("Frontend specialist");
    editableModel_->appendRow(row1);

    QList<QStandardItem*> row2;
    row2 << new QStandardItem("Mike") << new QStandardItem("Ross") << new QStandardItem("Designer")
         << new QStandardItem("UI/UX expert");
    editableModel_->appendRow(row2);

    QList<QStandardItem*> row3;
    row3 << new QStandardItem("Lisa") << new QStandardItem("Wong") << new QStandardItem("Manager")
         << new QStandardItem("Team lead");
    editableModel_->appendRow(row3);

    QList<QStandardItem*> row4;
    row4 << new QStandardItem("Tom") << new QStandardItem("Anderson")
         << new QStandardItem("Developer") << new QStandardItem("Backend specialist");
    editableModel_->appendRow(row4);

    // Make all items editable
    for (int row = 0; row < editableModel_->rowCount(); ++row) {
        for (int col = 0; col < editableModel_->columnCount(); ++col) {
            QStandardItem* item = editableModel_->item(row, col);
            if (item) {
                item->setFlags(item->flags() | Qt::ItemIsEditable);
            }
        }
    }

    tableView->setModel(editableModel_);
    tableView->setEditTriggers(QAbstractItemView::DoubleClicked |
                               QAbstractItemView::EditKeyPressed);

    QLabel* infoLabel = new QLabel("Double-click any cell or press F2 to edit. Changes apply to "
                                   "the model. (双击单元格或按F2编辑)",
                                   this);
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);

    layout->addWidget(tableView);
    scrollContent_->layout()->addWidget(groupBox);
}

void TableViewDemo::onSortIndicatorChanged(int logicalIndex, Qt::SortOrder order) {
    Q_UNUSED(logicalIndex);
    Q_UNUSED(order);
    // Handle sort indicator changes if needed
}

void TableViewDemo::onSelectionChanged() {
    // Handle selection changes if needed
}

QGroupBox* TableViewDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
