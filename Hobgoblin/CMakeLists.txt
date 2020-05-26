
cmake_minimum_required(VERSION 3.14)

project(hobgoblin VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Conan setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Collect all sources
set(HOBGOBLIN_SOURCE_CHIPMUNK
    source/Hobgoblin/ChipmunkPhysics/cpSpace.c
    source/Hobgoblin/ChipmunkPhysics/cpSpaceStep.c
    source/Hobgoblin/ChipmunkPhysics/cpSpaceDebug.c
    source/Hobgoblin/ChipmunkPhysics/cpGearJoint.c
    source/Hobgoblin/ChipmunkPhysics/cpGrooveJoint.c
    source/Hobgoblin/ChipmunkPhysics/cpSpaceQuery.c
    #source/Hobgoblin/ChipmunkPhysics/prime.h
    source/Hobgoblin/ChipmunkPhysics/cpHashSet.c
    source/Hobgoblin/ChipmunkPhysics/cpDampedRotarySpring.c
    source/Hobgoblin/ChipmunkPhysics/cpBody.c
    source/Hobgoblin/ChipmunkPhysics/cpSweep1D.c
    source/Hobgoblin/ChipmunkPhysics/cpShape.c
    source/Hobgoblin/ChipmunkPhysics/cpRobust.c
    source/Hobgoblin/ChipmunkPhysics/cpPinJoint.c
    source/Hobgoblin/ChipmunkPhysics/cpPivotJoint.c
    source/Hobgoblin/ChipmunkPhysics/cpCollision.c
    source/Hobgoblin/ChipmunkPhysics/cpBBTree.c
    source/Hobgoblin/ChipmunkPhysics/cpArray.c
    source/Hobgoblin/ChipmunkPhysics/cpMarch.c
    source/Hobgoblin/ChipmunkPhysics/cpPolyline.c
    source/Hobgoblin/ChipmunkPhysics/chipmunk.c
    source/Hobgoblin/ChipmunkPhysics/cpDampedSpring.c
    source/Hobgoblin/ChipmunkPhysics/cpSpaceComponent.c
    source/Hobgoblin/ChipmunkPhysics/cpConstraint.c
    source/Hobgoblin/ChipmunkPhysics/cpPolyShape.c
    source/Hobgoblin/ChipmunkPhysics/cpSpaceHash.c
    source/Hobgoblin/ChipmunkPhysics/cpArbiter.c
    source/Hobgoblin/ChipmunkPhysics/cpSimpleMotor.c
    source/Hobgoblin/ChipmunkPhysics/cpRotaryLimitJoint.c
    source/Hobgoblin/ChipmunkPhysics/cpSpatialIndex.c
    source/Hobgoblin/ChipmunkPhysics/cpRatchetJoint.c
    source/Hobgoblin/ChipmunkPhysics/cpSlideJoint.c
    source/Hobgoblin/ChipmunkPhysics/cpHastySpace.c
)

set(HOBGOBLIN_SOURCE_COLDETECT
    source/Hobgoblin/ColDetect/Qtree_collision_domain.cpp  
)

set(HOBGOBLIN_SOURCE_GRAPHICS
    source/Hobgoblin/Graphics/Sprite_loader.cpp
    source/Hobgoblin/Graphics/Multisprite.cpp
    source/Hobgoblin/Graphics/Color.cpp
    source/Hobgoblin/Graphics/Texture_packing.cpp
    source/Hobgoblin/Graphics/Views.cpp
    source/Hobgoblin/Graphics/Builtin_fonts.cpp
    source/Hobgoblin/Graphics/Brush.cpp    
)

#set(HOBGOBLIN_SOURCE_MATH
#     source/Hobgoblin/
#)

set(HOBGOBLIN_SOURCE_MAIN
     source/Hobgoblin/RigelNetMain.cpp
    # source/Hobgoblin/BrushMain.cpp
    # source/Hobgoblin/TimingMain.cpp
    # source/Hobgoblin/TextRect.cpp
    # source/Hobgoblin/SerializationMain.cpp
    # source/Hobgoblin/SpriteManagerMain.cpp
#    source/Hobgoblin/CollisionsMain.cpp
)

set(HOBGOBLIN_SOURCE_PRIVATE
    source/Hobgoblin/Private/Max_rects_bin_pack.cpp
    source/Hobgoblin/Private/Rbp_rect.cpp
)

set(HOBGOBLIN_SOURCE_QAO
    source/Hobgoblin/QAO/Priority_resolver2.cpp
    source/Hobgoblin/QAO/id.cpp
    source/Hobgoblin/QAO/base.cpp
    source/Hobgoblin/QAO/orderer.cpp
    source/Hobgoblin/QAO/registry.cpp
    source/Hobgoblin/QAO/runtime.cpp
    source/Hobgoblin/QAO/Priority_resolver.cpp
)

set(HOBGOBLIN_SOURCE_RIGELNET
    source/Hobgoblin/RigelNet/Udp_server.cpp
    source/Hobgoblin/RigelNet/Node.cpp
    source/Hobgoblin/RigelNet/Packet.cpp
    source/Hobgoblin/RigelNet/Handlermgmt.cpp
    source/Hobgoblin/RigelNet/Udp_client.cpp
    source/Hobgoblin/RigelNet/Event.cpp
    source/Hobgoblin/RigelNet/Udp_connector.cpp
)

set(HOBGOBLIN_SOURCE_UTILITY
    source/Hobgoblin/Utility/Exceptions.cpp
    source/Hobgoblin/Utility/Time_utils.cpp
    source/Hobgoblin/Utility/Serialization.cpp
)

set(HOBGOBLIN_SOURCE_ALL
    ${HOBGOBLIN_SOURCE_CHIPMUNK}
    source/Hobgoblin/ChipmunkPhysics/prime.h
    ${HOBGOBLIN_SOURCE_COLDETECT}
    ${HOBGOBLIN_SOURCE_GRAPHICS}
    ${HOBGOBLIN_SOURCE_PRIVATE}
    ${HOBGOBLIN_SOURCE_QAO}
    ${HOBGOBLIN_SOURCE_RIGELNET}
    ${HOBGOBLIN_SOURCE_UTILITY}
)

if(BUILD_DEBUG_EXECUTABLE)
    set(HOBGOBLIN_SOURCE_ALL
        ${HOBGOBLIN_SOURCE_ALL}
        ${HOBGOBLIN_SOURCE_MAIN}
    )
endif()

# Compilation configuration
add_compile_definitions("SFML_STATIC")
# Force Chipmunk to be compiled as C++
set_source_files_properties(${HOBGOBLIN_SOURCE_CHIPMUNK} PROPERTIES LANGUAGE CXX)

if(BUILD_DEBUG_EXECUTABLE)
    set(HOBGOBLIN_TARGET_NAME
        ${PROJECT_NAME}-debug-executable
    )
    add_executable(${HOBGOBLIN_TARGET_NAME}
        ${HOBGOBLIN_SOURCE_ALL}
    )
else()
    set(HOBGOBLIN_TARGET_NAME
        ${PROJECT_NAME}
    )
    add_library(${HOBGOBLIN_TARGET_NAME}
        STATIC
        ${HOBGOBLIN_SOURCE_ALL}
    )
endif()

target_include_directories(${HOBGOBLIN_TARGET_NAME} PUBLIC include)
target_include_directories(${HOBGOBLIN_TARGET_NAME} PUBLIC include/Hobgoblin/private)
target_link_libraries(${HOBGOBLIN_TARGET_NAME} PUBLIC legacy_stdio_definitions.lib ${CONAN_LIBS})
