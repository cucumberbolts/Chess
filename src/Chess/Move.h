#pragma once

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

    PieceTypeCount = 7
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

using Square = uint8_t;

struct LongAlgebraicMove {
    Square FirstSquare;
    Square SecondSquare;

    // Piece Promotion;

    LongAlgebraicMove(std::string_view longAlgebraic);
};

struct AlgebraicMove {
    Piece Piece;
    Square Square;

    // Piece Promotion;
};
