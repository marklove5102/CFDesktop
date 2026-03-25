log_info("Example Base Common" "Creating Device Example")

# Device example executable
add_executable(example_console_helper
    example_console_helper.cpp
)
target_link_libraries(example_console_helper
    PRIVATE
        cfbase
)

# Set output to examples directory
cf_set_example_output_dir(example_console_helper "base")
# Register for launcher generation (Windows)
cf_register_example_launcher(example_console_helper "base")

log_info("Example Base Common" "Device Example Created!")
