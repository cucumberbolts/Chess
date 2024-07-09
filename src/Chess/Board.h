#pragma once

#include <array>
#include <ostream>
#include <string>

#include "BitBoard.h"
#include "BoardFormat.h"
#include "Move.h"

class Game;
struct GameMove;

class Board {
    friend class Game;
public:
    Board() { Reset(); }
    Board(const std::string& fen) { FromFEN(fen); }

    void Reset(); // Set to starting position ("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")

    void FromFEN(const std::string& fen);
    std::string ToFEN() const;
    
    inline Piece operator[](Square s) const { return m_Board[s]; }

    inline Square GetEnPassantSquare() const { return m_EnPassantSquare; }
    inline Colour GetPlayerTurn() const { return m_PlayerTurn; }
    inline int32_t GetHalfMoves() const { return m_HalfMoves; }
    inline int32_t GetFullMoves() const { return m_FullMoves; }

    AlgebraicMove Move(LongAlgebraicMove m);
    LongAlgebraicMove Move(AlgebraicMove m);
    void UndoMove(const GameMove& m);

    inline bool IsMoveLegal(LongAlgebraicMove m) { return GetPieceLegalMoves(m.SourceSquare) & (1ull << m.DestinationSquare); }

    bool HasLegalMoves(Colour colour);
    BitBoard GetPieceLegalMoves(Square piece);

    inline static const char* START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\0";
private:
    BitBoard GetPseudoLegalMoves(Square piece) const;

    void PlacePiece(Piece p, Square s);
    void RemovePiece(Square s);

    BitBoard ControlledSquares(Colour colour) const;
private:
    std::array<BitBoard, ColourCount> m_ColourBitBoards;
    std::array<BitBoard, PieceTypeCount> m_PieceBitBoards;

    std::array<Piece, 64> m_Board;

    // It is the path from the king to the rook when castling (including the king square)
    // AND the path with blockers and attackedSquares to see if castling is legal
    // If you are not allowed to castle, then the path will be 0xFFFFFFFFFFFFFFFF
    //
    // OR the Colour enum with the CastleSide enum to get the index
    // [0] = White | KingSide
    // [1] = Black | KingSide
    // [2] = White | QueenSide
    // [3] = Black | QueenSide
    //
    // Explanation:
    // If you AND this with the check mask
    // and the result is not 0
    // the the king can't castle
    std::array<BitBoard, 4> m_CastlingPath;

    // The target square for en passant
    Square m_EnPassantSquare;
    
    Colour m_PlayerTurn;
    
    int32_t m_HalfMoves = 0;  // Number of half moves since the last pawn move or capture
    int32_t m_FullMoves = 1;  // The number of the full moves; it starts at 1, and is incremented after Black's move
};

inline void Board::PlacePiece(Piece p, Square s) {
    m_PieceBitBoards[GetPieceType(p)] |= 1ull << s;
    m_ColourBitBoards[GetColour(p)] |= 1ull << s;
    m_Board[s] = p;
}

inline void Board::RemovePiece(Square s) {
    Piece p = m_Board[s];
    if (p != Piece::None) {
        m_PieceBitBoards[GetPieceType(p)] &= ~(1ull << s);
        m_ColourBitBoards[GetColour(p)] &= ~(1ull << s);
        m_Board[s] = Piece::None;
    }
}

inline std::ostream& operator<<(std::ostream& os, const Board& board) {
    static std::array<std::string_view, ColourCount> rankNumbers = { "12345678", "87654321" };

    for (Square rank = 7; rank < 8; rank--) {
        if (BoardFormat::s_BoardFormat.Coordinates)
            os << rankNumbers[BoardFormat::s_BoardFormat.Orientation][rank] << ' ';

        for (Square file = 0; file < 8; file++) {
            Square square = (BoardFormat::s_BoardFormat.Orientation == White) ? (rank * 8 + file) : (63 - (rank * 8 + file));
            if (board[square] == Piece::None)
                os << '.';
            else
                os << PieceToChar(board[square]);
        }

        os << '\n';
    }

    if (BoardFormat::s_BoardFormat.Coordinates)
        os << (BoardFormat::s_BoardFormat.Orientation == White ? "  abcdefgh\n" : "  hgfedcba\n");

    return os;
}
