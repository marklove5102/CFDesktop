/**
 * @file main.cpp
 * @brief Material RadioButton Example - Main Entry Point
 *
 * Entry point for the Material Design 3 RadioButton example application.
 * Displays radio buttons with various states and configurations.
 *
 * @author CFDesktop Team
 * @date 2026-03-01
 * @version 0.1
 */

#include "RadioButtonExampleWindow.h"

#include "ui/core/material/material_factory_class.h"
#include "ui/core/token/theme_name/material_theme_name.h"
#include "ui/widget/material/application/material_application.h"

#include <QApplication>

using namespace cf::ui::widget::application_support;

int main(int argc, char* argv[]) {
    cf::ui::widget::material::MaterialApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName("Material RadioButton Example");
    app.setApplicationVersion("0.1");
    app.setOrganizationName("CFDesktop");

    // Create and show main window
    cf::ui::example::RadioButtonExampleWindow window;
    window.show();

    return app.exec();
}
