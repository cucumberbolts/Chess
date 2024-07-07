#pragma once

#include "Board.h"
#include "Utility/StringParser.h"

#include <filesystem>
#include <ostream>
#include <unordered_map>
#include <vector>

enum GameMoveFlags : uint8_t {
	CASTLE_WHITE_KINGSIDE	= 1 << 0,
	CASTLE_WHITE_QUEENSIDE	= 1 << 1,
	CASTLE_BLACK_KINGSIDE	= 1 << 2,
	CASTLE_BLACK_QUEENSIDE	= 1 << 3,
	EN_PASSANT				= 1 << 4,
};

struct GameMove {
	Square Start;
	Square Destination;
	Piece MovingPiece;
	Piece DestinationPiece;
	PieceType Promotion; // TODO: Not needed?
	GameMoveFlags Flags = (GameMoveFlags)0;

	std::string Comment;

	bool operator==(const GameMove& m) const {
		return Start == m.Start
			&& Destination == m.Destination
			&& MovingPiece == m.MovingPiece
			&& DestinationPiece == m.DestinationPiece
			&& Promotion == m.Promotion
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
	uint32_t CurrentPly() const { return m_Ply; }
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
