# ===== COMPONENT SETUP =====

set(COMPONENT_NAME "Hobgoblin.Private")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  # "Source/Max_rects_bin_pack.cpp" # TODO (depends on Utility... And what is this file doing here anyway?)
  "Source/Rbp_rect.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")