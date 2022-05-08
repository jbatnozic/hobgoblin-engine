# ===== ADD COMPONENTS =====

# Add Private, Common, Preprocessor, Format, Logging and Utility first, 
# in that order, because other components may depend on them
add_subdirectory("Private")
add_subdirectory("Common")
add_subdirectory("Preprocessor")
add_subdirectory("Format")
add_subdirectory("Logging")
add_subdirectory("Utility")

add_subdirectory("ChipmunkPhysics")
add_subdirectory("ColDetect")
add_subdirectory("Graphics")
add_subdirectory("Math")
add_subdirectory("QAO")
add_subdirectory("RigelNet")