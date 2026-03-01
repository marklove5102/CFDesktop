/**
 * @file main.cpp
 * @brief Material GroupBox Example - Main Entry Point
 *
 * Entry point for the Material Design 3 GroupBox example application.
 * Displays group box with various elevation levels, border styles,
 * and corner radius configurations.
 *
 * @author CFDesktop Team
 * @date 2026-03-01
 * @version 0.1
 */

#include "GroupBoxExampleWindow.h"

#include "ui/core/material/material_factory_class.h"
#include "ui/core/token/theme_name/material_theme_name.h"
#include "ui/widget/material/application/material_application.h"

#include <QApplication>

using namespace cf::ui::widget::application_support;

int main(int argc, char* argv[]) {
    cf::ui::widget::material::MaterialApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName("Material GroupBox Example");
    app.setApplicationVersion("0.1");
    app.setOrganizationName("CFDesktop");

    // Create and show main window
    cf::ui::example::GroupBoxExampleWindow window;
    window.show();

    return app.exec();
}
