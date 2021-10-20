#include "Board.h"

#include "StringParser.h"

#include <algorithm>

static constexpr Piece s_BlackPieces[8] = {
    BlackRook, BlackKnight, BlackBishop, BlackQueen, BlackKing, BlackBishop, BlackKnight, BlackRook
};

static constexpr Piece s_WhitePieces[8] = {
    WhiteRook, WhiteKnight, WhiteBishop, WhiteQueen, WhiteKing, WhiteBishop, WhiteKnight, WhiteRook
};

static constexpr std::array<std::bitset<64>, PieceType::PieceTypeCount> s_PieceBitBoards = {
    0b0000000011111111000000000000000000000000000000001111111100000000,  // Pawns
    0b0100001000000000000000000000000000000000000000000000000001000010,  // Knights
    0b0010010000000000000000000000000000000000000000000000000000100100,  // Bishops
    0b1000000100000000000000000000000000000000000000000000000010000001,  // Rooks
    0b0000100000000000000000000000000000000000000000000000000000001000,  // Queens
    0b0001000000000000000000000000000000000000000000000000000000010000   // Kings
};

static constexpr std::array<std::bitset<64>, Colour::ColourCount> s_ColourBitBoards = {
    0b1111111111111111000000000000000000000000000000000000000000000000,  // White pieces
    0b0000000000000000000000000000000000000000000000001111111111111111   // Black pieces
};

Board::Board() {
    Reset();
}

void Board::Reset() {
    m_PieceBitBoards = s_PieceBitBoards;
    m_ColourBitBoards = s_ColourBitBoards;

    std::copy_n(s_BlackPieces, 8, m_Board.data());
    std::fill(std::next(m_Board.begin(), 8), std::next(m_Board.begin(), 16), BlackPawn);

    std::fill(std::next(m_Board.begin(), 16), std::next(m_Board.begin(), 48), None);
    
    std::fill(std::next(m_Board.begin(), 48), std::next(m_Board.begin(), 56), WhitePawn);
    std::copy_n(s_WhitePieces, 8, m_Board.data() + 56);
}

void Board::FromFEN(const std::string& fen) {
    m_Board.fill(None);
    std::fill(m_PieceBitBoards.begin(), m_PieceBitBoards.end(), 0);
    std::fill(m_ColourBitBoards.begin(), m_ColourBitBoards.end(), 0);

    Square square = 0;

    StringParser fenParser(fen);

    std::string_view board;
    fenParser.Next(board);

    for (const char c : board) {
        if (std::isalpha(c)) {
            PlacePiece(CharToPiece(c), square);

            square++;
        } else if (std::isdigit(c)) {
            square += c - '0';
        } else if (c == '/') {
            if (square % 8 != 0)
                square = square - square % 8 + 8;  // Next line on chessboard
        }
    }
}

std::string Board::ToFEN() { return ""; }

AlgebraicMove Board::Move(LongAlgebraicMove m) {
    Piece p = m_Board[m.SourceSquare];

    // ASSERT(p != None)

    bool capture = m_Board[m.DestinationSquare] != None;

    m_Board[m.SourceSquare] = None;
    m_Board[m.DestinationSquare] = p;

    Colour c = ToColour(p);
    PieceType t = ToPieceType(p);

    m_ColourBitBoards[ToColour(p)][m.SourceSquare] = false;
    m_ColourBitBoards[ToColour(p)][m.DestinationSquare] = true;

    m_PieceBitBoards[ToPieceType(p)][m.SourceSquare] = false;
    m_PieceBitBoards[ToPieceType(p)][m.DestinationSquare] = true;

    return { p, m.DestinationSquare, capture };
}
