#pragma once

#include "BitBoard.h"

namespace KindergartenBitBoard {
    BitBoard BishopAttack(Square square, BitBoard blockers);
    BitBoard RookAttack(Square square, BitBoard blockers);
    BitBoard QueenAttack(Square square, BitBoard blockers);
}
