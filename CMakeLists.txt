cmake_minimum_required(VERSION 3.2)

project(Chess)

set(SOURCES
    "src/Main.cpp"
    "src/ChessApplication.h"
    "src/ChessApplication.cpp"

    "src/Resources.h"

    "src/Chess/AlgebraicMove.cpp"
    "src/Chess/BitBoard.h"
    "src/Chess/Board.h"
    "src/Chess/Board.cpp"
    "src/Chess/BoardFormat.h"
    "src/Chess/ChessException.h"
    "src/Chess/PseudoLegal.h"
    "src/Chess/PseudoLegal.cpp"
    "src/Chess/Move.h"

    "src/ChessEngine/Engine.h"
    "src/ChessEngine/Engine.cpp"
    "src/ChessEngine/EngineException.h"
    "src/ChessEngine/Option.h"

    "src/Graphics/Application.h"
    "src/Graphics/Application.cpp"
    "src/Graphics/Buffer.h"
    "src/Graphics/Buffer.cpp"
    "src/Graphics/DebugContext.h"
    "src/Graphics/DebugContext.cpp"
    "src/Graphics/Framebuffer.h"
    "src/Graphics/Framebuffer.cpp"
    "src/Graphics/Renderer.h"
    "src/Graphics/Renderer.cpp"
    "src/Graphics/Shader.h"
    "src/Graphics/Shader.cpp"
    "src/Graphics/SubTexture.h"
    "src/Graphics/SubTexture.cpp"
    "src/Graphics/Texture.h"
    "src/Graphics/Texture.cpp"
    "src/Graphics/VertexArray.h"
    "src/Graphics/VertexArray.cpp"

    "src/Utility/FileDialog.h"
    "src/Utility/StringParser.h"
    "src/Utility/Timer.h"

    "dependencies/imgui/imgui.cpp"
    "dependencies/imgui/imgui_demo.cpp"
    "dependencies/imgui/imgui_draw.cpp"
    "dependencies/imgui/imgui_tables.cpp"
    "dependencies/imgui/imgui_widgets.cpp"
    "dependencies/imgui/backends/imgui_impl_glfw.cpp"
    "dependencies/imgui/backends/imgui_impl_opengl3.cpp"
)

if (WIN32)
    add_compile_definitions(OS_WINDOWS)
elseif (UNIX)
    add_compile_definitions(OS_LINUX)
endif()

if (WIN32)
    set(SOURCES
        ${SOURCES}
        "src/Platform/Windows/WindowsEngine.h"
        "src/Platform/Windows/WindowsEngine.cpp"
        "src/Platform/Windows/WindowsFileDialog.cpp"
    )

    add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
elseif (UNIX)
    set(SOURCES
        ${SOURCES}
        "src/Platform/Unix/UnixEngine.h"
        "src/Platform/Unix/UnixEngine.cpp"
    )

    add_executable(${PROJECT_NAME} ${SOURCES})
else()
    message(FATAL_ERROR "Unsupported platform!")
endif()

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

target_include_directories(${PROJECT_NAME}
    PRIVATE
    "dependencies/stb_image/"
    "dependencies/glm/"
    "dependencies/imgui/"
)
