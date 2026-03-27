# Collect early_gain sources
# Paths relative to desktop/main (parent scope)

set(EARLY_GAIN_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/early_pass_stage.cpp
)

# Return to parent via PARENT_SCOPE
set(EARLY_GAIN_SOURCES ${EARLY_GAIN_SOURCES} PARENT_SCOPE)
