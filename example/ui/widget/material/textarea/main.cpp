/**
 * @file main.cpp
 * @brief Material TextArea Example - Main Entry Point
 *
 * Entry point for the Material Design 3 TextArea example application.
 * Displays all text area variants with various states and configurations.
 *
 * @author CFDesktop Team
 * @date 2026-03-01
 * @version 0.1
 */

#include "TextAreaExampleWindow.h"

#include "ui/core/material/material_factory_class.h"
#include "ui/core/token/theme_name/material_theme_name.h"
#include "ui/widget/material/application/material_application.h"

#include <QApplication>

using namespace cf::ui::widget::material;

int main(int argc, char* argv[]) {
    MaterialApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName("Material TextArea Example");
    app.setApplicationVersion("0.1");
    app.setOrganizationName("CFDesktop");

    // Create and show main window
    cf::ui::example::TextAreaExampleWindow window;
    window.show();

    return app.exec();
}
