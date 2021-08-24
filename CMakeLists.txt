cmake_minimum_required(VERSION 3.2)

project(ChessAnalysis)

set(SOURCES
    "src/ChessAnalysis/Main.cpp"
    "src/Chess/Engine.h"
    "src/Chess/Engine.cpp"
    "src/Platform/Windows/WindowsEngine.h"
    "src/Platform/Windows/WindowsEngine.cpp"
    "src/Chess/Option.h"
    "src/Chess/StringParser.h"
)

if (MSVC)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif(MSVC)

add_executable(ChessAnalysis ${SOURCES})

set_target_properties(ChessAnalysis PROPERTIES CXX_STANDARD 17)

target_include_directories(ChessAnalysis
    PRIVATE
    "src/"
)
