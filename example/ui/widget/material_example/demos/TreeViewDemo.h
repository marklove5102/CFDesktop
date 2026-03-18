/**
 * @file TreeViewDemo.h
 * @brief Material Design 3 TreeView Demo - Widget Component
 */

#pragma once

#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
class QGroupBox;
class QStandardItemModel;

namespace cf::ui::example {

class TreeViewDemo : public QWidget {
    Q_OBJECT

  public:
    explicit TreeViewDemo(QWidget* parent = nullptr);
    ~TreeViewDemo() override = default;

    QString title() const { return "TreeView"; }
    QString description() const { return "Material Design 3 TreeView Component"; }

  private:
    void setupUI();
    void createBasicTreeSection();
    void createNestedItemsSection();
    void createExpandCollapseSection();
    void createSelectionModesSection();
    void createIconsInTreeSection();
    QGroupBox* createGroupBox(const QString& title);

    void onSelectionChanged();
    void onExpandAll();
    void onCollapseAll();
    void onAddItem();
    void onRemoveItem();

    QWidget* scrollContent_;
    QVBoxLayout* layout_;
    QStandardItemModel* basicTreeModel_;
    QStandardItemModel* nestedTreeModel_;
    QStandardItemModel* expandTreeModel_;
    QStandardItemModel* selectionTreeModel_;
    QStandardItemModel* iconTreeModel_;
};

} // namespace cf::ui::example
