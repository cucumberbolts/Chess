#include "Board.h"

#include "PseudoLegal.h"

#include "Utility/StringParser.h"

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

                default: throw InvalidFenException("Unexpected character in FEN string!");
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

std::string Board::ToFEN() const {
    std::ostringstream fen;

    uint32_t emptySquares = 0;

    for (Square rank = 7; rank < 8; rank--) {
	    for (Square file = 0; file < 8; file++) {
            Piece p = m_Board[rank * 8 + file];
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

        if (rank > 0)
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
        fen << " " << (char)('a' + FileOf(m_EnPassantSquare)) << (char)('1' + RankOf(m_EnPassantSquare));
    else
        fen << " -";

    fen << " " << m_HalfMoves << " " << m_FullMoves;

	return fen.str();
}

AlgebraicMove Board::Move(LongAlgebraicMove m) {
    Piece piece = m_Board[m.SourceSquare];
    Colour colour = GetColour(piece);
    PieceType pieceType = GetPieceType(piece);

    if (!IsMoveLegal(m))
        throw IllegalMoveException(m.ToString());
    
    bool pawnMove = false;
    bool capture = m_Board[m.DestinationSquare] != Piece::None;
    Square newEnPassantSquare = 0;
    uint8_t moveFlags = 0;

    if (pieceType == King) {
        int direction = m.DestinationSquare - m.SourceSquare;  // Kingside or queenside

        // If king is castling
        if (abs(direction) == 2) {
            Square rookSquare, newRookSquare;

            if (direction < 0) {  // Queenside
                rookSquare = m.SourceSquare - 4;
                newRookSquare = m.DestinationSquare + 1;
                moveFlags |= MoveFlag::CastleQueenSide;
            } else {              // Kingside
                rookSquare = m.SourceSquare + 3;
                newRookSquare = m.DestinationSquare - 1;
                moveFlags |= MoveFlag::CastleKingSide;
            }

            // Only move the rook because the king will be moved below
            RemovePiece(rookSquare);
            PlacePiece(TypeAndColour(Rook, colour), newRookSquare);
        }

        m_CastlingPath[colour | KingSide] = NO_CASTLE;
        m_CastlingPath[colour | QueenSide] = NO_CASTLE;
    } else if (pieceType == Pawn) {
        pawnMove = true;
        if (m.SourceSquare - m.DestinationSquare == 16) {  // If black pushed pawn two squares
            newEnPassantSquare = m.DestinationSquare + 8;
        } else if (m.DestinationSquare - m.SourceSquare == 16) {  // If white pushed pawn two squares
            newEnPassantSquare = m.DestinationSquare - 8;
        } else if (m.DestinationSquare == m_EnPassantSquare) {  // If taking en passant
            // Remove the en passant-ed pawn
            if (colour == White)
                RemovePiece(m.DestinationSquare - 8);
            else
                RemovePiece(m.DestinationSquare + 8);

            capture = true;
        } else if ((1ull << m.DestinationSquare) & 0xFF000000000000FF) {  // If pawn is promoting
            if (m.Promotion == Pawn || m.Promotion == King)
                throw IllegalMoveException("Pawn must promote to another piece!");

            piece = TypeAndColour(m.Promotion, colour);
        }
    }

    m_EnPassantSquare = newEnPassantSquare;

    // If a rook moves or is captured, remove castling rights accordingly
    if (m.SourceSquare == A1 || m.DestinationSquare == A1)
        m_CastlingPath[White | QueenSide] = NO_CASTLE;
    else if (m.SourceSquare == H1 || m.DestinationSquare == H1)
        m_CastlingPath[White | KingSide] = NO_CASTLE;
    else if (m.SourceSquare == A8 || m.DestinationSquare == A8)
        m_CastlingPath[Black | QueenSide] = NO_CASTLE;
    else if (m.SourceSquare == H8 || m.DestinationSquare == H8)
        m_CastlingPath[Black | KingSide] = NO_CASTLE;

    m_HalfMoves = (m_HalfMoves + 1) * !(pawnMove || capture);  // Increments if no pawn move or capture, sets to 0 otherwise
    m_FullMoves += m_PlayerTurn == Black;

    //
    // Figure out the algebraic notation
    //

    Square specifier = m.SourceSquare;

    if (pieceType == Pawn) {
        // For things like 'axb7', the 'a' is needed
        specifier |= SpecifyFile * capture;
    } else if (pieceType != King) {
        // Other pieces that can go to the same square
        BitBoard possiblePieces = m_ColourBitBoards[colour] & m_PieceBitBoards[pieceType];
        const BitBoard allPieces = m_ColourBitBoards[White] | m_ColourBitBoards[Black];
        switch (pieceType) {
            case Knight: possiblePieces &= PseudoLegal::KnightAttack(m.DestinationSquare); break;
            case Bishop: possiblePieces &= PseudoLegal::BishopAttack(m.DestinationSquare, allPieces); break;
            case Rook:   possiblePieces &= PseudoLegal::RookAttack(m.DestinationSquare, allPieces); break;
            case Queen:  possiblePieces &= PseudoLegal::QueenAttack(m.DestinationSquare, allPieces); break;
            default: possiblePieces = 0;
        }

        // Loop through the possible pieces, remove the ones that can't move (pinned)
        for (BitBoard b = possiblePieces; b != 0; b &= b - 1) {
            if (!GetPieceLegalMoves(GetSquare(b)))
                possiblePieces &= ~(1ull << GetSquare(b));
        }

        // Map everything to the first rank and check if there is more than one piece
        if (SquareCount((possiblePieces * 0x0101010101010101) >> 56) > 1)
            specifier |= SpecifyFile;

        // On the same file
        if (BitBoardFile(m.SourceSquare) & possiblePieces & ~(1ull << m.SourceSquare))
            specifier |= SpecifyRank;
    }

    // Next player's turn
    m_PlayerTurn = OppositeColour(m_PlayerTurn);

    // Move the piece
    RemovePiece(m.SourceSquare);
    // We have to erase the piece from the bit boards before we capture it
    RemovePiece(m.DestinationSquare);
    PlacePiece(piece, m.DestinationSquare);

    // If the current move places the opponent in check
    bool isCheck = m_PieceBitBoards[King] & m_ColourBitBoards[m_PlayerTurn] & ControlledSquares(colour);
    bool isMate = !HasLegalMoves(m_PlayerTurn) && isCheck;
    
    moveFlags |= MoveFlag::Check * isCheck;
    moveFlags |= MoveFlag::Checkmate * isMate;
    moveFlags |= MoveFlag::Capture * capture;

    return { pieceType, m.DestinationSquare, specifier, moveFlags };
}

LongAlgebraicMove Board::Move(AlgebraicMove m) {
    Square source;
    PieceType Promotion = Pawn;

    Colour opponentColour = OppositeColour(m_PlayerTurn);

	if (m.Flags & (MoveFlag::CastleKingSide | MoveFlag::CastleQueenSide)) {
        Square kingStart = E1 ^ (m_PlayerTurn * 0b00111000);
        Square rookStart = H1 ^ (m_PlayerTurn * 0b00111000);
		Square kingDestination, rookDestination;

        if (m.Flags & MoveFlag::CastleKingSide) {
            kingDestination = G1 ^ (m_PlayerTurn * 0b00111000);
            rookDestination = F1 ^ (m_PlayerTurn * 0b00111000);
        } else {
            kingDestination = C1 ^ (m_PlayerTurn * 0b00111000);
            rookDestination = D1 ^ (m_PlayerTurn * 0b00111000);
        }
        
        if (IsMoveLegal({ kingStart, kingDestination })) {
            // Move the king
            RemovePiece(kingStart);
            PlacePiece(TypeAndColour(King, m_PlayerTurn), kingDestination);

            // Move the rook
            RemovePiece(rookStart);
            PlacePiece(TypeAndColour(Rook, m_PlayerTurn), rookDestination);

            m_PlayerTurn = opponentColour;
            return { kingStart, kingDestination, Promotion };
        }

        throw IllegalMoveException(m.ToString());
	} else if (m.MovingPiece == Pawn) {
        int8_t direction = m_PlayerTurn == White ? 8 : -8;
        if (m.Flags & MoveFlag::Capture) {
            BitBoard possiblePieces = PseudoLegal::PawnAttack(m.Destination, opponentColour);
            Square file = FileOf(m.Specifier);
            source = GetSquare(possiblePieces & BitBoardFile(file));
            
            // Remove the en passant-ed pawn if taking en passant
            if (m.Destination == m_EnPassantSquare)
                RemovePiece(m.Destination - direction);
        } else {  // Pawn push
            source = m.Destination - direction;

            // If the pawn ends up on the 4th or 5th rank
            // and there is no pawn 1 square at it's rear
            // it has been pushed two squares
            // which this comment makes clear
            const bool middle = RankOf(m.Destination) == (3 + m_PlayerTurn);  // If it is on the 4th or 5th rank (according to colour)
            if (middle && GetPieceType(m_Board[source]) != Pawn) {
                m_EnPassantSquare = source;
                source -= direction;  // Move 'source' further back one square
            }
        }
	} else {  // Normal piece
        // All of the same type of piece that can go to the same square
        BitBoard possiblePieces = m_PieceBitBoards[m.MovingPiece] & m_ColourBitBoards[m_PlayerTurn];

		const BitBoard allPieces = m_ColourBitBoards[White] | m_ColourBitBoards[Black];
		switch (m.MovingPiece) {
            case Knight: possiblePieces &= PseudoLegal::KnightAttack(m.Destination); break;
            case Bishop: possiblePieces &= PseudoLegal::BishopAttack(m.Destination, allPieces); break;
            case Rook:   possiblePieces &= PseudoLegal::RookAttack(m.Destination, allPieces); break;
            case Queen:  possiblePieces &= PseudoLegal::QueenAttack(m.Destination, allPieces); break;
            default: possiblePieces = 0;
        }
        
        // Prune the pieces that are not specified
        if (m.Specifier & SpecifyFile)
            possiblePieces &= BitBoardFile(FileOf(m.Specifier));

        if (m.Specifier & SpecifyRank)
            possiblePieces &= BitBoardFile(RankOf(m.Specifier));

        // Prune the pinned pieces
        for (BitBoard b = possiblePieces; b != 0; b &= b - 1) {
            if (!GetPieceLegalMoves(GetSquare(b)))
                possiblePieces &= ~(1ull << GetSquare(b));
        }

        if (SquareCount(possiblePieces) != 1)
            throw IllegalMoveException("None, or more than two pieces can move to the same square!");

        source = GetSquare(possiblePieces);
	}
    
    if (!IsMoveLegal({ source, m.Destination }))
        throw IllegalMoveException(m.ToString());

    Piece piece = m_Board[source];
    // Move the piece
    RemovePiece(source);
    // We have to erase the piece from the bit boards before we capture it
    RemovePiece(m.Destination);
    PlacePiece(piece, m.Destination);

    m_PlayerTurn = opponentColour;

    return { source, m.Destination, Promotion };
}

bool Board::HasLegalMoves(Colour colour) {
    for (Square s = 0; s < m_Board.size(); s++)
        if (GetColour(m_Board[s]) == colour)
            if (GetPieceLegalMoves(s) != 0)
                return true;

    return false;
}

BitBoard Board::GetPieceLegalMoves(Square piece) {
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
    checkMask *= SquareCount(checkers) < 2;

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
        BitBoard twoPawns = ((1ull << (m_EnPassantSquare + 8)) | (1ull << (m_EnPassantSquare - 8))) & 0x000000FFFF000000;
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
