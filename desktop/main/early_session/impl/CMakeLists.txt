# Collect impl directory sources
# Note: paths are prefixed with 'impl/' for parent usage

set(IMPL_SOURCES
    impl/early_welcome_impl.cpp
    impl/logger_stage.cpp
    impl/early_config_stage.cpp
)

# Return to parent via PARENT_SCOPE
set(IMPL_SOURCES ${IMPL_SOURCES} PARENT_SCOPE)
