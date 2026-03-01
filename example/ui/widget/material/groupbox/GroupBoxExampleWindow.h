/**
 * @file GroupBoxExampleWindow.h
 * @brief Material Design 3 GroupBox Example - Main Window
 *
 * A visual gallery to display Material Design 3 group box variants
 * with various states and configurations.
 *
 * @author CFDesktop Team
 * @date 2026-03-01
 * @version 0.1
 */

#pragma once

#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

namespace cf::ui::example {

/**
 * @brief Main window for the Material GroupBox Example.
 *
 * Displays Material Design 3 group box with various configurations:
 * - Different elevation levels (0-5)
 * - With and without border
 * - Different corner radius values
 * - Content examples with various widgets
 * - Title variations
 */
class GroupBoxExampleWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit GroupBoxExampleWindow(QWidget* parent = nullptr);
    ~GroupBoxExampleWindow() override;

  private:
    void setupUI();
    void createHeader();
    void createBasicGroupBoxSection();
    void createElevationSection();
    void createBorderSection();
    void createCornerRadiusSection();
    void createContentExamplesSection();
    void createNestedGroupBoxSection();

  private:
    // UI components
    QScrollArea* scrollArea_;
    QWidget* scrollContent_;
    QVBoxLayout* scrollLayout_;

    // Header
    QLabel* titleLabel_;
    QLabel* subtitleLabel_;
};

} // namespace cf::ui::example
