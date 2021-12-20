#include "Board.h"

#include "Utility/StringParser.h"

#include <iostream>

static constexpr std::array<Piece, 64> s_StartBoard = {
    BlackRook, BlackKnight, BlackBishop, BlackQueen, BlackKing, BlackBishop, BlackKnight, BlackRook,
    None,      None,        None,        None,       None,      None,        None,        None,
    None,      None,        None,        None,       None,      None,        None,        None,
    None,      None,        None,        None,       None,      None,        None,        None,
    None,      None,        None,        None,       None,      None,        None,        None,
    None,      None,        None,        None,       None,      None,        None,        None,
    None,      None,        None,        None,       None,      None,        None,        None,
    WhiteRook, WhiteKnight, WhiteBishop, WhiteQueen, WhiteKing, WhiteBishop, WhiteKnight, WhiteRook
};

static constexpr std::array<BitBoard, PieceType::PieceTypeCount> s_PieceBitBoards = {
    0b0000000011111111000000000000000000000000000000001111111100000000,  // Pawns
    0b0100001000000000000000000000000000000000000000000000000001000010,  // Knights
    0b0010010000000000000000000000000000000000000000000000000000100100,  // Bishops
    0b1000000100000000000000000000000000000000000000000000000010000001,  // Rooks
    0b0000100000000000000000000000000000000000000000000000000000001000,  // Queens
    0b0001000000000000000000000000000000000000000000000000000000010000   // Kings
};

static constexpr std::array<BitBoard, Colour::ColourCount> s_ColourBitBoards = {
    0b1111111111111111000000000000000000000000000000000000000000000000,  // White pieces
    0b0000000000000000000000000000000000000000000000001111111111111111   // Black pieces
};

Board::Board() {
    Reset();
}

void Board::Reset() {
    m_Board = s_StartBoard;
    m_PieceBitBoards = s_PieceBitBoards;
    m_ColourBitBoards = s_ColourBitBoards;

    m_CastlingRights = { true, true, true, true };
}

void Board::FromFEN(const std::string& fen) {
    m_Board.fill(None);
    m_PieceBitBoards.fill(0);
    m_ColourBitBoards.fill(0);
    m_CastlingRights.fill(false);

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

// TODO: Implement this at some point
std::string Board::ToFEN() { return ""; }

AlgebraicMove Board::Move(LongAlgebraicMove m) {
    Piece p = m_Board[m.SourceSquare];
    Colour c = GetColour(p);
    PieceType t = GetPieceType(p);

    // ASSERT(p != None)

    bool capture = m_Board[m.DestinationSquare] != Piece::None;

    if (t == PieceType::King) {
        int direction = m.DestinationSquare - m.SourceSquare;  // Kingside or queenside

        bool isOnSameRow = (m.DestinationSquare & 0xF8) == (m.SourceSquare & 0xF8);  // Checks if king is moving laterally

        if (abs(direction) == 2 && isOnSameRow) {
            Square rookSquare, newRookSquare;
            CastleSide castleSide;

            if (direction < 0) {  // Queenside
                castleSide = CastleSide::Queenside;
                rookSquare = m.SourceSquare - 4;
                newRookSquare = m.DestinationSquare + 1;
            } else {
                castleSide = CastleSide::KingSide;
                rookSquare = m.SourceSquare + 3;
                newRookSquare = m.DestinationSquare - 1;
            }
    
            // Check for castling rights
            if (m_CastlingRights[c | castleSide] == false)
                std::cout << "Castling is illegal!\n";
    
            // Check if castling is legal
            if (GetPieceType(m_Board[rookSquare]) != PieceType::Rook)
                std::cout << "Castling is illegal!\n";

            // TODO: check if move is legal (include checks and stuff)
    
            // Only move the rook because the king will be moved below
            RemovePiece(rookSquare);
            PlacePiece(TypeAndColour(PieceType::Rook, c), newRookSquare);
        }

        m_CastlingRights[c | CastleSide::KingSide] = false;
        m_CastlingRights[c | CastleSide::Queenside] = false;
    } else if (t == PieceType::Rook) {
        if (m.SourceSquare == 0 && c == Colour::Black)
            m_CastlingRights[0] = false;
        else if (m.SourceSquare == 7 && c == Colour::Black)
            m_CastlingRights[1] = false;
        else if (m.SourceSquare == 56 && c == Colour::White)
            m_CastlingRights[2] = false;
        else if (m.SourceSquare == 63 && c == Colour::White)
            m_CastlingRights[3] = false;
    }

    RemovePiece(m.SourceSquare);
    PlacePiece(p, m.DestinationSquare);

    return { p, m.DestinationSquare, capture };
}

bool Board::IsMoveLegal(LongAlgebraicMove move) {
    Colour playerColour = GetColour(m_Board[move.SourceSquare]);

    // Cannot capture your own piece
    bool validDestination = (~m_ColourBitBoards[playerColour])[move.DestinationSquare];

    if (!validDestination)
        return false;

    return true;
}
