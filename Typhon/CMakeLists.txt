
cmake_minimum_required(VERSION 3.14)

project(typhon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Conan setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Collect all sources
set(TYPHON_SOURCE_ALL
    #source/__Experimental/Isometric_tester.cpp
    source/__Experimental/Lighting.cpp
    #source/__Experimental/Isometric_tester.hpp
    source/__Experimental/Lighting.hpp
    #source/__Experimental/KbiTestMain.cpp
    source/SPeMPE/Include/Window_manager.hpp
    source/SPeMPE/Include/Synchronized_object_registry.hpp
    source/SPeMPE/Include/Game_object_framework.hpp
    source/SPeMPE/Include/Mouse_input.hpp
    source/SPeMPE/Include/Keyboard_input.hpp
    source/SPeMPE/Include/Networking_manager.hpp
    source/SPeMPE/Include/Game_context.hpp
    source/SPeMPE/SPeMPE.hpp
    source/SPeMPE/Implementation/Networking_manager.cpp
    source/SPeMPE/Implementation/Game_context.cpp
    source/SPeMPE/Implementation/Synchronized_object_registry.cpp
    source/SPeMPE/Implementation/Game_object_framework.cpp
    source/SPeMPE/Implementation/Keyboard_input.cpp
    source/SPeMPE/Implementation/Window_manager.cpp
    source/Typhon/Framework/Execution_priorities.hpp
    source/Typhon/Framework/Extensions.cpp
    source/Typhon/Framework/Collisions.hpp
    source/Typhon/Framework/Collisions.cpp
    source/Typhon/Framework/Execution_priorities.cpp
    source/Typhon/Framework/Extensions.hpp
    source/Typhon/GameObjects/UI/Main_menu.cpp
    source/Typhon/GameObjects/UI/Main_menu.hpp
    source/Typhon/GameObjects/Gameplay/PhysicsPlayer.cpp
    source/Typhon/GameObjects/Gameplay/PhysicsBullet.hpp
    source/Typhon/GameObjects/Gameplay/PhysicsPlayer.hpp
    source/Typhon/GameObjects/Gameplay/PhysicsBullet.cpp
    source/Typhon/GameObjects/Control/Gameplay_manager.hpp
    source/Typhon/GameObjects/Control/Controls_manager.hpp
    source/Typhon/GameObjects/Control/Environment_manager.hpp
    source/Typhon/GameObjects/Control/Gameplay_manager.cpp
    source/Typhon/GameObjects/Control/Controls_manager.cpp
    source/Typhon/GameObjects/Control/Environment_manager.cpp
    source/Typhon/Terrain/Terrain.hpp
    source/Typhon/Terrain/Terrain.cpp
    source/Typhon/Graphics/Sprites.hpp
    source/Typhon/Main/Main_impl.cpp
    source/Typhon/Main/Main_impl.hpp
    source/Typhon/Main/Main.cpp
    source/Typhon/Framework.hpp
    source/Typhon/Utility/Periodic_counter.hpp
    # source/__GameObjects/Gameplay/Player.cpp
    # source/__GameObjects/Gameplay/Player.hpp
)

# Compilation configuration
add_compile_definitions("SFML_STATIC")
add_compile_definitions("HOBGOBLIN_SHORT_NAMESPACE")

set(TYPHON_TARGET_NAME
    ${PROJECT_NAME}.exe
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    link_directories(../Hobgoblin/build/Hobgoblin-x64-Debug-Lib/lib)
else()
    link_directories(../Hobgoblin/build/Hobgoblin-x64-Release-Lib/lib)
endif()

add_executable(${TYPHON_TARGET_NAME}
    ${TYPHON_SOURCE_ALL}
)

target_include_directories(${TYPHON_TARGET_NAME} PUBLIC ../Hobgoblin/include)
target_include_directories(${TYPHON_TARGET_NAME} PUBLIC source)
target_link_libraries(${TYPHON_TARGET_NAME} PUBLIC 
    ${CONAN_LIBS}
    "hobgoblin"
)