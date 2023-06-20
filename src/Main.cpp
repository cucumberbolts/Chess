#include <iostream>

#include <ChessEngine/Engine.h>

#include <Graphics/Application.h>

#include <Chess/PseudoLegal.h>

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



    //Board board;

    //AlgebraicMove moves[] = {
    //    "e4",
    //    "e5",
    //    "Nf3",
    //    "Nc6",
    //    "Bc4",
    //    "Bc5",
    //    "O-O",
    //    "Nf6",
    //    "d3",
    //    "O-O",
    //    "Bg5",
    //    "d6",
    //};

    //for (size_t i = 0; i < sizeof(moves) / sizeof(AlgebraicMove); i++)
    //    std::cout << "Moving... " << board.Move(moves[i]) << "\n";

	//std::cout << board << std::endl;

    //for (auto move : moves)
    //    std::cout << move << std::endl;



    Application* app = new Application(1280, 720, "Chess");
    app->Run();
    delete app;
#elif 0
    std::cout << AlgebraicMove{ "e4" } << "\n";
    std::cout << AlgebraicMove{ "dxe4" } << "\n";
    std::cout << AlgebraicMove{ "e8=Q" } << "\n";
    std::cout << AlgebraicMove{ "Nd4" } << "\n";
    std::cout << AlgebraicMove{ "Nbd4+" } << "\n";
    std::cout << AlgebraicMove{ "Nb3d4" } << "\n";
    std::cout << AlgebraicMove{ "Nd4+" } << "\n";
    std::cout << AlgebraicMove{ "O-O-O#" } << "\n";
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
#elif 0
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

    std::cin.get();
#else
    std::string_view stockfish = "Engines/stockfish_14_x64_avx2.exe";

    {
        std::unique_ptr<Engine> engine = Engine::Create(stockfish);

        engine->Init();

        engine->Run();
        std::cout << "Running engine!\n";
        std::cin.get();
        std::cout << "Stopping engine!\n";
    }

    std::cout << s_Allocations << " allocations!\n";
    std::cout << s_BytesAllocated << " bytes allocated!\n";
    std::cout << s_Frees << " frees!\n";
#endif
}
