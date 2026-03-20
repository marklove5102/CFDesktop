log_info("Example Base Common" "Creating PolicyChain Example")

# PolicyChain example executable
add_executable(example_policy_chain
    example_policy_chain.cpp
)

target_link_libraries(example_policy_chain
    PRIVATE
        cfbase
)

# Set C++20 for this example
target_compile_features(example_policy_chain PRIVATE cxx_std_20)

# Set output to examples directory
cf_set_example_output_dir(example_policy_chain "base")

# Register for launcher generation (Windows)
cf_register_example_launcher(example_policy_chain "base")

log_info("Example Base Common" "PolicyChain Example Created!")
