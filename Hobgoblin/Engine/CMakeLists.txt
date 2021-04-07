# ===== ADD COMPONENTS =====

# Add Private, Common, Preprocessor and Utility first, in that order,
# because other components may depend on them
add_subdirectory("Private")
add_subdirectory("Common")
add_subdirectory("Preprocessor")
add_subdirectory("Utility")

add_subdirectory("QAO")
add_subdirectory("RigelNet")
