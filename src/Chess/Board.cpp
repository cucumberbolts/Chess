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

static constexpr std::array<BitBoard, PieceTypeCount> s_PieceBitBoards = {
    0b0000000011111111000000000000000000000000000000001111111100000000,  // Pawns
    0b0100001000000000000000000000000000000000000000000000000001000010,  // Knights
    0b0010010000000000000000000000000000000000000000000000000000100100,  // Bishops
    0b1000000100000000000000000000000000000000000000000000000010000001,  // Rooks
    0b0000100000000000000000000000000000000000000000000000000000001000,  // Queens
    0b0001000000000000000000000000000000000000000000000000000000010000   // Kings
};

static constexpr std::array<BitBoard, ColourCount> s_ColourBitBoards = {
    0b1111111111111111000000000000000000000000000000000000000000000000,  // White pieces
    0b0000000000000000000000000000000000000000000000001111111111111111   // Black pieces
};

static constexpr std::array<BitBoard, 4> s_CastlingPaths = {
    0x7000000000000000, 0x70, 0x1C00000000000000, 0x1C
};

void Board::Reset() {
    m_Board = s_StartBoard;
    m_PieceBitBoards = s_PieceBitBoards;
    m_ColourBitBoards = s_ColourBitBoards;

    m_PlayerTurn = White;
    m_CastlingPath = s_CastlingPaths;
    m_EnPassantSquare = 0;
}

void Board::FromFEN(const std::string& fen) {
    m_Board.fill(Piece::None);
    m_PieceBitBoards.fill(0);
    m_ColourBitBoards.fill(0);
    m_CastlingPath.fill(0xFFFFFFFFFFFFFFFF);

    StringParser fenParser(fen);

    std::string_view board;
    fenParser.Next(board);

    Square square = 0;
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
    m_PlayerTurn = playerTurn == "w" ? White : Black;

    std::string_view castlingRights;
    fenParser.Next(castlingRights);
    for (char c : castlingRights) {
        if (c == '-') break;
        if (c == 'K') m_CastlingPath[White | KingSide] =  s_CastlingPaths[White | KingSide];
        if (c == 'Q') m_CastlingPath[White | QueenSide] = s_CastlingPaths[White | QueenSide];
        if (c == 'k') m_CastlingPath[Black | KingSide] = s_CastlingPaths[Black | KingSide];
        if (c == 'q') m_CastlingPath[Black | QueenSide] = s_CastlingPaths[Black | QueenSide];
    }

    std::string_view enPassantSquare;
    fenParser.Next(enPassantSquare);
    if (enPassantSquare != "-")
        m_EnPassantSquare = ToSquare(enPassantSquare[0], enPassantSquare[1]);
}

// TODO: Implement this at some point
std::string Board::ToFEN() { return ""; }

AlgebraicMove Board::Move(LongAlgebraicMove m) {
    Piece piece = m_Board[m.SourceSquare];
    Colour colour = GetColour(piece);
    PieceType pieceType = GetPieceType(piece);

    // ASSERT(p != Piece::None)

    std::cout << "Moving: " << m << "\n";

    if (!IsMoveLegal(m))
        // Illegal move
        std::cout << "illegal move: " << m << "!\n"
            << GetPseudoLegalMoves(m.SourceSquare) << "\n"
            << GetLegalMoves(m.SourceSquare) << "\n"
            << BitBoard(m.DestinationSquare) << "\n";

    if (pieceType == King) {
        int direction = m.DestinationSquare - m.SourceSquare;  // Kingside or queenside

        if (abs(direction) == 2) {
            Square rookSquare, newRookSquare;

            if (direction < 0) {  // Queenside
                rookSquare = m.SourceSquare - 4;
                newRookSquare = m.DestinationSquare + 1;
            } else {
                rookSquare = m.SourceSquare + 3;
                newRookSquare = m.DestinationSquare - 1;
            }

            // TODO: Check if there is a rook on rookSquare

            // Only move the rook because the king will be moved below
            RemovePiece(rookSquare);
            PlacePiece(TypeAndColour(Rook, colour), newRookSquare);
        }

        m_CastlingPath[colour | KingSide] = 0xFFFFFFFFFFFFFFFF;
        m_CastlingPath[colour | QueenSide] = 0xFFFFFFFFFFFFFFFF;
    } else if (pieceType == Rook) {
        if (m.SourceSquare == 0)
            m_CastlingPath[Black | KingSide] = 0xFFFFFFFFFFFFFFFF;
        else if (m.SourceSquare == 7)
            m_CastlingPath[Black | QueenSide] = 0xFFFFFFFFFFFFFFFF;
        else if (m.SourceSquare == 56)
            m_CastlingPath[White | KingSide] = 0xFFFFFFFFFFFFFFFF;
        else if (m.SourceSquare == 63)
            m_CastlingPath[White | QueenSide] = 0xFFFFFFFFFFFFFFFF;
    } else if (pieceType == Pawn) {
        if (m.SourceSquare - m.DestinationSquare == 16)  // If white pushed pawn two squares
            m_EnPassantSquare = m.DestinationSquare + 8;
        else if (m.DestinationSquare - m.SourceSquare == 16)  // If white pushed pawn two squares
            m_EnPassantSquare = m.DestinationSquare - 8;
        else if (m.DestinationSquare == m_EnPassantSquare) {  // If taking en passant
            if (colour == White)
                RemovePiece(m.DestinationSquare + 8);
            else
                RemovePiece(m.DestinationSquare - 8);
        }
    }

    RemovePiece(m.SourceSquare);
    // We have to erase the piece from the bit boards before we capture it
    RemovePiece(m.DestinationSquare);
    PlacePiece(piece, m.DestinationSquare);

    m_PlayerTurn = OppositeColour(m_PlayerTurn);

    bool capture = m_Board[m.DestinationSquare] != Piece::None;

    return { piece, m.DestinationSquare, capture };
}

BitBoard Board::GetPseudoLegalMoves(Square piece) {
    PieceType pt = GetPieceType(m_Board[piece]);
    Colour c = GetColour(m_Board[piece]);

    if (c != m_PlayerTurn)
        return 0;

    BitBoard blockers = m_ColourBitBoards[White] | m_ColourBitBoards[Black];

    switch (pt) {
        case Pawn: return BitBoard::PawnMoves(piece, blockers, c, m_EnPassantSquare) & ~m_ColourBitBoards[c];
        case Knight: return BitBoard::KnightAttack(piece) & ~m_ColourBitBoards[c];
        case Bishop: return BitBoard::BishopAttack(piece, blockers) & ~m_ColourBitBoards[c];
        case Rook: return BitBoard::RookAttack(piece, blockers) & ~m_ColourBitBoards[c];
        case Queen: return BitBoard::QueenAttack(piece, blockers) & ~m_ColourBitBoards[c];
        case King: return BitBoard::KingAttack(piece) & ~m_ColourBitBoards[c];

        default: std::cout << "Invalid piece type!\n"; return 0;
    }
}

bool Board::IsMoveLegal(LongAlgebraicMove move) {
    return GetLegalMoves(move.SourceSquare) & BitBoard(1ull << move.DestinationSquare);
}

BitBoard Board::GetLegalMoves(Square piece) {
    // TODO: Calculate pins
    Colour playerColour = GetColour(m_Board[piece]);
    Colour enemyColour = OppositeColour(playerColour);

    BitBoard allPieces = m_ColourBitBoards[White] | m_ColourBitBoards[Black];
    BitBoard king = m_ColourBitBoards[playerColour] & m_PieceBitBoards[King];
    BitBoard enemyPieces = m_ColourBitBoards[OppositeColour(playerColour)];

    if (GetPieceType(m_Board[piece]) == King) {
        BitBoard legalMoves = GetPseudoLegalMoves(piece);
        BitBoard controlledSquares = ControlledSquares(enemyColour);
        // Deals with castling. Perhaps move to GetPseudoLegalMoves()?
        if (!((allPieces ^ king) & m_CastlingPath[playerColour | KingSide]) && !(controlledSquares & m_CastlingPath[playerColour | KingSide]))
            legalMoves |= 0x40ull << (playerColour == White ? 56 : 0);
        if (!((allPieces ^ king) & m_CastlingPath[playerColour | QueenSide]) && !(controlledSquares & m_CastlingPath[playerColour | QueenSide]))
            legalMoves |= 0x04ull << (playerColour == White ? 56 : 0);
        return legalMoves & ~controlledSquares;
    }

    Square kingSquare = BitBoard::GetSquare(king);

    BitBoard checkMask = 0;
    BitBoard checkers = 0;

    // Deals with checks from bishops, queens
    BitBoard bishopView = BitBoard::BishopAttack(kingSquare, allPieces) & enemyPieces;
    BitBoard bishopCheck = bishopView & (m_PieceBitBoards[Bishop] | m_PieceBitBoards[Queen]);
    checkers |= bishopCheck;
    if (bishopCheck)
        checkMask |= BitBoard::Line(kingSquare, BitBoard::GetSquare(bishopCheck));

    // Deals with checks from rooks, queens
    BitBoard rookView = BitBoard::RookAttack(kingSquare, allPieces) & enemyPieces;
    BitBoard rookCheck = rookView & (m_PieceBitBoards[Rook] | m_PieceBitBoards[Queen]);
    checkers |= rookCheck;
    if (rookCheck)
        checkMask |= BitBoard::Line(kingSquare, BitBoard::GetSquare(rookCheck));

    // Deals with checks from knights
    BitBoard knightCheck = BitBoard::KnightAttack(kingSquare) & enemyPieces & m_PieceBitBoards[Knight];
    checkers |= knightCheck;
    checkMask |= knightCheck;

    // Deals with checks from pawns
    BitBoard pawnCheck = BitBoard::PawnAttack(kingSquare, playerColour) & enemyPieces & m_PieceBitBoards[Pawn];
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
    BitBoard king = m_ColourBitBoards[OppositeColour(c)] & m_PieceBitBoards[King];
    BitBoard blockers = (m_ColourBitBoards[White] | m_ColourBitBoards[Black]) ^ king;

    BitBoard controlledSquares = 0;
    for (Square s = 0; s < 64; s++) {
        if (m_Board[s] != Piece::None && GetColour(m_Board[s]) == c) {
            switch (GetPieceType(m_Board[s])) {
                case Pawn:   controlledSquares |= BitBoard::PawnAttack(s, c); break;
                case Knight: controlledSquares |= BitBoard::KnightAttack(s) & ~m_ColourBitBoards[c]; break;
                case Bishop: controlledSquares |= BitBoard::BishopAttack(s, blockers) & ~m_ColourBitBoards[c]; break;
                case Rook:   controlledSquares |= BitBoard::RookAttack(s, blockers) & ~m_ColourBitBoards[c]; break;
                case Queen:  controlledSquares |= BitBoard::QueenAttack(s, blockers) & ~m_ColourBitBoards[c]; break;
                case King:   controlledSquares |= BitBoard::KingAttack(s) & ~m_ColourBitBoards[c]; break;

                default: std::cout << "Invalid piece type!\n"; return 0;
            }
        }
    }

    return controlledSquares;
}
