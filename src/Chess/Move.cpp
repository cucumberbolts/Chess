#include "Move.h"

#include <iostream>

LongAlgebraicMove::LongAlgebraicMove(std::string_view longAlgebraic) : SourceSquare(0), DestinationSquare(0) {
    if (longAlgebraic.size() < 4 || longAlgebraic.size() > 5) {
        std::cout << "invalid long algebraic notation!\n";
    } else {
        SourceSquare = ToSquare(longAlgebraic[0], longAlgebraic[1]);
        DestinationSquare = ToSquare(longAlgebraic[2], longAlgebraic[3]);

        // Deal with promotion
    }
}
