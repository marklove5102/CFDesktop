/**
 * @file CheckBoxExampleWindow.h
 * @brief Material Design 3 CheckBox Example - Main Window
 *
 * A visual gallery to display Material Design 3 checkbox with various
 * states and configurations.
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
 * @brief Main window for the Material CheckBox Example.
 *
 * Displays Material Design 3 checkbox with:
 * - Standard checkbox states (unchecked, checked, indeterminate)
 * - Error state
 * - Disabled state
 * - Checkbox with labels
 * - Interactive examples
 */
class CheckBoxExampleWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit CheckBoxExampleWindow(QWidget* parent = nullptr);
    ~CheckBoxExampleWindow() override;

  private:
    void setupUI();
    void createHeader();
    void createCheckboxStatesSection();
    void createTristateSection();
    void createErrorStateSection();
    void createDisabledSection();
    void createInteractiveDemoSection();
    void createCustomLabelsSection();

    QGroupBox* createGroupBox(const QString& title);
    QWidget* createScrollContent();

  private slots:
    void onAcceptCheckboxClicked(bool checked);
    void onTermsCheckboxClicked(bool checked);
    void onNewsletterCheckboxClicked(bool checked);
    void onErrorToggleClicked(bool checked);

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

    // Interactive demo
    QLabel* statusLabel_;
};

} // namespace cf::ui::example
