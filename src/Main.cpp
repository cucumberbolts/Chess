#include <iostream>

#include "ChessApplication.h"

#include "Chess/BitBoard.h"

#if 0
static uint32_t s_Allocations = 0;
static uint32_t s_BytesAllocated = 0;
static uint32_t s_Frees = 0;

void* operator new(size_t size) {
    s_Allocations++;
    s_BytesAllocated += size;
    return malloc(size);
}

void operator delete(void* ptr) noexcept {
    s_Frees++;
    free(ptr);
}
#endif

int main() {
#if 1
    //Board b("k7/8/8/4Q3/8/4Q1Q1/8/K7 w - - 0 1");
    //std::cout << b.Move(AlgebraicMove("e4"))  << "\n";
    //std::cout << b.Move(AlgebraicMove("e5"))  << "\n";
    //std::cout << b.Move(AlgebraicMove("Nf3")) << "\n";
    //std::cout << b.Move(AlgebraicMove("Nc6")) << "\n";
    //std::cout << b.Move(AlgebraicMove("Qh2")) << "\n";
    //std::cout << b.Move(AlgebraicMove("Qef3")) << "\n";
    //std::cout << b.Move(AlgebraicMove("Qgf3")) << "\n";

    //std::cout << b << std::endl;
#if 0
    Board board;

    std::string inputString = R"(
e2e4 c7c5 g1f3 b8c6
d2d4 c5d4 f3d4 e7e6
b1c3 d8c7 c1e3 a7a6
d1f3 g8f6 d4c6 b7c6
e1c1 d7d5 f3g3 c7g3
h2g3 f8d6 e3g5 d6e5
e4d5 c6d5 g5f4 e5f4 g3f4
	)";

    try {
        std::istringstream input(inputString);
        std::string move;
        uint32_t iteration = 0;
        while (input >> move) {
            std::cout << "Iteration: " << iteration << "\n";
            AlgebraicMove m = board.Move(LongAlgebraicMove(move));
            std::cout << "Moving " << m << "\n";
            iteration++;
        }
    } catch (IllegalMoveException& e) {
        std::cout << "Illegal move: " << e.move() << "\n";
    }

	std::cout << board << std::endl;

#endif
    
    auto app = new ChessApplication(1280, 720, "Chess");
    try {
        app->Run();
    } catch (...) {
    	std::cout << "Unhandled exception!\n";
    }
    delete app;

#elif 0 // Test algebraic move generation in Board::Move()
    // 8/1b5b/2N3N1/8/4K3/3N1N2/8/8 w - - 0 1  // Pinned knights
    // k7/8/8/4Q3/8/4Q1Q1/8/K7 w - - 0 1       // Multiple queens can go to the same square
    Board b;

    // With a pin
    std::cout << b.Move({ E2, E4 }) << std::endl;
    std::cout << b.Move({ E7, E5 }) << std::endl;
    std::cout << b.Move({ G1, F3 }) << std::endl;
    std::cout << b.Move({ B8, C6 }) << std::endl;
    std::cout << b.Move({ F1, B5 }) << std::endl;
    std::cout << b.Move({ D7, D6 }) << std::endl;
    std::cout << b.Move({ B1, C3 }) << std::endl;
    std::cout << b.Move({ G8, E7 }) << std::endl;
    std::cout << b.Move({ B5, C6 }) << std::endl;

    std::cout << "---------------------------------------------------\n";

    // Without a pin
    b.FromFEN("r1bqkbnr/pppp1ppp/2n5/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 3 3");
    std::cout << b << std::endl;
    std::cout << b.Move({ G8, E7 }) << std::endl;

    // Rank stuff
    std::cout << "---------------------------------------------------\n";

    b.FromFEN("1k6/8/8/2N5/8/2N5/8/1K6 w - - 0 1");
    std::cout << b.Move({ C3, E4 }) << std::endl;

    std::cout << "---------------------------------------------------\n";

    b.FromFEN("1k6/8/8/2N5/8/2N3N1/8/1K6 w - - 0 1");
    std::cout << b.Move({ C3, E4 }) << std::endl;
#elif 0  // Testing Engine class
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;

    std::string_view stockfish14 = "Engines/stockfish_14_x64_avx2.exe";
    std::string_view leelaChess0 = "Engines/lc0.exe";

    auto engine = Engine::Create(leelaChess0);

    engine->Init();

    std::cout << "Printing info...\n";
    engine->PrintInfo();
    std::cout << "Done printing info...\n";

    std::cout << "Press enter to start engine:\n";
    std::cin.get();

    engine->Run();
    std::cin.get();

    std::cout << "Stopping engine...\n";
    engine->Stop();

    std::cout << "Best continuation: ";
    for (AlgebraicMove m : engine->m_BestContinuation.Continuation)
        std::cout << m << " ";

    std::cout << "\nDepth: " << engine->m_BestContinuation.Depth << "\n\n";

    std::cout << "Board: \n" << engine->m_Board << "\n";

    std::cout << s_Allocations << " allocations!\n";
    std::cout << s_BytesAllocated << " bytes allocated!\n";
    std::cout << s_Frees << " frees!\n";

    std::cin.get();
#endif
}
