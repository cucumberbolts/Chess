#pragma once

#include <iostream>

#include "BoardFormat.h"

#include <intrin.h>

using BitBoard = uint64_t;

// Returns least significant bit on bitboard
inline Square GetSquare(BitBoard board) {
    unsigned long index;
    return _BitScanForward64(&index, board) ? (Square)index : 0;
}

// Returns a BitBoard highlighting the file of the given square
inline BitBoard BitBoardFile(Square s) {
    return 0x0101010101010101 << (s & 0b00000111);  // Shifts the 'a' file to the file of the square
}

// Returns a BitBoard highlighting the rank of the given square
inline BitBoard BitBoardRank(Square s) {
    return 0x00000000000000FF << (s & 0b11111000);  // Shifts the first rank to the rank of the square
}

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
