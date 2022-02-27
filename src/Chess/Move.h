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

inline PieceType CharToPieceType(char piece) {
    switch ((char)std::tolower(piece)) {
        case 'p': return Pawn;
        case 'n': return Knight;
        case 'b': return Bishop;
        case 'r': return Rook;
        case 'q': return Queen;
        case 'k': return King;
        //default: std::cout << "Invalid 'piece'\n";
    }
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

// Returns the left-most square on the same rank as s
inline constexpr Square RankOf(Square s) { return s & 0b11111000; }
// Returns the right-most square on the same rank as s
inline constexpr Square FileOf(Square s) { return s & 0b00000111; }

struct LongAlgebraicMove {
    Square SourceSquare;
    Square DestinationSquare;
    PieceType Promotion;

    LongAlgebraicMove() = default;
    LongAlgebraicMove(std::string_view longAlgebraic);
};

inline std::ostream& operator<<(std::ostream& os, LongAlgebraicMove m) {
    os << (char)('a' + FileOf(m.SourceSquare));  // File
    os << (char)('8' - RankOf(m.SourceSquare) / 8);  // Rank

    os << (char)('a' + FileOf(m.DestinationSquare));  // File
    os << (char)('8' - RankOf(m.DestinationSquare) / 8);  // Rank

    return os;
}

struct AlgebraicMove {
    Piece Piece;
    Square Square;

    char Specifier;  // the specific rank or file the piece is from (in case there are 2) ex. n*b*d7
    bool Capture;

    // Piece Promotion;
};

inline std::ostream& operator<<(std::ostream& os, AlgebraicMove m) {
    if (GetPieceType(m.Piece) != Pawn)
        os << (char)std::tolower(PieceToChar(m.Piece));

    if (m.Specifier)
        os << m.Specifier;

    if (m.Capture)
        os << 'x';

    os << (char)('a' + FileOf(m.Square));  // File
    os << (char)('8' - RankOf(m.Square) / 8);  // Rank

    return os;
}
