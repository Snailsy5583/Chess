set(PROJECT_NAME Chess)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "Main.cpp"
    "stb.cpp"
)
source_group("" FILES ${no_group_source_files})

set(src
    "src/Application.cpp"
    "src/Application.h"
)
source_group("src" FILES ${src})

set(src__Board
    "src/Board/Board.cpp"
    "src/Board/Board.h"
)
source_group("src\\Board" FILES ${src__Board})

set(src__Board__Pieces
    "src/Board/Pieces/Piece.cpp"
    "src/Board/Pieces/Piece.h"
    "src/Board/Pieces/SlidingPieces.cpp"
    "src/Board/Pieces/SlidingPieces.h"
    "src/Board/Pieces/SpecialPieces.cpp"
    "src/Board/Pieces/SpecialPieces.h"
)
source_group("src\\Board\\Pieces" FILES ${src__Board__Pieces})

set(src__Engine
    "src/Engine/Layer.cpp"
    "src/Engine/Layer.h"
    "src/Engine/Renderer.cpp"
    "src/Engine/Renderer.h"
    "src/Engine/Shader.cpp"
    "src/Engine/Shader.h"
    "src/Engine/Window.cpp"
    "src/Engine/Window.h"
)
source_group("src\\Engine" FILES ${src__Engine})

set(src__Engine__Events
    "src/Engine/Events/Events.h"
    "src/Engine/Events/MouseEvents.h"
    "src/Engine/Events/WindowEvents.h"
)
source_group("src\\Engine\\Events" FILES ${src__Engine__Events})

set(ALL_FILES
    ${no_group_source_files}
    ${src}
    ${src__Board}
    ${src__Board__Pieces}
    ${src__Engine}
    ${src__Engine__Events}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Chess)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_DEBUG   "Chess"
    TARGET_NAME_DIST    "Chess"
    TARGET_NAME_RELEASE "Chess"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../bin/Debug-windows-x86_64/Chess/"
    OUTPUT_DIRECTORY_DIST    "${CMAKE_CURRENT_SOURCE_DIR}/../bin/Dist-windows-x86_64/Chess/"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../bin/Release-windows-x86_64/Chess/"
)

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/spdlog/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/glfw/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/GLAD/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/stb;"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "GLFW_INCLUDE_NONE;"
    "UNICODE;"
    "_UNICODE"
)

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    GLAD
)

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "opengl32"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

