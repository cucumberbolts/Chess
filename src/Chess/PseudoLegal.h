#pragma once

#include "BitBoard.h"

namespace PseudoLegal {

    BitBoard PawnAttack(Square square, Colour colour);
    BitBoard PawnMoves(Square square, Colour colour, BitBoard blockers, Square enPassant);

    BitBoard KnightAttack(Square square);
    BitBoard BishopAttack(Square square, BitBoard blockers);
    BitBoard RookAttack(Square square, BitBoard blockers);
    BitBoard QueenAttack(Square square, BitBoard blockers);
    BitBoard KingAttack(Square square);

    // A line (diagnol, vertical, or horizontal) between two squares
    BitBoard Line(BitBoard square1, BitBoard square2);

}
