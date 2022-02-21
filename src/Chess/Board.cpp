#include "Board.h"

#include "Utility/StringParser.h"

#include <iostream>

#include <intrin.h>

static constexpr std::array<Piece, 64> s_StartBoard = {
    BlackRook, BlackKnight, BlackBishop, BlackQueen, BlackKing, BlackBishop, BlackKnight, BlackRook,
    BlackPawn, BlackPawn,   BlackPawn,   BlackPawn,  BlackPawn, BlackPawn,   BlackPawn,   BlackPawn,
    None,      None,        None,        None,       None,      None,        None,        None,
    None,      None,        None,        None,       None,      None,        None,        None,
    None,      None,        None,        None,       None,      None,        None,        None,
    None,      None,        None,        None,       None,      None,        None,        None,
    WhitePawn, WhitePawn,   WhitePawn,   WhitePawn,  WhitePawn, WhitePawn,   WhitePawn,   WhitePawn,
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

    m_PlayerTurn = Colour::White;
    m_CastlingRights = { true, true, true, true };
    m_EnPassantSquare = 0;
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

    std::string_view playerTurn;
    fenParser.Next(playerTurn);
    m_PlayerTurn = playerTurn == "w" ? Colour::White : Colour::Black;

    std::string_view castlingRights;
    fenParser.Next(castlingRights);
    for (char c : castlingRights) {
        if (c == '-') break;
        if (c == 'K') m_CastlingRights[Colour::White | CastleSide::KingSide] = true;
        if (c == 'Q') m_CastlingRights[Colour::White | CastleSide::QueenSide] = true;
        if (c == 'k') m_CastlingRights[Colour::Black | CastleSide::KingSide] = true;
        if (c == 'q') m_CastlingRights[Colour::Black | CastleSide::QueenSide] = true;
    }

    std::string_view enPassantSquare;
    fenParser.Next(enPassantSquare);
    if (enPassantSquare != "-")
        ToSquare(enPassantSquare[0], enPassantSquare[1]);
}

// TODO: Implement this at some point
std::string Board::ToFEN() { return ""; }

AlgebraicMove Board::Move(LongAlgebraicMove m) {
    Piece p = m_Board[m.SourceSquare];
    Colour c = GetColour(p);
    PieceType t = GetPieceType(p);

    // ASSERT(p != None)

    bool capture = m_Board[m.DestinationSquare] != Piece::None;

    // TODO: Put castling in BitBoard::KingAttack() and prune it if needed
    // Clean this castling code up somehow (it's kinda ugly)

    if (t == PieceType::King) {
        int direction = m.DestinationSquare - m.SourceSquare;  // Kingside or queenside

        bool isOnSameRow = (m.DestinationSquare & 0xF8) == (m.SourceSquare & 0xF8);  // Checks if king is moving laterally

        if (abs(direction) == 2 && isOnSameRow) {
            Square rookSquare, newRookSquare;
            CastleSide castleSide;

            if (direction < 0) {  // Queenside
                castleSide = CastleSide::QueenSide;
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
        m_CastlingRights[c | CastleSide::QueenSide] = false;
    } else if (t == PieceType::Rook) {
        if (m.SourceSquare == 0)
            m_CastlingRights[Colour::Black | CastleSide::KingSide] = false;
        else if (m.SourceSquare == 7)
            m_CastlingRights[Colour::Black | CastleSide::QueenSide] = false;
        else if (m.SourceSquare == 56)
            m_CastlingRights[Colour::White | CastleSide::KingSide] = false;
        else if (m.SourceSquare == 63)
            m_CastlingRights[Colour::White | CastleSide::QueenSide] = false;
    }

    RemovePiece(m.SourceSquare);
    PlacePiece(p, m.DestinationSquare);

    m_PlayerTurn = OppositeColour(m_PlayerTurn);

    return { p, m.DestinationSquare, capture };
}

bool Board::IsMovePseudoLegal(LongAlgebraicMove move) {
    Colour playerColour = GetColour(m_Board[move.SourceSquare]);

    if (playerColour != m_PlayerTurn)
        return false;

    return GetPseudoLegalMoves(move.SourceSquare) & BitBoard(1ull << move.DestinationSquare);
}

BitBoard Board::GetPseudoLegalMoves(Square piece) {
    PieceType pt = GetPieceType(m_Board[piece]);
    Colour c = GetColour(m_Board[piece]);

    BitBoard blockers = m_ColourBitBoards[White] | m_ColourBitBoards[Black];

    switch (pt) {
        case PieceType::Pawn: return BitBoard::PawnMoves(piece, blockers, c);
        case PieceType::Knight: return BitBoard::KnightAttack(piece) & ~m_ColourBitBoards[c];
        case PieceType::Bishop: return BitBoard::BishopAttack(piece, blockers) & ~m_ColourBitBoards[c];
        case PieceType::Rook: return BitBoard::RookAttack(piece, blockers) & ~m_ColourBitBoards[c];
        case PieceType::Queen: return BitBoard::QueenAttack(piece, blockers) & ~m_ColourBitBoards[c];
        case PieceType::King: return BitBoard::KingAttack(piece) & ~m_ColourBitBoards[c];

        default: std::cout << "Invalid piece type!\n"; return 0;
    }
}

bool Board::IsMoveLegal(LongAlgebraicMove move) {
    Colour playerColour = GetColour(m_Board[move.SourceSquare]);

    if (playerColour != m_PlayerTurn)
        return false;

    return GetPseudoLegalMoves(move.SourceSquare) & BitBoard(1ull << move.DestinationSquare);
}

BitBoard Board::GetLegalMoves(Square piece) {
    Colour playerColour = GetColour(m_Board[piece]);
    Colour enemyColour = OppositeColour(playerColour);

    BitBoard allPieces = m_ColourBitBoards[0] | m_ColourBitBoards[1];
    BitBoard enemyPieces = m_ColourBitBoards[OppositeColour(playerColour)];

    //std::cout << "Controlled squares:\n" << ControlledSquares(enemyColour) << "\n";
    
    if (GetPieceType(m_Board[piece]) == PieceType::King)
        return GetPseudoLegalMoves(piece) & ~ControlledSquares(enemyColour);

    Square kingSquare = BitBoard::GetSquare(m_ColourBitBoards[playerColour] & m_PieceBitBoards[PieceType::King]);

    BitBoard checkMask = 0;
    BitBoard checkers = 0;

    // Deals with checks from bishops, queens
    BitBoard bishopView = BitBoard::BishopAttack(kingSquare, allPieces) & enemyPieces;
    BitBoard bishopCheck = bishopView & (m_PieceBitBoards[PieceType::Bishop] | m_PieceBitBoards[PieceType::Queen]);
    checkers |= bishopCheck;
    checkMask |= BitBoard::Line(kingSquare, BitBoard::GetSquare(bishopCheck));

    // Deals with checks from rooks, queens
    BitBoard rookView = BitBoard::RookAttack(kingSquare, allPieces) & enemyPieces;
    BitBoard rookCheck = rookView & (m_PieceBitBoards[PieceType::Rook] | m_PieceBitBoards[PieceType::Queen]);
    checkers |= rookCheck;
    checkMask |= BitBoard::Line(kingSquare, BitBoard::GetSquare(rookCheck));

    // Deals with checks from knights
    BitBoard knightCheck = BitBoard::KnightAttack(kingSquare) & enemyPieces & m_PieceBitBoards[PieceType::Knight];
    checkers |= knightCheck;
    checkMask |= knightCheck;

    // Deals with checks from pawns
    BitBoard pawnCheck = BitBoard::PawnAttack(kingSquare, allPieces, playerColour) & enemyPieces & m_PieceBitBoards[PieceType::Pawn];
    checkers |= pawnCheck;
    checkMask |= pawnCheck;

    // If there are no checks, we don't prune any moves
    if (checkMask == 0)
        checkMask = 0xFFFFFFFFFFFFFFFF;
    
    // Get the number of checkers (using Kernighan's population count)
    int numCheckers = 0;
    for (; checkers != 0; numCheckers++)
        checkers &= checkers - 1;

    // If it is double check, we can only move the king
    if (numCheckers > 1)
        checkMask = 0;

    return GetPseudoLegalMoves(piece) & checkMask;
}


BitBoard Board::ControlledSquares(Colour c) {
    BitBoard king = m_ColourBitBoards[OppositeColour(c)] & m_PieceBitBoards[PieceType::King];
    BitBoard blockers = (m_ColourBitBoards[Colour::White] | m_ColourBitBoards[Colour::Black]) ^ king;

    BitBoard controlledSquares = 0;
    for (Square s = 0; s < 64; s++) {
        if (m_Board[s] != Piece::None && GetColour(m_Board[s]) == c) {
            switch (GetPieceType(m_Board[s])) {
                case PieceType::Pawn:   controlledSquares |= BitBoard::PawnAttack(s, blockers, c); break;
                case PieceType::Knight: controlledSquares |= BitBoard::KnightAttack(s) & ~m_ColourBitBoards[c]; break;
                case PieceType::Bishop: controlledSquares |= BitBoard::BishopAttack(s, blockers) & ~m_ColourBitBoards[c]; break;
                case PieceType::Rook:   controlledSquares |= BitBoard::RookAttack(s, blockers) & ~m_ColourBitBoards[c]; break;
                case PieceType::Queen:  controlledSquares |= BitBoard::QueenAttack(s, blockers) & ~m_ColourBitBoards[c]; break;
                case PieceType::King:   controlledSquares |= BitBoard::KingAttack(s) & ~m_ColourBitBoards[c]; break;

                default: std::cout << "Invalid piece type!\n"; return 0;
            }
        }
    }

    return controlledSquares;
}
