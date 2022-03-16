#include <ChessEngine/Engine.h>

#include <iostream>

#include "Chess/BitBoard.h"
#include "Chess/KindergartenBitBoard.h"

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
#elif 1
    BitBoard blockers = 0;
    blockers |= 1ull << B4;
    blockers |= 1ull << H4;
    blockers |= 1ull << D6;
    blockers |= 1ull << D2;
    blockers |= 1ull << C7;
    blockers |= 1ull << E3;

    std::cout << "blockers:\n";
    PrintBitBoard(blockers);

    std::cout << "Pawns:\n";
    PrintBitBoard(PseudoLegal::PawnAttack(E5, White));
    PrintBitBoard(PseudoLegal::PawnMoves(E5, White, blockers, 0));
    PrintBitBoard(PseudoLegal::PawnMoves(E7, Black, blockers, 0));
    PrintBitBoard(PseudoLegal::PawnMoves(E2, White, blockers, 0));
    PrintBitBoard(PseudoLegal::PawnMoves(F2, White, blockers, 0));

    std::cout << "Piece attacks:\n";
    PrintBitBoard(PseudoLegal::KnightAttack(F4));
    PrintBitBoard(PseudoLegal::KnightAttack(F4));
    PrintBitBoard(PseudoLegal::BishopAttack(F4, blockers));
    PrintBitBoard(PseudoLegal::RookAttack(C3, blockers));
    PrintBitBoard(PseudoLegal::QueenAttack(C3, blockers));

    std::cout << "Lines:\n";
    PrintBitBoard(PseudoLegal::Line(1ull << B2, 1ull << E5));
    PrintBitBoard(PseudoLegal::Line(1ull << C1, 1ull << C7));
    PrintBitBoard(PseudoLegal::Line(1ull << A8, 1ull << B7));
    PrintBitBoard(PseudoLegal::Line(1ull << F7, 1ull << H7));
    PrintBitBoard(PseudoLegal::Line(1ull << A1, 1ull << D6));
    PrintBitBoard(PseudoLegal::Line(1ull << F7, 0));
#elif 1
    Board board("b6k/8/8/3R4/1r1NK3/8/8/8 w - - 0 1");

    std::cout << board << "\n";

    PrintBitBoard(board.GetLegalMoves(ToSquare('d', '5')));
    PrintBitBoard(board.GetLegalMoves(ToSquare('d', '4')));
#endif
}
