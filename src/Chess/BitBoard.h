#pragma once

#include <iostream>

#include "BoardFormat.h"

using BitBoard = uint64_t;

// For debugging
inline void PrintBitBoard(BitBoard board) {
    static std::string_view rankNumbers[ColourCount] = { "12345678", "87654321" };

    for (Square rank = 7; rank < 8; rank--) {
        if (BoardFormat::s_BoardFormat.Coordinates)
            std::cout << rankNumbers[BoardFormat::s_BoardFormat.Orientation][rank] << ' ';

        for (Square file = 0; file < 8; file++) {
            Square square = (BoardFormat::s_BoardFormat.Orientation == White) ? (rank * 8 + file) : (63 - (rank * 8 + file));
            std::cout << ((board & (1ull << square)) >> square);
        }

        std::cout << '\n';
    }

    if (BoardFormat::s_BoardFormat.Coordinates)
        std::cout << (BoardFormat::s_BoardFormat.Orientation == White ? "  abcdefgh\n" : "  hgfedcba\n");

    std::cout << "\n";
}
