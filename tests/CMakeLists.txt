# Test chess board logic
add_executable(board_test
	board_test.cpp
	"${CMAKE_SOURCE_DIR}/src/Chess/AlgebraicMove.cpp"
	"${CMAKE_SOURCE_DIR}/src/Chess/Board.cpp"
	"${CMAKE_SOURCE_DIR}/src/Chess/PseudoLegal.cpp"
)

# Test uci chess engine
set(ENGINE_TEST_SOURCES
    engine_test.cpp
	"${CMAKE_SOURCE_DIR}/src/Chess/AlgebraicMove.cpp"
	"${CMAKE_SOURCE_DIR}/src/Chess/Board.cpp"
	"${CMAKE_SOURCE_DIR}/src/Chess/PseudoLegal.cpp"
	"${CMAKE_SOURCE_DIR}/src/Engine/Engine.cpp"
)

if (WIN32)
    set(ENGINE_TEST_SOURCES ${ENGINE_TEST_SOURCES} "${CMAKE_SOURCE_DIR}/src/Platform/Windows/WindowsEngine.cpp")
    add_executable(engine_test ${ENGINE_TEST_SOURCES})
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
elseif (UNIX)
    set(ENGINE_TEST_SOURCES ${ENGINE_TEST_SOURCES} "${CMAKE_SOURCE_DIR}/src/Platform/Unix/UnixEngine.cpp")
    add_executable(engine_test ${ENGINE_TEST_SOURCES})
else()
    message(FATAL_ERROR "Unsupported platform!")
endif()

# Test PGN/game-tree logic
add_executable(pgn_test
	pgn_test.cpp
	"${CMAKE_SOURCE_DIR}/src/Chess/AlgebraicMove.cpp"
	"${CMAKE_SOURCE_DIR}/src/Chess/Board.cpp"
	"${CMAKE_SOURCE_DIR}/src/Chess/Game.cpp"
	"${CMAKE_SOURCE_DIR}/src/Chess/PseudoLegal.cpp"
)

set(TESTS board_test engine_test pgn_test)

set_target_properties(${TESTS} PROPERTIES
    CXX_STANDARD 17
    FOLDER "THINGS/Tests"
)

foreach(test IN LISTS TESTS)
    target_include_directories(${test} PRIVATE "${CMAKE_SOURCE_DIR}/src/")
endforeach()
