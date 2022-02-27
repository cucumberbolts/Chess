#include "Move.h"

#include <iostream>

LongAlgebraicMove::LongAlgebraicMove(std::string_view longAlgebraic)
        : SourceSquare(0), DestinationSquare(0), Promotion(Pawn) {
    if (longAlgebraic.size() == 4) {
        SourceSquare = ToSquare(longAlgebraic[0], longAlgebraic[1]);
        DestinationSquare = ToSquare(longAlgebraic[2], longAlgebraic[3]);
    } else if (longAlgebraic.size() == 5) {
        SourceSquare = ToSquare(longAlgebraic[0], longAlgebraic[1]);
        DestinationSquare = ToSquare(longAlgebraic[2], longAlgebraic[3]);
        Promotion = CharToPieceType(longAlgebraic[4]);
    } else {
        std::cout << "invalid long algebraic notation!\n";

    }
}
