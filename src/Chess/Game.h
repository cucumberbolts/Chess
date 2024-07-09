#pragma once

#include "Board.h"
#include "Utility/StringParser.h"

#include <filesystem>
#include <ostream>
#include <unordered_map>
#include <vector>

// Closely resembles the MoveFlag enum
// that is designed for AlgebraicMove
using GameMoveFlags = uint8_t;

namespace GameMoveFlag {
	enum : uint8_t {
		PromoteKnight  = 0b001,
		PromoteBishop  = 0b010,
		PromoteRook    = 0b011,
		PromoteQueen   = 0b100,

		PromotionFlags = 0b111,
		
		CastleWhiteKingSide  = 0b001000,
		CastleWhiteQueenSide = 0b010000,
		CastleBlackKingSide  = 0b101000,
		CastleBlackQueenSide = 0b110000,

		// Exclude CanCastleOtherSide
		CastlingFlags        = 0b111000,

		// When using the GameMove class to undo a castling move on a Board,
		// it is necessary to know whether or not the king could castle the
		// *other* direction so that we can reset the castling rights accordingly
		CanCastleOtherSide = 1 << 6,

		EnPassant = 1 << 7,
	};
}

struct GameMove {
	Square Start;
	Square Destination;
	Piece MovingPiece;
	Piece DestinationPiece;
	GameMoveFlags Flags = 0;

	std::string Comment;

	bool operator==(const GameMove& m) const {
		return Start == m.Start
			&& Destination == m.Destination
			&& MovingPiece == m.MovingPiece
			&& DestinationPiece == m.DestinationPiece
			&& Flags == m.Flags;
	}
};

// A series of moves without variations
// As soon as a variation starts, the
// 'Variations' vector is populated
struct Branch {
	// Index into 'Branches' in 'Game'
	Branch* Parent;

	// The ply number before the first move of 'Moves'
	uint32_t StartingPly;

	std::vector<GameMove> Moves;

	// Index 0 is the main line
	std::vector<Branch*> Variations;

	Branch(Branch* parent, uint32_t startingPly)
		: Parent(parent), StartingPly(startingPly) {
	}

	~Branch() {
		for (Branch* b : Variations)
			delete b;
	}

	Branch(const Branch&) = delete;
	Branch(Branch&&) = delete;

	Branch& operator=(const Branch&) = delete;
	Branch& operator=(Branch&&) = delete;
};

// TODO: Make function that parses file with multiple games
// std::vector<Game> ParseGames(const std::filesystem::path& path);

class Game {
	friend std::ostream& operator<<(std::ostream&, const Game&);
public:
	Game();
	Game(const std::string& pgn);
	~Game();

	std::string ToPGN() const;

	const Board& GetPosition() const { return m_Position; }
	uint32_t CurrentPly()      const { return m_Ply; }
	Branch* CurrentVariation() const { return m_Variation; }

	// Add move to tree
	LongAlgebraicMove Move(AlgebraicMove move);
	AlgebraicMove Move(LongAlgebraicMove move);

	void Back();	// Back 1 move
	void Forward();	// Forward 1 move
	void Seek(uint32_t ply, Branch* variation);	  // Back/Forward to an index
	void Delete(uint32_t ply, Branch* variation); // Deletes the specified move (and every subsequent move)

	// Add comment to current move
	void AddComment(const std::string& comment) const;
	void AddComment(std::string&& comment) const;
private:
	void Move(GameMove move);

	void FromPGN(const std::string& pgn);
	void ParseVariation(StringParser& sp);

	// Game info
	std::unordered_map<std::string, std::string> m_Header;
	
	// Top-level branch, contains all the moves
	Branch* m_Branches;

	// Current state
	uint32_t m_Ply = 0;
	Branch* m_Variation;
	Board m_Position;
};

std::ostream& operator<<(std::ostream& os, const Game& game);
