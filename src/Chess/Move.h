#pragma once

#include <string_view>

#include <iostream>

enum Colour : uint8_t {
    White, Black,

    ColourCount = 2
};

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

    // TODO: move NO_CASTLE here?
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

inline char PieceTypeToChar(PieceType t) {
    constexpr static char s_PieceTypeToChar[] = "PNBRQK";

    return s_PieceTypeToChar[t];
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

// Uses this format: https://www.chessprogramming.org/Square_Mapping_Considerations#LittleEndianRankFileMapping
// Increases from left to right
using Square = uint8_t;

enum SquareValues : Square {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    SQUARE_MAX = 64, INVALID_SQUARE = 64
};

inline constexpr Square ToSquare(char file, char rank) {
    Square x = file - 'a';  // ASSERT(x < 8)
    Square y = (rank - 1) - '0';  // ASSERT(x < 8)
    return y * 8 + x;
}

// Returns the left-most square on the same rank as s
// TODO: Make this not-stupid
inline constexpr Square RankOf(Square s) { return s & 0b11111000; }
// Returns the bottom-most square on the same file as s
inline constexpr Square FileOf(Square s) { return s & 0b00000111; }

struct LongAlgebraicMove {
    Square SourceSquare = 0;
    Square DestinationSquare = 0;
    PieceType Promotion = Pawn;

    LongAlgebraicMove() = default;

    LongAlgebraicMove(Square a, Square b) : SourceSquare(a), DestinationSquare(b) {}

    LongAlgebraicMove(std::string_view longAlgebraic) {
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
};

inline std::ostream& operator<<(std::ostream& os, LongAlgebraicMove m) {
    os << (char)('a' + FileOf(m.SourceSquare));  // File
    os << (char)('1' + RankOf(m.SourceSquare) / 8);  // Rank

    os << (char)('a' + FileOf(m.DestinationSquare));  // File
    os << (char)('1' + RankOf(m.DestinationSquare) / 8);  // Rank

    return os;
}

enum MoveFlags : uint8_t {
    // To check for promotion, & with 0b111
    PromoteKnight = 0b001,
    PromoteBishop = 0b011,
    PromoteRook   = 0b100,
    PromoteQueen  = 0b101,

    CastleKingSide  = 1 << 3,
    CastleQueenSide = 1 << 4,

	Capture         = 1 << 5,
    Check           = 1 << 6,
    Checkmate       = 1 << 7,
};

// TODO: Think about making these bitfields
enum SpecifierFlags : uint8_t {
    File = 1 << 7,
    Rank = 1 << 6,
    FileAndRank = 0b11 << 6,

    RemoveSpecifierFlag = 0b00111111,
};

class Board;

struct AlgebraicMove {
    PieceType MovingPiece;
    Square Destination;

    // The specific rank or file the piece is from (in case there are 2) ex. Nbd7
    // If the file is needed, | it with 1 << 7
    // If the rank is needed, | it with 1 << 6
    // If both are needed (very rare), | it with 0b11 << 6
    Square Specifier;

    // Other information like check(mate), captures, castling, and promotion
    MoveFlags Flags;

    AlgebraicMove(PieceType movingPiece, Square destination, Square specifier, MoveFlags flags)
	    : MovingPiece(movingPiece), Destination(destination), Specifier(specifier), Flags(flags) {}

    AlgebraicMove(LongAlgebraicMove move, Board& board);
    AlgebraicMove(const std::string& str);

    std::string ToString();
};

inline std::ostream& operator<<(std::ostream& os, AlgebraicMove m) {
    os << m.ToString();

    return os;
}
