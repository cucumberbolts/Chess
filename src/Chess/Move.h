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

// ORed with Colour enum to get the index to the m_CastlingRights array in the Board class
enum CastleSide : uint8_t {
    KingSide = 0b00,
    QueenSide = 0b10,
};

// Returns the PieceType of the given Piece
constexpr inline PieceType GetPieceType(Piece p) { return (PieceType)(p & 0b0111); }

// Returns the Colour of the given Piece
constexpr inline Colour GetColour(Piece p) { return (Colour)((p & 0b1000) >> 3); }

// Returns the opposite colour
constexpr inline Colour OppositeColour(Colour c) { return (Colour)(1 - c); }

// Returns the Piece of the given PieceType and Colour
constexpr inline Piece TypeAndColour(PieceType t, Colour c) { return (Piece)(t | (c << 3)); }

inline char PieceToChar(Piece p) {
    constexpr static char s_PieceToChar[] = "PNBRQK  pnbrqk";

    return s_PieceToChar[p];
}

using Square = uint8_t;

inline constexpr Square ToSquare(char file, char rank) {
    uint8_t x = file - 'a';  // ASSERT(x < 8)
    uint8_t y = 8 - (rank - '0'); // ASSERT(x < 8)
    return y * 8 + x;
}

inline constexpr Square ToSquare(std::string_view square) {
    // ASSERT
    if (square.size() > 2)
        return 0;

    uint8_t x = square[0] - 'a';  // ASSERT(x < 8)
    uint8_t y = 8 - (square[1] - '0'); // ASSERT(x < 8)
    return y * 8 + x;
}

struct LongAlgebraicMove {
    Square SourceSquare;
    Square DestinationSquare;

    LongAlgebraicMove() = default;
    LongAlgebraicMove(std::string_view longAlgebraic);
};

inline std::ostream& operator<<(std::ostream& os, LongAlgebraicMove m) {
    os << (char)('a' + m.SourceSquare % 8);  // File
    os << 8 - m.SourceSquare / 8;  // Rank

    os << (char)('a' + m.DestinationSquare % 8);  // File
    os << 8 - m.DestinationSquare / 8;  // Rank

    return os;
}

struct AlgebraicMove {
    Piece Piece;
    Square Square;

    // char Specifier;
    bool Capture;

    // Piece Promotion;
};

inline std::ostream& operator<<(std::ostream& os, AlgebraicMove m) {
    if (GetPieceType(m.Piece) != Pawn)
        os << (char)std::tolower(PieceToChar(m.Piece));

    if (m.Capture)
        os << 'x';

    os << (char)('a' + m.Square % 8);  // File
    os << 8 - m.Square / 8;  // Rank

    return os;
}
