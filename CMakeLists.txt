cmake_minimum_required(VERSION 3.2)

project(Chess)

set(SOURCES
    "src/Main.cpp"

    "src/Chess/BitBoard.h"
    "src/Chess/Board.h"
    "src/Chess/Board.cpp"
    "src/Chess/BoardFormat.h"
    "src/Chess/PseudoLegal.h"
    "src/Chess/PseudoLegal.cpp"
    "src/Chess/Move.h"

    "src/ChessEngine/Engine.h"
    "src/ChessEngine/Engine.cpp"
    "src/ChessEngine/Option.h"

    "src/Graphics/Application.h"
    "src/Graphics/Application.cpp"
    "src/Graphics/Buffer.h"
    "src/Graphics/Buffer.cpp"
    "src/Graphics/Shader.h"
    "src/Graphics/Shader.cpp"
    "src/Graphics/VertexArray.h"
    "src/Graphics/VertexArray.cpp"

    "src/Utility/StringParser.h"

    "src/Platform/Windows/WindowsEngine.h"
    "src/Platform/Windows/WindowsEngine.cpp"
)

if (MSVC)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif(MSVC)

add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

target_include_directories(${PROJECT_NAME}
    PRIVATE
    "src/"
)

# ---------- DEPENDENCIES ----------

# GLFW
set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)

add_subdirectory(dependencies/glfw)

# OpenGL
find_package(OpenGL REQUIRED)

# glad
add_subdirectory(dependencies/glad)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    ${OPENGL_LIBRARY}
    glad
)
