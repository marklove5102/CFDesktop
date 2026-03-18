/**
 * @file TableViewDemo.h
 * @brief Material Design 3 TableView Demo - Widget Component
 */

#pragma once

#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
class QGroupBox;
class QStandardItemModel;

namespace cf::ui::example {

class TableViewDemo : public QWidget {
    Q_OBJECT

  public:
    explicit TableViewDemo(QWidget* parent = nullptr);
    ~TableViewDemo() override = default;

    QString title() const { return "TableView"; }
    QString description() const { return "Material Design 3 TableView Component"; }

  private:
    void setupUI();
    void createBasicTableSection();
    void createSortingSection();
    void createColumnHeadersSection();
    void createSelectionModesSection();
    void createEditableCellsSection();
    QGroupBox* createGroupBox(const QString& title);

    void onSortIndicatorChanged(int logicalIndex, Qt::SortOrder order);
    void onSelectionChanged();

    QWidget* scrollContent_;
    QVBoxLayout* layout_;
    QStandardItemModel* basicTableModel_;
    QStandardItemModel* sortableModel_;
    QStandardItemModel* headerModel_;
    QStandardItemModel* singleSelectModel_;
    QStandardItemModel* multiSelectModel_;
    QStandardItemModel* editableModel_;
};

} // namespace cf::ui::example
