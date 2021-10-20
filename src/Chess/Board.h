#pragma once

#include <array>
#include <bitset>
#include <ostream>
#include <string>

#include "Move.h"

// TODO: implement castling in Board class

class Board {
private:
    using BitBoard = std::bitset<64>;
public:
    Board();
    Board(const std::string& fen) { FromFEN(fen); }

    void Reset(); // Set to starting position ("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")

    void FromFEN(const std::string& fen);
    std::string ToFEN();
    
    AlgebraicMove Move(LongAlgebraicMove m);

    friend std::ostream& operator<<(std::ostream& os, const Board& board);
private:
    BitBoard AllPieces() const { return m_ColourBitBoards[White] | m_ColourBitBoards[Black]; }

    void PlacePiece(Piece p, Square s);

    static Piece CharToPiece(char c);
private:
    std::array<BitBoard, ColourCount> m_ColourBitBoards;
    std::array<BitBoard, PieceTypeCount> m_PieceBitBoards;

    std::array<Piece, 64> m_Board;
};

inline void Board::PlacePiece(Piece p, Square s) {
    m_PieceBitBoards[ToPieceType(p)][s] = true;
    m_ColourBitBoards[ToColour(p)][s] = true;
    m_Board[s] = p;
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
    for (size_t rank = 0; rank < 8; rank++) {
        for (size_t file = 0; file < 8; file++) {
            if (board.m_Board[rank * 8 + file] == None)
                os << '.';
            else
                os << PieceToChar(board.m_Board[rank * 8 + file]);
        }

        os << '\n';
    }

    return os;
}
