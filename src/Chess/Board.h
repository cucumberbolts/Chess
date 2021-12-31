#pragma once

#include <array>
#include <ostream>
#include <string>

#include "BitBoard.h"
#include "BoardFormat.h"
#include "Move.h"

// TODO: Error checking with ASSERT()s

class Board {
public:
    Board();
    Board(const std::string& fen) { FromFEN(fen); }

    void Reset(); // Set to starting position ("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")

    void FromFEN(const std::string& fen);
    std::string ToFEN();

    friend std::ostream& operator<<(std::ostream& os, const Board& board);

    AlgebraicMove Move(LongAlgebraicMove m);

    bool IsMovePseudoLegal(LongAlgebraicMove move);
    BitBoard GetPseudoLegalMoves(Square piece);
private:
    void PlacePiece(Piece p, Square s);
    void RemovePiece(Square s);

    static Piece CharToPiece(char c);
private:
    std::array<BitBoard, ColourCount> m_ColourBitBoards;
    std::array<BitBoard, PieceTypeCount> m_PieceBitBoards;

    std::array<Piece, 64> m_Board;

    Colour m_PlayerTurn = Colour::White;

    std::array<bool, 4> m_CastlingRights = { true, true, true, true };

    Square m_EnPassantSquare = 0;
};

inline void Board::PlacePiece(Piece p, Square s) {
    m_PieceBitBoards[GetPieceType(p)][s] = true;
    m_ColourBitBoards[GetColour(p)][s] = true;
    m_Board[s] = p;
}

inline void Board::RemovePiece(Square s) {
    Piece p = m_Board[s];
    m_PieceBitBoards[GetPieceType(p)][s] = false;
    m_ColourBitBoards[GetColour(p)][s] = false;
    m_Board[s] = Piece::None;
}


inline Piece Board::CharToPiece(char c) {
    switch (c) {
        case 'P': return WhitePawn;
        case 'N': return WhiteKnight;
        case 'B': return WhiteBishop;
        case 'R': return WhiteRook;
        case 'Q': return WhiteQueen;
        case 'K': return WhiteKing;

        case 'p': return BlackPawn;
        case 'n': return BlackKnight;
        case 'b': return BlackBishop;
        case 'r': return BlackRook;
        case 'q': return BlackQueen;
        case 'k': return BlackKing;

        default: return None;  // Error
    }
}

inline std::ostream& operator<<(std::ostream& os, const Board& board) {
    static std::array<std::string_view, Colour::ColourCount> rankNumbers = { "87654321", "12345678" };

    for (Square rank = 0; rank < 8; rank++) {
        if (BoardFormat::s_BoardFormat.Coordinates)
            os << rankNumbers[BoardFormat::s_BoardFormat.Orientation][rank] << ' ';

        for (Square file = 0; file < 8; file++) {
            Square square = (BoardFormat::s_BoardFormat.Orientation == Colour::White) ? (rank * 8 + file) : (63 - (rank * 8 + file));
            if (board.m_Board[square] == None)
                os << '.';
            else
                os << PieceToChar(board.m_Board[square]);
        }

        os << '\n';
    }

    if (BoardFormat::s_BoardFormat.Coordinates)
        os << (BoardFormat::s_BoardFormat.Orientation == Colour::White ? "  abcdefgh\n" : "  hgfedcba\n");

    return os;
}
