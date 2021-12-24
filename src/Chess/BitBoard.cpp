#include "BitBoard.h"

BitBoard BitBoard::PawnAttack(Square square, BitBoard blockers, Colour colour) {
    // TODO: en passant
    BitBoard attackedSquares;

    if (colour == Colour::White) {
        if (((square - 8) & 0b11111000) == ((square - 9) & 0b11111000) && (square - 9) >= 0)
            attackedSquares[square - 9] = true & blockers[square - 9];
        if (((square - 8) & 0b11111000) == ((square - 7) & 0b11111000) && (square - 7) >= 0)
            attackedSquares[square - 7] = true & blockers[square - 7];
        // Calculates one square in front of the pawn
        if (square - 8 >= 0)
            attackedSquares[square - 8] = true ^ blockers[square - 8];
        // Calculates two squares in frot of the pawn for only the first push
        if ((1ull << square) & 0x00FF000000000000 && !blockers[square - 8])
            attackedSquares[square - 16] = true ^ blockers[square - 16];
    } else {
        if (((square + 8) & 0b11111000) == ((square + 9) & 0b11111000) && (square + 9) < 64)
            attackedSquares[square + 9] = true & blockers[square + 9];
        if (((square + 8) & 0b11111000) == ((square + 7) & 0b11111000) && (square + 7) < 64)
            attackedSquares[square + 7] = true & blockers[square + 7];
        // Calculates the square in front of the pawn
        if (square + 8 < 64)
            attackedSquares[square + 8] = true ^ blockers[square - 8];
        // Calculates two squares in frot of the pawn for only the first push
        if ((1ull << square) & 0x000000000000FF00 && !blockers[square + 8])
            attackedSquares[square + 16] = true ^ blockers[square + 16];
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
        const int32_t knightSquare = (square + s_KnightSquares[i] & 0b11111000);
        const int32_t squareRow = (square + s_Rows[i] & 0b11111000);

        if (knightSquare == squareRow && square + s_KnightSquares[i] < 64 && square + s_KnightSquares[i] > 0)
            attackedSquares[square + s_KnightSquares[i]] = true;
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
        const int32_t knightSquare = (square + s_KingSquares[i] & 0b11111000);
        const int32_t squareRow = (square + s_Rows[i] & 0b11111000);

        if (knightSquare == squareRow && square + s_KingSquares[i] < 64 && square + s_KingSquares[i] > 0)
            attackedSquares[square + s_KingSquares[i]] = true;
    }

    return attackedSquares;
}
