/**
 * @file TabViewDemo.h
 * @brief Material Design 3 TabView Demo - Widget Component
 */

#pragma once

#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
class QGroupBox;

namespace cf::ui::example {

class TabViewDemo : public QWidget {
    Q_OBJECT

  public:
    explicit TabViewDemo(QWidget* parent = nullptr);
    ~TabViewDemo() override = default;

    QString title() const { return "TabView"; }
    QString description() const { return "Material Design 3 TabView Component"; }

  private:
    void setupUI();
    void createBasicTabsSection();
    void createTabsWithIconsSection();
    void createCloseableTabsSection();
    void createTabPositionSection();
    void createDisabledTabsSection();
    QGroupBox* createGroupBox(const QString& title);

    QWidget* scrollContent_;
    QVBoxLayout* layout_;
};

} // namespace cf::ui::example
