# ------ Qt Setup ------
# find Qt6, As Requested
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)
log_info("Post Qt Setups" "Run Post Qt Dependencies check")
include(cmake/custom_target_helper.cmake)
log_info("Post Qt Setups" "Run Post Qt Dependencies OK")
