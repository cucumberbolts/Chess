#pragma once

#include <iostream>

#include "BoardFormat.h"

using BitBoard = uint64_t;

#if defined(_WIN32)
#include <intrin.h>

// Returns least significant bit on bitboard
// Returns 0 if board is 0
inline Square GetSquare(BitBoard board) {
    unsigned long index;
    _BitScanForward64(&index, board);
    return static_cast<Square>(index) * (board != 0);
}

// Gets the number of bits set
inline uint64_t SquareCount(BitBoard board) {
    return __popcnt64(board);
}
#else
// Returns least significant bit on bitboard
// Returns 0 if board is 0
inline Square GetSquare(BitBoard board) {
    // Isolate the lowest bit in 'board'
    board = board & (~board + 1);

    Square index = 0;
    if ((board & 0xffffffff00000000) != 0) index += 32;
    if ((board & 0xffff0000ffff0000) != 0) index += 16;
    if ((board & 0xff00ff00ff00ff00) != 0) index += 8;
    if ((board & 0xf0f0f0f0f0f0f0f0) != 0) index += 4;
    if ((board & 0xcccccccccccccccc) != 0) index += 2;
    if ((board & 0xaaaaaaaaaaaaaaaa) != 0) index += 1;

    return index;

    /*
     *Apparently that's faster than this?????:
     *
     *unsigned long index;
     *_BitScanForward64(&index, board);
     *return static_cast<Square>(index) * (board != 0);
    */
}

// Gets the number of bits set
inline uint64_t SquareCount(BitBoard board) {
    size_t count = 0;
    for (; board != 0; count++)
        board = board & (board - 1);
	return count;
}
#endif

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
