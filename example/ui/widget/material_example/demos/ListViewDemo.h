/**
 * @file ListViewDemo.h
 * @brief Material Design 3 ListView Demo - Widget Component
 */

#pragma once

#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
class QGroupBox;
class QStringListModel;
class QStandardItemModel;

namespace cf::ui::example {

class ListViewDemo : public QWidget {
    Q_OBJECT

  public:
    explicit ListViewDemo(QWidget* parent = nullptr);
    ~ListViewDemo() override = default;

    QString title() const { return "ListView"; }
    QString description() const { return "Material Design 3 ListView Component"; }

  private:
    void setupUI();
    void createBasicListSection();
    void createSingleSelectionSection();
    void createMultiSelectionSection();
    void createWithIconsSection();
    void createDifferentItemHeightsSection();
    QGroupBox* createGroupBox(const QString& title);

    QWidget* scrollContent_;
    QVBoxLayout* layout_;
};

} // namespace cf::ui::example
