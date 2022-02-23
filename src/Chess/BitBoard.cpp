#include "BitBoard.h"

#include <intrin.h>

#pragma intrinsic(_BitScanForward)

BitBoard BitBoard::PawnMoves(Square square, BitBoard blockers, Colour colour) {
    // TODO: en passant
    BitBoard availibleSquares;

    if (colour == Colour::White) {
        if (((square - 8) & 0b11111000) == ((square - 9) & 0b11111000) && (square - 9) >= 0)
            availibleSquares[square - 9] = true & blockers[square - 9];
        if (((square - 8) & 0b11111000) == ((square - 7) & 0b11111000) && (square - 7) >= 0)
            availibleSquares[square - 7] = true & blockers[square - 7];
        // Calculates one square in front of the pawn
        if (square - 8 >= 0)
            availibleSquares[square - 8] = true ^ blockers[square - 8];
        // Calculates two squares in frot of the pawn for only the first push
        if ((1ull << square) & 0x00FF000000000000 && !blockers[square - 8])
            availibleSquares[square - 16] = true ^ blockers[square - 16];
    } else {
        if (((square + 8) & 0b11111000) == ((square + 9) & 0b11111000) && (square + 9) < 64)
            availibleSquares[square + 9] = true & blockers[square + 9];
        if (((square + 8) & 0b11111000) == ((square + 7) & 0b11111000) && (square + 7) < 64)
            availibleSquares[square + 7] = true & blockers[square + 7];
        // Calculates the square in front of the pawn
        if (square + 8 < 64)
            availibleSquares[square + 8] = true ^ blockers[square + 8];
        // Calculates two squares in frot of the pawn for only the first push
        if ((1ull << square) & 0x000000000000FF00 && !blockers[square + 8])
            availibleSquares[square + 16] = true ^ blockers[square + 16];
    }

    return availibleSquares;
}

BitBoard BitBoard::PawnAttack(Square square, BitBoard blockers, Colour colour) {
    // TODO: en passant
    BitBoard attackedSquares;

    if (colour == Colour::White) {
        if (((square - 8) & 0b11111000) == ((square - 9) & 0b11111000) && (square - 9) >= 0)
            attackedSquares[square - 9] = true;
        if (((square - 8) & 0b11111000) == ((square - 7) & 0b11111000) && (square - 7) >= 0)
            attackedSquares[square - 7] = true;
    } else {
        if (((square + 8) & 0b11111000) == ((square + 9) & 0b11111000) && (square + 9) < 64)
            attackedSquares[square + 9] = true;
        if (((square + 8) & 0b11111000) == ((square + 7) & 0b11111000) && (square + 7) < 64)
            attackedSquares[square + 7] = true;
    }

    return attackedSquares;
}

BitBoard BitBoard::KnightAttack(Square square) {
    constexpr size_t numSquares = 8;

    static constexpr int8_t s_KnightSquares[numSquares] = {
        -17, -15, -10, -6, 6, 10, 15, 17
    };

    static constexpr int8_t s_Rows[numSquares] = {
        -16, -16, -8, -8, 8, 8, 16, 16
    };

    BitBoard attackedSquares;

    for (size_t i = 0; i < numSquares; i++) {
        const int32_t knightSquare = square + s_KnightSquares[i];
        const int32_t squareRow = (square + s_Rows[i]) & 0b11111000;

        if ((knightSquare & 0b11111000) == squareRow && knightSquare < 64 && knightSquare > -1)
            attackedSquares[knightSquare] = true;
    }

    return attackedSquares;
}

BitBoard BitBoard::BishopAttack(Square square, BitBoard blockers) {
    blockers[square] = false;

    BitBoard attackedSquares;

    const Square distanceTop = (square - (square & 0b00000111)) / 8;
    const Square distanceBottom = ((square & 0b00000111) + 56 - square) / 8;
    const Square distanceLeft = square - (square & 0b11111000);
    const Square distanceRight = (square & 0b11111000) + 7 - square;

    for (int s = square; s > square - (std::min(distanceTop, distanceLeft) * 9) - 1; s -= 9) {
        attackedSquares[s] = true;
    
        if (blockers[s] == true)
            break;
    }
    
    for (int s = square; s < square + (std::min(distanceBottom, distanceRight) * 9) + 1; s += 9) {
        attackedSquares[s] = true;
    
        if (blockers[s] == true)
            break;
    }

    for (int s = square; s > square - (std::min(distanceTop, distanceRight) * 7) - 1; s -= 7) {
        attackedSquares[s] = true;
    
        if (blockers[s] == true)
            break;
    }
    
    for (int s = square; s < square + (std::min(distanceBottom, distanceLeft) * 7) + 1; s += 7) {
        attackedSquares[s] = true;
    
        if (blockers[s] == true)
            break;
    }

    attackedSquares[square] = false;

    return attackedSquares;
}

BitBoard BitBoard::RookAttack(Square square, BitBoard blockers) {
    blockers[square] = false;

    BitBoard attackedSquares;

    for (int s = square; s != (square & 0b11111000) - 1; s--) {
        attackedSquares[s] = true;
    
        if (blockers[s] == true)
            break;
    }

    for (int s = square; s < (square & 0b11111000) + 8; s++) {
        attackedSquares[s] = true;

        if (blockers[s] == true)
            break;
    }

    for (int s = square; s != (square & 0b00000111) - 8; s -= 8) {
        attackedSquares[s] = true;
    
        if (blockers[s] == true)
            break;
    }

    for (int s = square; s < (square & 0b00000111) + 64; s += 8) {
        attackedSquares[s] = true;

        if (blockers[s] == true)
            break;
    }

    attackedSquares[square] = false;

    return attackedSquares;
}

BitBoard BitBoard::QueenAttack(Square square, BitBoard blockers) {
    return BishopAttack(square, blockers) | RookAttack(square, blockers);
}

BitBoard BitBoard::KingAttack(Square square) {
    constexpr size_t numSquares = 8;

    static constexpr int8_t s_KingSquares[numSquares] = {
        -9, -8, -7, -1, 1, 7, 8, 9
    };

    static constexpr int8_t s_Rows[numSquares] = {
        -8, -8, -8, 0, 0, 8, 8, 8
    };

    BitBoard attackedSquares;

    for (size_t i = 0; i < numSquares; i++) {
        const int32_t kingSquare = square + s_KingSquares[i];
        const int32_t squareRow = (square + s_Rows[i] & 0b11111000);

        if ((kingSquare & 0b11111000) == squareRow && kingSquare < 64 && kingSquare > -1)
            attackedSquares[square + s_KingSquares[i]] = true;
    }

    return attackedSquares;
}

BitBoard BitBoard::Line(Square square1, Square square2) {
    Square min = std::min(square1, square2);
    Square max = std::max(square1, square2);

    BitBoard result = 0;

    if ((square1 & 0b11111000) == (square2 & 0b11111000)) {  // If the two squares are on the same rank
        for (Square i = min; i <= max; i++)
            result[i] = true;
    } else if ((square1 & 0b00000111) == (square2 & 0b00000111)) {  // If the two squares are on the same file
        for (Square i = min; i <= max; i += 8)
            result[i] = true;
    } else if ((square1 - square2) % 7 == 0) {  // Northwest to southeast
        for (Square i = min; i <= max; i += 7)
            result[i] = true;
    } else if ((square1 - square2) % 9 == 0) {  // Northeast to southwest
        for (Square i = min; i <= max; i += 9)
            result[i] = true;
    }

    return result;
}

Square BitBoard::GetSquare(BitBoard b) {
    unsigned long index;
    return _BitScanForward64(&index, b.m_Board) ? (Square)index : 0;
}
