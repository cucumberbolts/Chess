#pragma once

#include "Board.h"

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
	uint32_t Parent = 0;

	// The ply number of the first move of 'Moves'
	uint32_t StartingPly = 1;

	std::vector<GameMove> Moves;

	// Index 0 is the main line
	std::vector<uint32_t> Variations;

	Branch(uint32_t parent, uint32_t startingPly)
		: Parent(parent), StartingPly(startingPly) {}
};

class Game {
	friend std::ostream& PrintBranch(std::ostream&, Board&, const Game&, const Branch&);
	friend std::ostream& operator<<(std::ostream&, const Game&);
public:
	Game();

	// TODO: Figure this out
	//Game(const std::filesystem::path& path);
	//Game(const std::string& fen);
	//Game(const std::string& pgn);
	~Game() = default;

	std::string ToPGN() const;

	const Board& GetPosition() const { return m_Position; }
	uint32_t CurrentPly() const { return m_Ply; }
	uint32_t CurrentVariation() const { return m_Variation; }

	// Add move to tree
	LongAlgebraicMove Move(AlgebraicMove move);
	AlgebraicMove Move(LongAlgebraicMove move);

	void Back();	// Back 1 move
	void Forward();	// Forward 1 move
	void Seek(uint32_t ply, uint32_t variation);	// Back/Forward to an index
	void Delete(uint32_t ply, uint32_t variation);	// Deletes the specified move (and every subsequent move)
private:
	void Move(GameMove move);

	// Game info
	std::unordered_map<std::string, std::string> m_Header;

	// All the moves
	std::vector<Branch> m_Branches;

	// Current state
	uint32_t m_Ply = 0;
	uint32_t m_Variation = 0;
	Board m_Position;
};

std::ostream& operator<<(std::ostream& os, const Game& game);
