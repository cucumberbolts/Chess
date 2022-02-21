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
#elif 0
    Board board;

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

    std::cout << BoardFormat::OrientationBlack << board << "\n\n";
    board.Move(LongAlgebraicMove("e7e5"));
    std::cout << BoardFormat::OrientationWhite << BoardFormat::NoBoardCoordinates << board << "\n\n";

    std::cout << board.GetPseudoLegalMoves(ToSquare('f', '1')) << "\n";
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
