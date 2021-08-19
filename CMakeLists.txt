cmake_minimum_required(VERSION 3.2)

project(ChessAnalysis)

set(SOURCES
    "src/ChessAnalysis/Main.cpp"
    "src/Chess/Engine.h"
    "src/Chess/Engine.cpp"
 "src/Platform/Windows/WindowsEngine.h" "src/Platform/Windows/WindowsEngine.cpp")

add_executable(ChessAnalysis ${SOURCES})

target_include_directories(ChessAnalysis
    PRIVATE
    "src/"
)
