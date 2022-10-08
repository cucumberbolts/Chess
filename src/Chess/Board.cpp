#include "Board.h"

#include "PseudoLegal.h"

#include "Utility/StringParser.h"

#include <iostream>
#include <sstream>

static constexpr std::array<Piece, 64> s_StartBoard = {
    WhiteRook, WhiteKnight, WhiteBishop, WhiteQueen, WhiteKing, WhiteBishop, WhiteKnight, WhiteRook,
    WhitePawn, WhitePawn,   WhitePawn,   WhitePawn,  WhitePawn, WhitePawn,   WhitePawn,   WhitePawn,
    None,      None,        None,        None,       None,      None,        None,        None,
    None,      None,        None,        None,       None,      None,        None,        None,
    None,      None,        None,        None,       None,      None,        None,        None,
    None,      None,        None,        None,       None,      None,        None,        None,
    BlackPawn, BlackPawn,   BlackPawn,   BlackPawn,  BlackPawn, BlackPawn,   BlackPawn,   BlackPawn,
    BlackRook, BlackKnight, BlackBishop, BlackQueen, BlackKing, BlackBishop, BlackKnight, BlackRook
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
    0b0000000000000000000000000000000000000000000000001111111111111111,  // White Pieces
    0b1111111111111111000000000000000000000000000000000000000000000000   // Black pieces
};

static constexpr std::array<BitBoard, 4> s_CastlingPaths = {
	0x70, 0x70ull << 56, 0xE, 0xEull << 56
};

void Board::Reset() {
    m_Board = s_StartBoard;
    m_PieceBitBoards = s_PieceBitBoards;
    m_ColourBitBoards = s_ColourBitBoards;

    m_PlayerTurn = White;
    m_CastlingPath = s_CastlingPaths;
    m_EnPassantSquare = 0;

    m_HalfMoves = 0;
    m_FullMoves = 1;
}

void Board::FromFEN(const std::string& fen) {
    m_Board.fill(Piece::None);
    m_PieceBitBoards.fill(0);
    m_ColourBitBoards.fill(0);
    m_CastlingPath.fill(0xFFFFFFFFFFFFFFFF);

    StringParser fenParser(fen);

    std::string_view board;
    fenParser.Next(board);

    Square square = 56;
    for (const char c : board) {
        if (std::isalpha(c)) {
            Piece p;

            switch (c) {
                case 'P': p = WhitePawn;   break;
                case 'N': p = WhiteKnight; break;
                case 'B': p = WhiteBishop; break;
                case 'R': p = WhiteRook;   break;
                case 'Q': p = WhiteQueen;  break;
                case 'K': p = WhiteKing;   break;
                case 'p': p = BlackPawn;   break;
                case 'n': p = BlackKnight; break;
                case 'b': p = BlackBishop; break;
                case 'r': p = BlackRook;   break;
                case 'q': p = BlackQueen;  break;
                case 'k': p = BlackKing;   break;

                default: std::cout << "Invalid FEN!\n"; return;
            }

            PlacePiece(p, square);
            square++;
        } else if (std::isdigit(c)) {
            square += c - '0';  // Skip empty squares
        } else if (c == '/') {
            square -= 16;  // Next line on chessboard
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

    fenParser.Next(m_HalfMoves);
    fenParser.Next(m_FullMoves);
}

std::string Board::ToFEN() {
    std::ostringstream fen;

    uint32_t emptySquares = 0;

    for (Square rank = 7; rank < 8; rank--) {
	    for (Square file = 0; file < 8; file++) {
            Piece p = m_Board[ToSquare('a' + file, '1' + rank)];
            if (p == Piece::None) {
                emptySquares++;
            } else {
                // Outputs the number of empty squares
                if (emptySquares > 0) {
                    fen << emptySquares;
                    emptySquares = 0;
                }

                fen << PieceToChar(p);
            }
	    }

        // Outputs the number of empty squares
        if (emptySquares > 0)
            fen << emptySquares;
        fen << "/";
        emptySquares = 0;
    }

    fen << " " << (m_PlayerTurn == White ? "w " : "b ");

    uint32_t castlingCount = 0;
    if (m_CastlingPath[White | KingSide] != NO_CASTLE) {
        fen << "K";
        castlingCount++;
    }
    if (m_CastlingPath[White | QueenSide] != NO_CASTLE) {
        fen << "Q";
    	castlingCount++;
    }
    if (m_CastlingPath[Black | KingSide] != NO_CASTLE) {
        fen << "k";
    	castlingCount++;
    }
    if (m_CastlingPath[Black | QueenSide] != NO_CASTLE) {
        fen << "q";
    	castlingCount++;
    }
    if (castlingCount == 0)
        fen << "-";

    if (m_EnPassantSquare != 0)
        fen << " " << (char)('a' + FileOf(m_EnPassantSquare)) << (char)('1' + RankOf(m_EnPassantSquare) / 8);
    else
        fen << " -";

    fen << " " << m_HalfMoves << " " << m_FullMoves;

	return fen.str();
}

AlgebraicMove Board::Move(LongAlgebraicMove m) {
    Piece piece = m_Board[m.SourceSquare];
    Colour colour = GetColour(piece);
    PieceType pieceType = GetPieceType(piece);

    // ASSERT(p != Piece::None)

    if (!IsMoveLegal(m)) {
        std::cout << "Illegal move: " << m << "!\n";
        std::cout << "Pseudo legal:\n";
        PrintBitBoard(GetPseudoLegalMoves(m.SourceSquare));
        std::cout << "Legal:\n";
        PrintBitBoard(GetLegalMoves(m.SourceSquare));
        std::cout << "Destination:\n";
        PrintBitBoard(1ull << m.DestinationSquare);
    }

    Square enPassantSquare = 0;
    bool pawnMoveOrCapture = false;

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

            // Only move the rook because the king will be moved below
            RemovePiece(rookSquare);
            PlacePiece(TypeAndColour(Rook, colour), newRookSquare);
        }

        m_CastlingPath[colour | KingSide] = 0xFFFFFFFFFFFFFFFF;
        m_CastlingPath[colour | QueenSide] = 0xFFFFFFFFFFFFFFFF;
    } else if (pieceType == Pawn) {
        pawnMoveOrCapture = true;
        if (m.SourceSquare - m.DestinationSquare == 16) {  // If black pushed pawn two squares
            enPassantSquare = m.DestinationSquare + 8;
        } else if (m.DestinationSquare - m.SourceSquare == 16) {  // If white pushed pawn two squares
            enPassantSquare = m.DestinationSquare - 8;
        } else if (m.DestinationSquare == m_EnPassantSquare) {  // If taking en passant
            // Remove the en passant-ed pawn
            if (colour == White)
                RemovePiece(m.DestinationSquare - 8);
            else
                RemovePiece(m.DestinationSquare + 8);
        } else if ((1ull << m.DestinationSquare) & 0xFF000000000000FF) {  // If pawn is promoting
            if (m.Promotion == Pawn || m.Promotion == King)
                std::cout << "Must promote to another piece!\n";

            piece = TypeAndColour(m.Promotion, colour);
        }
    }

    m_EnPassantSquare = enPassantSquare;

    if (m.SourceSquare == 0 || m.DestinationSquare == 0)
        m_CastlingPath[Black | KingSide] = 0xFFFFFFFFFFFFFFFF;
    else if (m.SourceSquare == 7 || m.DestinationSquare == 7)
        m_CastlingPath[Black | QueenSide] = 0xFFFFFFFFFFFFFFFF;
    else if (m.SourceSquare == 56 || m.DestinationSquare == 56)
        m_CastlingPath[White | KingSide] = 0xFFFFFFFFFFFFFFFF;
    else if (m.SourceSquare == 63 || m.DestinationSquare == 63)
        m_CastlingPath[White | QueenSide] = 0xFFFFFFFFFFFFFFFF;

    bool capture = m_Board[m.DestinationSquare] != Piece::None;

    RemovePiece(m.SourceSquare);
    // We have to erase the piece from the bit boards before we capture it
    RemovePiece(m.DestinationSquare);
    PlacePiece(piece, m.DestinationSquare);

    pawnMoveOrCapture = pawnMoveOrCapture || capture;
    m_HalfMoves = (m_HalfMoves + 1) * !pawnMoveOrCapture;  // Increments if no pawn move or capture
    m_FullMoves += m_PlayerTurn == Black;

    m_PlayerTurn = OppositeColour(m_PlayerTurn);

    char specifier = 0;
    if (capture && pieceType == Pawn)
        specifier = 'a' + FileOf(m.SourceSquare);

    return { piece, m.DestinationSquare, specifier, capture };
}

bool Board::IsMoveLegal(LongAlgebraicMove move) {
    return GetLegalMoves(move.SourceSquare) & (1ull << move.DestinationSquare);
}

BitBoard Board::GetLegalMoves(Square piece) {
    Colour playerColour = GetColour(m_Board[piece]);
    Colour enemyColour = OppositeColour(playerColour);

    if (enemyColour == m_PlayerTurn)
        return 0;

    BitBoard allPieces = m_ColourBitBoards[White] | m_ColourBitBoards[Black];
    BitBoard king = m_ColourBitBoards[playerColour] & m_PieceBitBoards[King];
    BitBoard enemyPieces = m_ColourBitBoards[enemyColour];

    if (GetPieceType(m_Board[piece]) == King) {
        BitBoard legalMoves = GetPseudoLegalMoves(piece);
        BitBoard controlledSquares = ControlledSquares(enemyColour);
        // Deals with castling
        if (!((allPieces & ~king) & m_CastlingPath[playerColour | KingSide]) && !(controlledSquares & m_CastlingPath[playerColour | KingSide]))
            legalMoves |= 0x40ull << (playerColour == White ? 0 : 56);
        if (!((allPieces & ~king) & m_CastlingPath[playerColour | QueenSide]) && !(controlledSquares & m_CastlingPath[playerColour | QueenSide]))
            legalMoves |= 0x04ull << (playerColour == White ? 0 : 56);
        return legalMoves & ~controlledSquares;
    }

    Square kingSquare = GetSquare(king);

    BitBoard checkMask = 0;
    BitBoard checkers = 0;

    // Deals with checks from bishops, queens
    BitBoard bishopView = PseudoLegal::BishopAttack(kingSquare, allPieces);
    BitBoard bishopCheck = bishopView & enemyPieces & (m_PieceBitBoards[Bishop] | m_PieceBitBoards[Queen]);
    checkers |= bishopCheck;
    checkMask |= PseudoLegal::Line(king, bishopCheck);

    // Deals with pins from bishops, queens
    BitBoard bishopXRay = PseudoLegal::BishopAttack(kingSquare, allPieces & ~bishopView) & enemyPieces & (m_PieceBitBoards[Bishop] | m_PieceBitBoards[Queen]);
    BitBoard bishopPin = 0;
    while (bishopXRay) {
        bishopPin |= PseudoLegal::Line(king, bishopXRay);
        bishopXRay &= bishopXRay - 1;
    }

    // Deals with checks from rooks, queens
    BitBoard rookView = PseudoLegal::RookAttack(kingSquare, allPieces);
    BitBoard rookCheck = rookView & enemyPieces & (m_PieceBitBoards[Rook] | m_PieceBitBoards[Queen]);
    checkers |= rookCheck;
    checkMask |= PseudoLegal::Line(king, rookCheck);

    // Deals with pins from rooks and queens
    BitBoard rookXRay = PseudoLegal::RookAttack(kingSquare, allPieces & ~rookView) & enemyPieces & (m_PieceBitBoards[Rook] | m_PieceBitBoards[Queen]);
    BitBoard rookPin = 0;
    while (rookXRay) {
        rookPin |= PseudoLegal::Line(king, rookXRay);
        rookXRay &= rookXRay - 1;
    }

    // Deals with checks from knights
    BitBoard knightCheck = PseudoLegal::KnightAttack(kingSquare) & enemyPieces & m_PieceBitBoards[Knight];
    checkers |= knightCheck;
    checkMask |= knightCheck;

    // Deals with checks from pawns
    BitBoard pawnCheck = PseudoLegal::PawnAttack(kingSquare, playerColour) & enemyPieces & m_PieceBitBoards[Pawn];
    checkers |= pawnCheck;
    checkMask |= pawnCheck;

    // If there are no checks, we don't prune any moves
    if (checkMask == 0)
        checkMask = 0xFFFFFFFFFFFFFFFF;
    
    // If it is double check, we can remove all blocking moves (we can only move the king)
    checkMask *= __popcnt64(checkers) < 2;

    BitBoard pseudoLegal = GetPseudoLegalMoves(piece);

    BitBoard pieceSquare = 1ull << piece;
    if (pieceSquare & rookPin)  // If piece is horizontally pinned
        pseudoLegal &= rookPin & PseudoLegal::RookAttack(piece, allPieces);
    else if (pieceSquare & bishopPin)  // If piece is diagonally pinned
        pseudoLegal &= bishopPin & PseudoLegal::BishopAttack(piece, allPieces);

    pseudoLegal &= checkMask;

    // Handles en passant pin: 8/4p3/8/2K2P1r/8/8/8/7k b - - 0 1
    if (GetPieceType(m_Board[piece]) == Pawn && king & 0x000000FFFF000000 && m_EnPassantSquare) {
        // Gets the two pawns involved in en passant
        BitBoard twoPawns = ((1ull << m_EnPassantSquare + 8) | (1ull << m_EnPassantSquare - 8)) & 0x000000FFFF000000;
        twoPawns |= pieceSquare;

        // Removes the pawns and sees if king is in check
        rookView = PseudoLegal::RookAttack(kingSquare, allPieces & ~twoPawns);
        if (rookView & enemyPieces & (m_PieceBitBoards[Rook] | m_PieceBitBoards[Queen]))
            pseudoLegal &= ~(1ull << m_EnPassantSquare);
    }

    return pseudoLegal;
}

BitBoard Board::GetPseudoLegalMoves(Square piece) const {
    PieceType pt = GetPieceType(m_Board[piece]);
    Colour c = GetColour(m_Board[piece]);

    BitBoard blockers = m_ColourBitBoards[White] | m_ColourBitBoards[Black];

    switch (pt) {
        case Pawn:   return PseudoLegal::PawnMoves(piece, c, blockers, m_EnPassantSquare) & ~m_ColourBitBoards[c];
        case Knight: return PseudoLegal::KnightAttack(piece) & ~m_ColourBitBoards[c];
        case Bishop: return PseudoLegal::BishopAttack(piece, blockers) & ~m_ColourBitBoards[c];
        case Rook:   return PseudoLegal::RookAttack(piece, blockers) & ~m_ColourBitBoards[c];
        case Queen:  return PseudoLegal::QueenAttack(piece, blockers) & ~m_ColourBitBoards[c];
        case King:   return PseudoLegal::KingAttack(piece) & ~m_ColourBitBoards[c];

        default: return 0;
    }
}

BitBoard Board::ControlledSquares(Colour c) const {
    BitBoard king = m_ColourBitBoards[OppositeColour(c)] & m_PieceBitBoards[King];
    BitBoard blockers = (m_ColourBitBoards[White] | m_ColourBitBoards[Black]) ^ king;

    BitBoard controlledSquares = 0;
    for (Square s = 0; s < 64; s++) {
        if (m_Board[s] != Piece::None && GetColour(m_Board[s]) == c) {
            switch (GetPieceType(m_Board[s])) {
                case Pawn:   controlledSquares |= PseudoLegal::PawnAttack(s, c); break;
                case Knight: controlledSquares |= PseudoLegal::KnightAttack(s); break;
                case Bishop: controlledSquares |= PseudoLegal::BishopAttack(s, blockers); break;
                case Rook:   controlledSquares |= PseudoLegal::RookAttack(s, blockers); break;
                case Queen:  controlledSquares |= PseudoLegal::QueenAttack(s, blockers); break;
                case King:   controlledSquares |= PseudoLegal::KingAttack(s); break;

                default: return 0;
            }
        }
    }

    return controlledSquares;
}
