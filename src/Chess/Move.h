#pragma once

#include <ostream>
#include <string_view>

enum Colour : uint8_t {
    White, Black,

    ColourCount = 2
};

// Mainly used for bitboards
enum PieceType : uint8_t {
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King,

    PieceTypeCount = 6
};

enum Piece : uint8_t {
    WhitePawn,
    WhiteKnight,
    WhiteBishop,
    WhiteRook,
    WhiteQueen,
    WhiteKing,
    BlackPawn = 8,
    BlackKnight,
    BlackBishop,
    BlackRook,
    BlackQueen,
    BlackKing,

    None,  // Used in board for empty square
    //PieceCount = 13,
};

constexpr PieceType ToPieceType(Piece p) { return (PieceType)(p & 0b0111); }
constexpr Colour ToColour(Piece p) { return (Colour)((p & 0b1000) >> 3); }

inline char PieceToChar(Piece p) {
    constexpr static char s_PieceToChar[] = "PNBRQK  pnbrqk";

    return s_PieceToChar[p];
}

using Square = uint8_t;

struct LongAlgebraicMove {
    Square SourceSquare;
    Square DestinationSquare;

    // Piece Promotion;

    LongAlgebraicMove() = default;
    LongAlgebraicMove(std::string_view longAlgebraic);
};

struct AlgebraicMove {
    Piece Piece;
    Square Square;

    // bool Specifier;
    bool Capture;

    // Piece Promotion;
};

inline std::ostream& operator<<(std::ostream& os, AlgebraicMove m) {
    if (ToPieceType(m.Piece) != Pawn)
        os << PieceToChar(m.Piece);

    if (m.Capture)
        os << 'x';

    os << (char)('a' + m.Square % 8) << 8 - (m.Square / 8);

    return os;
}
