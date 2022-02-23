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
    /*
    Board board("r1bqk1nr/pppp1ppp/2n5/2b1p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 4 4");
    board.Reset();

    std::cout << board << "\n";

    board.Move(LongAlgebraicMove("e2e4"));
    board.Move(LongAlgebraicMove("e7e5"));
    board.Move(LongAlgebraicMove("g1f3"));
    board.Move(LongAlgebraicMove("b8c6"));
    board.Move(LongAlgebraicMove("f1c4"));
    board.Move(LongAlgebraicMove("f8c5"));

    board.Move(LongAlgebraicMove("e1g1"));

    std::cout << board << "\n";

    board.FromFEN("rnbqk2r/pppp1ppp/8/8/8/8/PPPP1PPP/RNBQR1K1 b Qkq - 0 1");
    std::cout << board.GetLegalMoves(ToSquare('e', '8')) << "\n";
    
    board.FromFEN("rnbqk2r/pppp1ppp/8/8/8/8/PPPP1PPP/RNBQ2K1 b Qkq - 0 1");
    std::cout << board.GetLegalMoves(ToSquare('e', '8')) << "\n";
    */
    Board board;

    std::istringstream iss("d2d4 d7d5 c2c4 e7e6 b1c3 c7c5 c4d5 e6d5 g1f3 c5d4 f3d4 b8c6 c1f4 g8f6 e2e3 f8c5 f1e2 c5d4 e3d4 e8g8");
    std::string move;
    iss >> move;
    while (iss) {
        std::cout << "move: " << move << "\n";
        board.Move(LongAlgebraicMove(move));
        iss >> move;
    }

    std::cout << board << "\n";
#elif 1
    Board board("7k/8/8/8/K4q2/3N4/8/8 w - - 0 1");
    std::cout << board << "\n";
    std::cout << board.GetPseudoLegalMoves(ToSquare('d', '3')) << "\n";
    std::cout << board.GetLegalMoves(ToSquare('d', '3')) << "\n";
    
    Board board1("7k/8/1n6/8/K7/8/1Q6/8 w - - 0 1");
    std::cout << board1 << "\n";
    std::cout << board1.GetPseudoLegalMoves(ToSquare('b', '2')) << "\n";
    std::cout << board1.GetLegalMoves(ToSquare('b', '2')) << "\n";
    
    Board board2("4k3/8/8/8/4n3/2K4r/8/3Q4 w - - 0 1");
    std::cout << board2 << "\n";
    std::cout << board2.GetPseudoLegalMoves(ToSquare('d', '1')) << "\n";
    std::cout << board2.GetLegalMoves(ToSquare('d', '1')) << "\n";
    std::cout << board2.GetLegalMoves(ToSquare('c', '3')) << "\n";
    
    Board board3("4k3/8/8/8/8/2b3b1/4N3/4K3 w - - 0 1");
    std::cout << board3 << "\n";
    std::cout << board3.GetPseudoLegalMoves(ToSquare('e', '2')) << "\n";
    std::cout << board3.GetLegalMoves(ToSquare('e', '2')) << "\n";

    Board board4("4k3/8/8/8/8/8/3p2R1/4K3 w - - 0 1");
    std::cout << board4 << "\n";
    std::cout << board4.GetLegalMoves(ToSquare('g', '2')) << "\n";
    std::cout << board4.GetLegalMoves(ToSquare('e', '1')) << "\n";

    Board board5("4k3/8/4r3/8/8/8/3p2R1/4K3 w - - 0 1");
    std::cout << board5 << "\n";
    std::cout << board5.GetLegalMoves(ToSquare('g', '2')) << "\n";
    std::cout << board5.GetLegalMoves(ToSquare('e', '1')) << "\n";
#endif
}
