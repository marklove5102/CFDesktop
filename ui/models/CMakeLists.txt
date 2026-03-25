project(ui_models LANGUAGES CXX)

log_info("UI_Model" "Start Configure the UI Model")

# Placeholder for future UI model components
# This will provide SDK-like models for programmers to quickly start apps
add_library(cf_ui_models INTERFACE)

# Add alias for consistent naming
add_library(UI::cf_ui_models ALIAS cf_ui_models)

log_info("UI_Model" "the UI Model Configuration Done")