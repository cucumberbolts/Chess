#include "Move.h"

static char s_Promotions[] = " NBRQEEE";  // E for error lol

std::string LongAlgebraicMove::ToString() {
	char result[5]; // source square + destination square + possible promotion
	char* ptr = result;

	*(ptr++) = (char)('a' + FileOf(SourceSquare));
	*(ptr++) = (char)('1' + RankOf(SourceSquare));
	*(ptr++) = (char)('a' + FileOf(DestinationSquare));
	*(ptr++) = (char)('1' + RankOf(DestinationSquare));

	if (Promotion != Pawn && Promotion != King)
		*(ptr++) = s_Promotions[(size_t)Promotion];

	return { result, ptr };
}

AlgebraicMove::AlgebraicMove(const std::string& str) {
	bool error = false;
	
	char* fromFront = (char*)&str.front();
	char* fromBack = (char*)&str.back();

	if (*fromBack == '#') {
		Flags |= MoveFlag::Checkmate;
		fromBack--;
	} else if (*fromBack == '+') {
		Flags |= MoveFlag::Check;
		fromBack--;
	}

	const std::string_view castle(fromFront, fromBack - fromFront + 1);

	if (castle == "O-O-O") {
		Flags |= MoveFlag::CastleQueenSide;
		return;
	}
	
	if (castle == "O-O") {
		Flags |= MoveFlag::CastleKingSide;
		return;
	}
	
	if (std::islower(str[0])) {  // Pawn
		if (str[1] == 'x') {  // Capture
			char file = str[0];  // The 'a' in something like 'axb7'
			// Technically we could calculate the rank of the pawn
			// But it is not necessary, so just add '1'
			Specifier = ToSquare(file, '1');
			Specifier |= SpecifyFile;

			Flags |= MoveFlag::Capture;

			Destination = ToSquare(str[2], str[3]);
			fromFront += 4;
		} else {
			Destination = ToSquare(str[0], str[1]);
			fromFront += 2;
		}

		// Promotion
		if (*fromFront == '=') {
			char promotion = *(++fromFront);

			switch (promotion) {
				case 'N': Flags |= MoveFlag::PromoteKnight; break;
				case 'B': Flags |= MoveFlag::PromoteBishop; break;
				case 'R': Flags |= MoveFlag::PromoteRook; break;
				case 'Q': Flags |= MoveFlag::PromoteQueen; break;
				default: error = true;
			}
		}
	} else {  // Not a pawn
		MovingPiece = CharToPieceType(str[0]);

		++fromFront;

		char rank = *(fromBack--);
		char file = *(fromBack--);
		Destination = ToSquare(file, rank);

		// Capture
		if (*fromBack == 'x')
			fromBack--;

		// The only thing left is the specifier
		// N'b'd4, N'3'd4, or N'b3'd4
		const uint64_t length = fromBack - fromFront + 1;
		if (length == 1) {
			// We don't know the other coordinate (rank or file)
			// So just add 'a' or '1'
			if (std::isalpha(*fromFront)) {
				Specifier = ToSquare(*fromFront, '1');
				Specifier |= SpecifyFile;
			} else if (std::isalpha(*fromFront)) {
				Specifier = ToSquare('a', *fromFront);
				Specifier |= SpecifyRank;
			} else {
				error = true;
			}
		} else if (length == 2) {
			file = *(fromFront);
			rank = *(++fromFront);
			Specifier = ToSquare(file, rank);
			Specifier |= SpecifyFile | SpecifyRank;
		} else if (length != 0) {
			error = true;
		}
	}

	if (error)
		//throw std::exception("Invalid algebraic move!");
		std::cout << "Invalid algebraic move!\n";
}

std::string AlgebraicMove::ToString() {
	if (Flags & MoveFlag::CastleKingSide) {
		if (Flags & MoveFlag::Check)
			return "O-O+";
		if (Flags & MoveFlag::Checkmate)
			return "O-O#";
		return "O-O";
	}

	if (Flags & MoveFlag::CastleQueenSide) {
		if (Flags & MoveFlag::Check)
			return "O-O-O+";
		if (Flags & MoveFlag::Checkmate)
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
		if (Specifier & SpecifyFile)
			*(ptr++) = (char)('a' + FileOf(Specifier & RemoveSpecifierFlag));

		// If the specefier is a rank (for example: 'N1d2')
		if (Specifier & SpecifyRank)
			*(ptr++) = (char)('1' + RankOf(Specifier & RemoveSpecifierFlag));
	}

	// Adds an 'x' for a capture
	if (Flags & MoveFlag::Capture) {
		// gets the 'a' in something like 'axb7'
		if (MovingPiece == Pawn)
			*(ptr++) = (char)('a' + FileOf(Specifier & RemoveSpecifierFlag));

		*(ptr++) = 'x';
	}

	// The destination square
	*(ptr++) = (char)('a' + FileOf(Destination));
	*(ptr++) = (char)('1' + RankOf(Destination));

	// Promotion
	if (Flags & 0b111) {
		*(ptr++) = '=';

		// Uses the promotion-flag as an index
		*(ptr++) = s_Promotions[(Flags & 0b111)];
	}

	if (Flags & MoveFlag::Checkmate)
		*(ptr++) = '#';
	else if (Flags & MoveFlag::Check)
		*(ptr++) = '+';

	return { result, ptr };
}
