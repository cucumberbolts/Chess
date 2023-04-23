#include "Move.h"

static char s_Promotions[] = " NBRQ";

std::string AlgebraicMove::ToString() {
	if (Flags & MoveFlags::CastleKingSide) {
		if (Flags & MoveFlags::Check)
			return "O-O+";
		if (Flags & MoveFlags::Checkmate)
			return "O-O#";
		return "O-O";
	}

	if (Flags & MoveFlags::CastleQueenSide) {
		if (Flags & MoveFlags::Check)
			return "O-O-O+";
		if (Flags & MoveFlags::Checkmate)
			return "O-O-O#";
		return "O-O-O";
	}

	// Longest possible string:
	// Qd3xf5+ or hxg8=Q#
	char result[7];
	char* ptr = result;
	
	if (MovingPiece != Pawn) {
		// Gets the piece type (NBRQK)
		*(ptr++) = PieceTypeToChar(MovingPiece);

		// Gets the specifier (like the 'b' in 'Nbd2')

		// If the specefier is a file (for example: 'Nbd2')
		if (Specifier & SpecifierFlags::File)
			*(ptr++) = (char)('a' + FileOf(Specifier & RemoveSpecifierFlag));

		// If the specefier is a rank (for example: 'N1d2')
		if (Specifier & SpecifierFlags::Rank)
			*(ptr++) = (char)('1' + RankOf(Specifier & RemoveSpecifierFlag) / 8);
	}

	// Adds an 'x' for a capture
	if (Flags & MoveFlags::Capture) {
		// gets the 'a' in something like 'axb7'
		if (MovingPiece == Pawn)
			*(ptr++) = (char)('a' + FileOf(Specifier & RemoveSpecifierFlag));

		*(ptr++) = 'x';
	}

	// The destination square
	*(ptr++) = (char)('a' + FileOf(Destination));
	*(ptr++) = (char)('1' + RankOf(Destination) / 8);

	// Promotion
	if (Flags & 0b111) {
		*(ptr++) = '=';

		// Uses the promotion-flag as an index
		*(ptr++) = s_Promotions[(Flags & 0b111)];
	}

	if (Flags & MoveFlags::Checkmate)
		*(ptr++) = '#';
	else if (Flags & MoveFlags::Check)
		*(ptr++) = '+';

	return { result, ptr };
}
