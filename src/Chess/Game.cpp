#include "Game.h"

#include <sstream>

Game::Game() {
	m_Branches.emplace_back(0, 1);
}

std::string Game::ToPGN() const {
	std::ostringstream oss;
	oss << *this;
	return oss.str();
}

LongAlgebraicMove Game::Move(AlgebraicMove move) {
	Piece destinationPiece = m_Position[move.Destination];

	LongAlgebraicMove lam = m_Position.Move(move);

	Colour colour = GetColour(m_Position[lam.DestinationSquare]);
	// Use ply count parity instead?
	//Colour colour = (Colour)~(m_Ply & 1);

	GameMove gameMove = {
		lam.SourceSquare,
		lam.DestinationSquare,
		TypeAndColour(move.MovingPiece, colour),
		destinationPiece,
		// TODO: FIX
		Pawn,
		(GameMoveFlags)0
	};

	Move(gameMove);

	return lam;
}

AlgebraicMove Game::Move(LongAlgebraicMove move) {
	GameMove gameMove = {
		move.SourceSquare,
		move.DestinationSquare,
		m_Position[move.SourceSquare],
		m_Position[move.DestinationSquare],
		move.Promotion,
		(GameMoveFlags)0  // TODO: FIX
	};

	Move(gameMove);

	return m_Position.Move(move);
}

void Game::Back() {
	if (m_Ply == 0)
		return;

	// Undo the current move on the board
	// TODO: Special cases (castling, promotion, en passant)
	const GameMove& gm = m_Branches[m_Variation].Moves[m_Ply - m_Branches[m_Variation].StartingPly];
	m_Position.RemovePiece(gm.Destination);
	m_Position.PlacePiece(gm.MovingPiece, gm.Start);
	if (gm.DestinationPiece != None)
		m_Position.PlacePiece(gm.DestinationPiece, gm.Destination);

	m_Position.m_PlayerTurn = OppositeColour(m_Position.m_PlayerTurn);

	// If the pointer is on the first move of the variation,
	// Jump to the last move of the parent branch
	if (m_Ply == m_Branches[m_Variation].StartingPly) {
		m_Variation = m_Branches[m_Variation].Parent;
	}

	m_Ply--;
}

void Game::Forward() {
	const uint32_t maxPly = (uint32_t)m_Branches[m_Variation].Moves.size() + m_Branches[m_Variation].StartingPly - 1;

	// If we are at the end of the branch, go to main variation if it exists
	if (m_Ply == maxPly) {
		if (m_Branches[m_Variation].Variations.empty())
			return;

		// Index 0 is the main line
		m_Variation = m_Branches[m_Variation].Variations[0];
	}

	m_Ply++;

	const GameMove& gm = m_Branches[m_Variation].Moves[m_Ply - m_Branches[m_Variation].StartingPly];
	m_Position.Move(LongAlgebraicMove(gm.Start, gm.Destination, gm.Promotion));
}

void Game::Seek(uint32_t ply, uint32_t variation) {

}

void Game::Delete(uint32_t ply, uint32_t variation) {
	// Make it memory efficient--invalidate branches and reuse them
}

void Game::Move(GameMove move) {
	Branch* currentBranch = &m_Branches[m_Variation];

	const uint32_t maxPly = (uint32_t)m_Branches[m_Variation].Moves.size() + m_Branches[m_Variation].StartingPly - 1;

	// If variations exist and index is on the last move of the list
	if (!m_Branches[m_Variation].Variations.empty() && m_Ply == maxPly) {
		// check if the move is one of them
		for (const uint32_t var : m_Branches[m_Variation].Variations) {
			if (m_Branches[var].Moves[0] == move) {
				m_Variation = var;
				m_Ply++;
				return;
			}
		}

		// If it doesn't exist
		// Create new branch for new move
		currentBranch = &m_Branches.emplace_back(m_Variation, m_Ply + 1);
		// Add variation to old branch
		m_Branches[m_Variation].Variations.push_back((uint32_t)m_Branches.size() - 1);
		// Change index to new variation
		m_Variation = (uint32_t)m_Branches.size() - 1;
	}
	else if (m_Ply < maxPly) {
		// If the next move is already in the tree,
		// advance the index and return the move
		// Otherwise, split the tree
		if (m_Branches[m_Variation].Moves[m_Ply - m_Branches[m_Variation].StartingPly] == move) {
			m_Ply++;
			return;
		}

		// Reserve space
		m_Branches.reserve(m_Branches.capacity() + 2);

		// Create new branch for old moves
		Branch& old = m_Branches.emplace_back(m_Variation, m_Ply + 1);
		// Copy the old moves into the new branch
		std::copy(m_Branches[m_Variation].Moves.begin() + m_Ply, m_Branches[m_Variation].Moves.end(), std::back_inserter(old.Moves));
		// Move old variations to new branch
		if (!m_Branches[m_Variation].Variations.empty())
			old.Variations = std::move(m_Branches[m_Variation].Variations);
		// Delete old moves from old branch
		m_Branches[m_Variation].Moves.erase(m_Branches[m_Variation].Moves.begin() + m_Ply, m_Branches[m_Variation].Moves.end());
		// Add variation to old branch
		m_Branches[m_Variation].Variations.push_back((uint32_t)m_Branches.size() - 1);

		// Create new branch for new move
		currentBranch = &m_Branches.emplace_back(m_Variation, m_Ply + 1);
		// Add variation to old branch
		m_Branches[m_Variation].Variations.push_back((uint32_t)m_Branches.size() - 1);
		// Change index to new variation
		m_Variation = (uint32_t)m_Branches.size() - 1;
	}

	// TODO: Determine castle, en passant, or promotion
	// (Castling and promotion can be determined in the caller
	// functions since it is a different process for both)
	// en passant can be determined here

	currentBranch->Moves.emplace_back(std::move(move));
	m_Ply++;
}

std::ostream& PrintBranch(std::ostream& os, Board& board, const Game& game, const Branch& branch) {
	os << "Ply: " << branch.StartingPly << ": ";
	
	uint32_t index = 0;
	for (; index < branch.Moves.size(); index++) {
		os << board.Move(LongAlgebraicMove(branch.Moves[index].Start, branch.Moves[index].Destination)) << " ";
	}

	os << "\n";

	for (uint32_t var : branch.Variations) {
		os << "Variation: " << var << "\n";
		PrintBranch(os, board, game, game.m_Branches[var]);
	}

	// Undo moves
	for (uint32_t i = index - 1; i < index; i--) {
		const GameMove& gm = branch.Moves[i];
		board.RemovePiece(gm.Destination);
		board.PlacePiece(gm.MovingPiece, gm.Start);
		if (gm.DestinationPiece != None)
			board.PlacePiece(gm.DestinationPiece, gm.Destination);
		board.m_PlayerTurn = OppositeColour(board.m_PlayerTurn);
	}

	return os;
}

std::ostream& operator<<(std::ostream& os, const Game& game) {
#if 0
	Board board;

	os << "Variation: 0\n";
	PrintBranch(os, board, game, game.m_Branches[0]);
#else
	// Mostly for debugging
	for (uint32_t b = 0; b < game.m_Branches.size(); b++) {
		os << "Variation #" << b << ":\n";
		os << "Ply: " << game.m_Branches[b].StartingPly << " ";

		// Print moves
		for (const GameMove& g : game.m_Branches[b].Moves)
			os << LongAlgebraicMove(g.Start, g.Destination) << " ";

		// Print variations
		os << "-> (";
		for (uint32_t var : game.m_Branches[b].Variations) {
			os << var << ", ";
		}

		os << ")\n";
	}
#endif

	return os;
}
