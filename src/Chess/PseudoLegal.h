#pragma once

#include "BitBoard.h"

namespace PseudoLegal {

    /**
     * \brief returns the two capture squares of a pawn
     * \param square the square the pawn is on
     * \param colour the colour of the pawn
     * \return BitBoard of the attacked squares
     */
    BitBoard PawnAttack(Square square, Colour colour);

    /**
     * \brief gets all the pawn moves (captures, forward moves, en-passant)
     * \param square the square of the pawn
     * \param colour colour of the pawn
     * \param blockers blockers (pieces in front of the pawn the prevent it from moving forward)
     * \param enPassant the en-passant square
     * \return BitBoard of the pseudolegal moves for the pawn
     */
    BitBoard PawnMoves(Square square, Colour colour, BitBoard blockers, Square enPassant);

    // Note: The bitboards returned include the blockers

    BitBoard KnightAttack(Square square);
    BitBoard BishopAttack(Square square, BitBoard blockers);
    BitBoard RookAttack(Square square, BitBoard blockers);
    BitBoard QueenAttack(Square square, BitBoard blockers);
    BitBoard KingAttack(Square square);

    // A line (diagonal, vertical, or horizontal) between two squares
    BitBoard Line(BitBoard square1, BitBoard square2);

}
