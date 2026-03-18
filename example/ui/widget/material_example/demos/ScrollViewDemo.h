/**
 * @file ScrollViewDemo.h
 * @brief Material Design 3 ScrollView Demo - Widget Component
 */

#pragma once

#include <QVBoxLayout>
#include <QWidget>
class QGroupBox;

namespace cf::ui::example {

class ScrollViewDemo : public QWidget {
    Q_OBJECT

  public:
    explicit ScrollViewDemo(QWidget* parent = nullptr);
    ~ScrollViewDemo() override = default;

    QString title() const { return "ScrollView"; }
    QString description() const { return "Material Design 3 ScrollView Component"; }

  private:
    void setupUI();
    void createVerticalScrollSection();
    void createHorizontalScrollSection();
    void createBothDirectionsSection();
    void createScrollBarPoliciesSection();
    void createContentExamplesSection();
    QGroupBox* createGroupBox(const QString& title);
    QWidget* createLargeContentWidget(Qt::Orientation orientation);

    QWidget* scrollContent_;
    QVBoxLayout* layout_;
};

} // namespace cf::ui::example
