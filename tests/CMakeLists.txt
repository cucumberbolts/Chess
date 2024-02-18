add_executable(pgn_test
	pgn_test.cpp
	"${CMAKE_SOURCE_DIR}/src/Chess/AlgebraicMove.cpp"
	"${CMAKE_SOURCE_DIR}/src/Chess/Board.cpp"
	"${CMAKE_SOURCE_DIR}/src/Chess/Game.cpp"
	"${CMAKE_SOURCE_DIR}/src/Chess/PseudoLegal.cpp"
)

set(TESTS pgn_test)

set_target_properties(${TESTS} PROPERTIES
    CXX_STANDARD 17
    FOLDER "THINGS/Tests"
)

target_include_directories(${TESTS} PRIVATE "${CMAKE_SOURCE_DIR}/src/")
