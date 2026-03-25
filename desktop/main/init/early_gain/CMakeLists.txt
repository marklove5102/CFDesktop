# Collect early_gain sources
# Note: paths are prefixed with 'early_gain/' for parent usage

set(EARLY_GAIN_SOURCES
    early_gain/early_pass_stage.cpp
)

# Return to parent via PARENT_SCOPE
set(EARLY_GAIN_SOURCES ${EARLY_GAIN_SOURCES} PARENT_SCOPE)
