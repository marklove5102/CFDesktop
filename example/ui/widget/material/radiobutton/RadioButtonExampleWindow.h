/**
 * @file RadioButtonExampleWindow.h
 * @brief Material Design 3 RadioButton Example - Main Window
 *
 * A visual gallery to display Material Design 3 radio button functionality
 * with various states and configurations.
 *
 * @author CFDesktop Team
 * @date 2026-03-01
 * @version 0.1
 */

#pragma once

#include <QButtonGroup>
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
 * @brief Main window for the Material RadioButton Example.
 *
 * Displays Material Design 3 radio buttons with various configurations:
 * - Basic radio buttons with mutual exclusion
 * - Disabled states
 * - Error state
 * - Button groups for independent selection
 * - Interactive demo
 */
class RadioButtonExampleWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit RadioButtonExampleWindow(QWidget* parent = nullptr);
    ~RadioButtonExampleWindow() override;

  private:
    void setupUI();
    void createHeader();
    void createBasicRadioSection();
    void createRadioStatesSection();
    void createButtonGroupSection();
    void createErrorStateSection();
    void createInteractionDemoSection();

    QGroupBox* createGroupBox(const QString& title);

  private slots:
    void onOption1Clicked();
    void onOption2Clicked();
    void onOption3Clicked();
    void onErrorToggleClicked();

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

    // Interaction demo
    QLabel* selectionLabel_;
};

} // namespace cf::ui::example
