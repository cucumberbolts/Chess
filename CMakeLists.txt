cmake_minimum_required(VERSION 3.2)

project(Chess)

set(SOURCES
    "src/Main.cpp"

    "src/ChessEngine/Engine.h"
    "src/ChessEngine/Engine.cpp"
    "src/ChessEngine/Option.h"

    "src/Utility/StringParser.h"

    "src/Chess/BitBoard.h"
    "src/Chess/Board.h"
    "src/Chess/Board.cpp"
    "src/Chess/BoardFormat.h"
    "src/Chess/KindergartenBitBoard.h"
    "src/Chess/KindergartenBitBoard.cpp"
    "src/Chess/Move.h"
    "src/Chess/Move.cpp"

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
