/**
 * @file LabelExampleWindow.h
 * @brief Material Design 3 Label Example - Main Window
 *
 * A visual gallery to display all Material Design 3 label typography
 * styles and color variants.
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
 * @brief Main window for the Material Label Example.
 *
 * Displays all 15 Material Design 3 typography styles:
 * - Display Large/Medium/Small (hero content)
 * - Headline Large/Medium/Small (app bar text)
 * - Title Large/Medium/Small (section headings)
 * - Body Large/Medium/Small (main content)
 * - Label Large/Medium/Small (secondary information)
 *
 * Also demonstrates:
 * - Different color variants (OnSurface, Primary, Error, etc.)
 * - Disabled state
 * - Auto-hiding behavior
 * - Word wrap functionality
 */
class LabelExampleWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit LabelExampleWindow(QWidget* parent = nullptr);
    ~LabelExampleWindow() override;

  private:
    void setupUI();
    void createHeader();
    void createDisplaySection();
    void createHeadlineSection();
    void createTitleSection();
    void createBodySection();
    void createLabelSection();
    void createColorVariantsSection();
    void createStatesSection();
    void createFeaturesSection();

    QGroupBox* createGroupBox(const QString& title);
    QWidget* createScrollContent();

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
