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
#elif 0
    Board board("3k4/1P6/8/8/8/8/6K1/8 w - - 0 1");

    std::cout << board << "\n";

    std::cout << board.GetLegalMoves(ToSquare('b', '7'));
    std::cout << board.GetLegalMoves(ToSquare('d', '8'));
    board.Move(LongAlgebraicMove("b7b8n"));
    std::cout << board << "\n";
    std::cout << board.GetLegalMoves(ToSquare('d', '8'));
#elif 1
    Board board;

    std::istringstream ss("e2e4 c7c5 g1f3 d7d6 f1c4 b8c6 e1g1 g8f6 b1c3 e7e6 d2d4 c5d4 f3d4");

    std::string move;
    while (ss) {
        ss >> move;

        if (!ss)
            break;

        board.Move(LongAlgebraicMove(move));

        std::cout << board << "\n";
    }
#endif
}
