cmake_minimum_required(VERSION 3.2)

project(Chess)

set(SOURCES
    "src/Main.cpp"

    "src/Chess/AlgebraicMove.cpp"
    "src/Chess/BitBoard.h"
    "src/Chess/Board.h"
    "src/Chess/Board.cpp"
    "src/Chess/BoardFormat.h"
    "src/Chess/ExceptionTypes.h"
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
    "src/Graphics/DebugContext.h"
    "src/Graphics/DebugContext.cpp"
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
    set(SOURCES
        ${SOURCES}
        "src/Platform/Windows/WindowsEngine.h"
        "src/Platform/Windows/WindowsEngine.cpp"
    )
elseif (UNIX)
    set(SOURCES
        ${SOURCES}
        "src/Platform/Unix/UnixEngine.h"
        "src/Platform/Unix/UnixEngine.cpp"
    )
else()
    message(FATAL_ERROR "Unsupported platform!")
endif()

if (MSVC)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif ()

add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

target_include_directories(${PROJECT_NAME}
    PRIVATE
    "src/"
)

# Copy resources to executable folder
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)

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
