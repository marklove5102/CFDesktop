/**
 * @file TextFieldExampleWindow.h
 * @brief Material Design 3 TextField Example - Main Window
 *
 * A visual gallery to display all Material Design 3 text field variants
 * with various states and configurations.
 *
 * @author CFDesktop Team
 * @date 2026-03-01
 * @version 0.1
 */

#pragma once

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

namespace cf::ui::example {

/**
 * @brief Main window for the Material TextField Example.
 *
 * Displays both Material Design 3 text field variants:
 * - Filled TextField (填充文本框)
 * - Outlined TextField (轮廓文本框)
 *
 * Also demonstrates:
 * - Text fields with floating labels
 * - Text fields with prefix/suffix icons
 * - Password mode
 * - Character counter
 * - Helper text and error text
 * - Disabled state
 */
class TextFieldExampleWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit TextFieldExampleWindow(QWidget* parent = nullptr);
    ~TextFieldExampleWindow() override;

  private:
    void setupUI();
    void createHeader();
    void createTextFieldVariantsSection();
    void createTextFieldStatesSection();
    void createTextFieldWithIconsSection();
    void createTextFieldWithErrorSection();
    void createTextFieldAdvancedSection();

    QGroupBox* createGroupBox(const QString& title);
    QWidget* createScrollContent();

  private slots:
    void onTextChanged(const QString& text);
    void onValidateClicked();
    void onClearClicked();

  private:
    // UI components
    QWidget* centralWidget_;
    QVBoxLayout* mainLayout_;
    QScrollArea* scrollArea_;
    QWidget* scrollContent_;
    QVBoxLayout* scrollLayout_;

    // Header
    QLabel* titleLabel_;
    QLabel* subtitleLabel_;
};

} // namespace cf::ui::example
