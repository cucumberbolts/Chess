#include "Move.h"

#include <iostream>

LongAlgebraicMove::LongAlgebraicMove(std::string_view longAlgebraic) : FirstSquare(0), SecondSquare(0) {
    if (longAlgebraic.size() < 4 || longAlgebraic.size() > 5) {
        std::cout << "invalid long algebraic notation!\n";
    } else {
        uint32_t x = longAlgebraic[0] - 'a';  // ASSERT(x < 8)
        uint32_t y = 8 - (longAlgebraic[1] - '0'); // ASSERT(x < 8)
        FirstSquare = y * 8 + x;

        x = longAlgebraic[2] - 'a';  // ASSERT(x < 8)
        y = 8 - (longAlgebraic[3] - '0'); // ASSERT(x < 8)
        SecondSquare = y * 8 + x;

        // Deal with promotion
    }
}
