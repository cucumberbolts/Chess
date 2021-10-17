#include <Chess/Engine.h>

#include <iostream>

#include <string_view>

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

    std::cout << s_Allocations << " allocations!\n";

    std::cin.get();
#endif

    using namespace std::literals;

    auto board = std::make_unique<Board>("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    std::cout << *board << '\n';

    std::cout << board->m_PieceBitBoards[Pawn] << "\n";
    std::cout << board->m_PieceBitBoards[Knight] << "\n";
    std::cout << board->m_PieceBitBoards[Bishop] << "\n";
    std::cout << board->m_PieceBitBoards[Rook] << "\n";
    std::cout << board->m_PieceBitBoards[Queen] << "\n";
    std::cout << board->m_PieceBitBoards[King] << "\n";

    std::cout << board->m_ColourBitBoards[White] << "\n";
    std::cout << board->m_ColourBitBoards[Black] << "\n";

    board->Move("e2e4"sv);

    std::cout << *board << '\n';

    std::cout << board->m_PieceBitBoards[Pawn] << "\n";
    std::cout << board->m_PieceBitBoards[Knight] << "\n";
    std::cout << board->m_PieceBitBoards[Bishop] << "\n";
    std::cout << board->m_PieceBitBoards[Rook] << "\n";
    std::cout << board->m_PieceBitBoards[Queen] << "\n";
    std::cout << board->m_PieceBitBoards[King] << "\n";

    std::cout << board->m_ColourBitBoards[White] << "\n";
    std::cout << board->m_ColourBitBoards[Black] << "\n";

    std::cin.get();
}
