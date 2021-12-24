#include <ChessEngine/Engine.h>

#include <iostream>

#include "Chess/Board.h"

static uint32_t s_Allocations = 0;
void* operator new(size_t size) {
    s_Allocations++;
    return malloc(size);
}

int main() {
#if 0
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
    Board board;

    for (int i = 48; i < 64; i++) {
        std::cout << board.GetPseudoLegalMoves((Square)i) << "\n\n";
    }

    std::cout << std::boolalpha;
    std::cout << "is e2e4 possible? " << board.IsMovePseudoLegal(LongAlgebraicMove("e2e4")) << "\n";
    std::cout << "is e7e5 possible? " << board.IsMovePseudoLegal(LongAlgebraicMove("e7e5")) << "\n";
    std::cout << "is e2e5 possible? " << board.IsMovePseudoLegal(LongAlgebraicMove("e2e5")) << "\n";
    std::cout << "is b1e6 possible? " << board.IsMovePseudoLegal(LongAlgebraicMove("b1e6")) << "\n";
    std::cout << "is b1c3 possible? " << board.IsMovePseudoLegal(LongAlgebraicMove("b1c3")) << "\n";

    std::cout << "\nMoving e2e4!\n\n";
    board.Move(LongAlgebraicMove("e2e4"));
    std::cout << board << "\n\n";

    std::cout << "is e2e4 possible? " << board.IsMovePseudoLegal(LongAlgebraicMove("e2e4")) << "\n";
    std::cout << "is e7e5 possible? " << board.IsMovePseudoLegal(LongAlgebraicMove("e7e5")) << "\n";

#else
    constexpr Square testSquare = ToSquare('b', '2');
    BitBoard testBoard;
    testBoard[ToSquare('g', '5')] = true;
    testBoard[ToSquare('a', '7')] = true;
    testBoard[ToSquare('d', '5')] = true;
    testBoard[ToSquare('c', '3')] = true;
    testBoard[ToSquare('g', '4')] = true;

    std::cout << testBoard << "\n\n\n";

    std::cout << "Pawn attack (white): \n" << BitBoard::PawnAttack(testSquare, testBoard, Colour::White) << "\n\n";
    std::cout << "Pawn attack (black): \n" << BitBoard::PawnAttack(testSquare, testBoard, Colour::Black) << "\n\n";
    std::cout << "Knight attack: \n" << BitBoard::KnightAttack(testSquare) << "\n\n";
    std::cout << "Bishop attack: \n" << BitBoard::BishopAttack(testSquare, testBoard) << "\n\n";
    std::cout << "Rook attack: \n" << BitBoard::RookAttack(testSquare, testBoard) << "\n\n";
    std::cout << "Queen attack: \n" << BitBoard::QueenAttack(testSquare, testBoard) << "\n\n";
    std::cout << "King attack: \n" << BitBoard::KingAttack(testSquare) << "\n\n";
#endif
}
