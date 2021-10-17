#include "Board.h"

#include <algorithm>

void Board::FromFEN(const std::string& fen) {
    m_Board.fill(None);
    //TODO: fill bitboards with zeros

    Square square = 0;

    for (const char c : fen) {
        if (std::isalpha(c)) {
            PlacePiece(CharToPiece(c), square);

            square++;
        } else if (std::isdigit(c)) {
            square += c - '0';
        } else if (c == '/') {
            if (square % 8 != 0)
                square = square - square % 8 + 8;  // Next line on chessboard
        } else {
            return;
        }
    }
}

std::string Board::ToFEN() { return ""; }

// void Board::Move(AlgebraicMove m) {}

void Board::Move(LongAlgebraicMove m) {
    Piece p = m_Board[m.FirstSquare];

    m_Board[m.FirstSquare] = None;
    m_Board[m.SecondSquare] = p;

    m_ColourBitBoards[ToColour(p)][m.FirstSquare] = false;
    m_ColourBitBoards[ToColour(p)][m.SecondSquare] = true;

    m_PieceBitBoards[ToPieceType(p)][m.FirstSquare] = false;
    m_PieceBitBoards[ToPieceType(p)][m.SecondSquare] = true;
}
