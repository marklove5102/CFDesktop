/**
 * @file TextAreaExampleWindow.h
 * @brief Material Design 3 TextArea Example - Main Window
 *
 * A visual gallery to display all Material Design 3 text area variants
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
 * @brief Main window for the Material TextArea Example.
 *
 * Displays Material Design 3 text area variants:
 * - Filled TextArea (填充文本域)
 * - Outlined TextArea (轮廓文本域)
 *
 * Also demonstrates:
 * - Floating labels
 * - Character counter
 * - Helper text and error states
 * - Min/max lines configuration
 * - Disabled states
 */
class TextAreaExampleWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit TextAreaExampleWindow(QWidget* parent = nullptr);
    ~TextAreaExampleWindow() override;

  private:
    void setupUI();
    void createHeader();
    void createTextAreaVariantsSection();
    void createTextAreaStatesSection();
    void createCharacterCounterSection();
    void createMinMaxLinesSection();
    void createInteractionDemoSection();

    QGroupBox* createGroupBox(const QString& title);
    QWidget* createScrollContent();

  private slots:
    void onFilledTextChanged();
    void onOutlinedTextChanged();
    void onDemoTextChanged();

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

    // Character counters
    QLabel* filledCounter_;
    QLabel* outlinedCounter_;

    // Interaction demo
    QLabel* charCountLabel_;
};

} // namespace cf::ui::example
