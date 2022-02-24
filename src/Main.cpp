#include <ChessEngine/Engine.h>

#include <iostream>

#include <sstream>

#include "Chess/Board.h"

static uint32_t s_Allocations = 0;
void* operator new(size_t size) {
    s_Allocations++;
    return malloc(size);
}

int main() {
#if 1
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;

    std::string_view stockfish14 = "Engines/stockfish_14_x64_avx2.exe";
    std::string_view leelaChess0 = "Engines/lc0.exe";

    auto engine = Engine::Create(stockfish14);

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
#elif 1
    Board board("rnbqkbnr/ppp1ppp1/7p/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3");

    std::cout << board.GetLegalMoves(ToSquare('e', '5'));
    board.Move(LongAlgebraicMove("e5d6"));
    std::cout << board << "\n";

    board.Reset();

    std::cout << "------------------------------\n\n";

    board.Move(LongAlgebraicMove("e2e4"));
    board.Move(LongAlgebraicMove("h7h6"));
    board.Move(LongAlgebraicMove("e4e5"));
    board.Move(LongAlgebraicMove("f7f5"));
    board.Move(LongAlgebraicMove("e5f6"));

    std::cout << board << "\n";
#endif
}
