/**
 * @file SeparatorDemo.h
 * @brief Material Design 3 Separator Demo - Widget Component
 */

#pragma once

#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
class QGroupBox;

namespace cf::ui::example {

class SeparatorDemo : public QWidget {
    Q_OBJECT

  public:
    explicit SeparatorDemo(QWidget* parent = nullptr);
    ~SeparatorDemo() override = default;

    QString title() const { return "Separator"; }
    QString description() const { return "Material Design 3 Separator Component"; }

  private:
    void setupUI();
    void createHorizontalSeparatorSection();
    void createVerticalSeparatorSection();
    void createSeparatorModesSection();
    void createInListSection();
    QGroupBox* createGroupBox(const QString& title);

    QWidget* scrollContent_;
    QVBoxLayout* layout_;
};

} // namespace cf::ui::example
