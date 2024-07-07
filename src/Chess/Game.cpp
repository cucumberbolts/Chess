#include "Game.h"

#include <fstream>
#include <sstream>

Game::Game()
	: m_Branches(new Branch(nullptr, 0)), m_Variation(m_Branches) {
	
	Branch* newBranch = new Branch(m_Branches, 0);
	m_Branches->Variations.push_back(newBranch);
	m_Variation = newBranch;
}

Game::Game(const std::string& pgn)
	: m_Branches(new Branch(nullptr, 0)), m_Variation(m_Branches) {

	FromPGN(pgn);
}

Game::~Game() {
	delete m_Branches;
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

	m_Position.UndoMove(m_Variation->Moves[m_Ply - m_Variation->StartingPly - 1]);
	
	// If the pointer is on the first move of the variation,
	// Jump to the last move of the parent branch
	if (m_Ply == m_Variation->StartingPly + 1)
		m_Variation = m_Variation->Parent;

	m_Ply--;
}

void Game::Forward() {
	// The ply of the last move of the branch
	const uint32_t maxPly = (uint32_t)m_Variation->Moves.size() + m_Variation->StartingPly;

	// If we are at the end of the branch, go to main variation if it exists
	if (m_Ply == maxPly) {
		if (m_Variation->Variations.empty())
			return;

		// Index 0 is the main line
		m_Variation = m_Variation->Variations[0];
	}

	m_Ply++;

	const GameMove& gm = m_Variation->Moves[m_Ply - m_Variation->StartingPly - 1];
	m_Position.Move(LongAlgebraicMove(gm.Start, gm.Destination, gm.Promotion));
}

void Game::Seek(uint32_t ply, Branch* variation) {
}

void Game::Delete(uint32_t ply, Branch* variation) {
}

void Game::Move(GameMove move) {
	const uint32_t maxPly = (uint32_t)m_Variation->Moves.size() + m_Variation->StartingPly;

	// If variations exist and index is on the last move of the list
	// Or if the current branch is the top node
	if (!m_Variation->Variations.empty() && m_Ply == maxPly) {
		// Check if the move is one of them
		for (Branch* var : m_Variation->Variations) {
			if (var->Moves[0] == move) {
				m_Variation = var;
				m_Ply++;
				return;
			}
		}
		
		// Create a new branch for new move
		m_Variation->Variations.push_back(new Branch(m_Variation, m_Ply));
		// Make the current variation the new branch
		m_Variation = m_Variation->Variations.back();
	}
	else if (m_Ply < maxPly) {
		// If the next move is already in the tree, advance
		// the index and return. Otherwise, split the tree
		// TODO: LOOK AT THIS?
		if (m_Ply != 0) {
			if (m_Variation->Moves[m_Ply - m_Variation->StartingPly - 1] == move) {
				m_Ply++;
				return;
			}
		}

		// Reserve space
		m_Variation->Variations.reserve(m_Variation->Variations.capacity() + 2);

		// Create new branch for old moves
		Branch* oldMoves = new Branch(m_Variation, m_Ply);
		// The index of the first move to copy
		size_t offset = m_Ply - m_Variation->StartingPly;
		// Copy the old moves into the new branch
		std::copy(m_Variation->Moves.begin() + offset, m_Variation->Moves.end(), std::back_inserter(oldMoves->Moves));
		// Move old variations to new branch
		if (!m_Variation->Variations.empty())
			oldMoves->Variations = std::move(m_Variation->Variations);
		m_Variation->Variations.push_back(oldMoves);
		// Delete old moves from old branch
		m_Variation->Moves.erase(m_Variation->Moves.begin() + offset, m_Variation->Moves.end());

		// Create new branch for new move
		Branch* newMoves = new Branch(m_Variation, m_Ply);
		// Add variation to old branch
		m_Variation->Variations.push_back(newMoves);
		// Change index to new variation
		m_Variation = newMoves;
	}

	// TODO: Determine castle, en passant, or promotion
	// (Castling and promotion can be determined in the caller
	// functions since it is a different process for both)
	// en passant can be determined here

	m_Variation->Moves.emplace_back(std::move(move));
	m_Ply++;
}


void Game::AddComment(const std::string& comment) const {
	if (!m_Branches)
		return;
	
	m_Variation->Moves[m_Ply - m_Variation->StartingPly - 1].Comment = comment;
}

void Game::AddComment(std::string&& comment) const {
	if (!m_Branches)
		return;
	
	m_Variation->Moves[m_Ply - m_Variation->StartingPly - 1].Comment = std::move(comment);
}

void Game::FromPGN(const std::string& pgn) {
	// Resets the moves
	for (Branch* b : m_Branches->Variations)
		delete b;
	m_Branches->Variations.resize(1);
	Branch* newBranch = new Branch(m_Branches, 0);
	m_Branches->Variations[0] = newBranch;
	m_Variation = newBranch;

	StringParser sp(pgn);
	while (sp.JumpPast("[")) {
		auto key = sp.Next<std::string_view>();
		sp.JumpPast("\"");
		auto value = sp.Next("\"");
		
		if (!(key || value))
			throw InvalidPgnException("Invalid tag in PGN header!");

		if (key.value() == "FEN") {
			m_Position.FromFEN(std::string(value.value()));
			m_Ply = m_Position.GetFullMoves() * 2 - 2 + (m_Position.GetPlayerTurn() == Black);
			m_Variation->StartingPly = m_Ply;
		}

		m_Header[std::string(key.value())] = value.value();
	}
	sp.JumpPast("]");

	const bool checkResult = m_Header.count("Result");

	// Parse the moves
	while (auto m = sp.Next<std::string_view>()) {
		std::string_view move = m.value();
		
		if (checkResult)
			if (move == m_Header.at("Result"))
				break;

		if (move.front() == '(') {
			Back();
			ParseVariation(sp);
			Forward();
		}
		else {
			if (move.back() == '.')
				continue;

			if (move.front() == '{') {
				std::string_view comment = sp.Reread("}").value_or("");
				comment.remove_prefix(1); // Remove the '{' from the beginning
				AddComment(std::string(comment));
				continue;
			}

			Move(AlgebraicMove(move));
		}
	}
}

void Game::ParseVariation(StringParser& sp) {
	uint32_t moveCount = 0;

	while (auto m = sp.Next<std::string_view>()) {
		std::string_view move = m.value();

		if (move.front() == '(') {
			Back();
			ParseVariation(sp);
			Forward();
		}
		else {
			if (move.back() == '.')
				continue;

			if (move.front() == '{') {
				std::string_view comment = sp.Reread("}").value_or("");
				comment.remove_prefix(1); // Remove the '{' from the beginning
				AddComment(std::string(comment));
				continue;
			}

			if (move.back() == ')') {
				move.remove_suffix(1); // Remove the ')' from the end

				Move(AlgebraicMove(move));
				moveCount++;

				for (uint32_t i = 0; i < moveCount; i++)
					Back();

				Forward();

				break;
			}

			Move(AlgebraicMove(move));
			moveCount++;
		}
	}
}

// Don't touch this function; it's very delicate.
// Don't ask how it works it just does
static std::ostream& PrintBranch(std::ostream& os, Board& board, const Branch& branch, bool offset) {
	if ((branch.StartingPly + offset) % 2 == 1 && !offset)
		os << branch.StartingPly / 2 + 1 << "... ";

	// Print the moves of the current branch
	for (uint32_t i = offset; i < branch.Moves.size(); i++) {
		uint32_t ply = branch.StartingPly + i;

		if (ply % 2 == 0)
			os << ply / 2 + 1 << ". ";

		os << board.Move(LongAlgebraicMove(branch.Moves[i].Start, branch.Moves[i].Destination));
		if (!branch.Moves[i].Comment.empty()) {
			os << " {" << branch.Moves[i].Comment << "} ";

			if (i < branch.Moves.size() - 1)
				os << branch.StartingPly / 2 + 2 << "...";
		}

		if (i < branch.Moves.size() - 1 || branch.Variations.size() > 0)
			os << " ";
	}
	
	if (branch.Variations.size() > 0) {
		if (branch.Variations[0]->Moves.empty())
			return os;

		// Print the first move of the first branch (main line)
		uint32_t ply = branch.Variations[0]->StartingPly;
		if (ply % 2 == 0)
			os << ply / 2 + 1 << ".";

		GameMove& move = branch.Variations[0]->Moves[0];
		os << " " << board.Move(LongAlgebraicMove(move.Start, move.Destination)) << " ";
		if (!move.Comment.empty())
			os << " {" << move.Comment << "} ";
		board.UndoMove(move);

		// Print the rest of the branches
		for (uint32_t b = 1; b < branch.Variations.size(); b++) {
			os << "(";

			PrintBranch(os, board, *branch.Variations[b], false);

			// Undo moves on board to reset the position
			// to before the beginning of the branch
			for (size_t j = branch.Variations[b]->Moves.size() - 1; j < branch.Variations[b]->Moves.size(); j--)
				board.UndoMove(branch.Variations[b]->Moves[j]);

			os << ") ";
		}

		// Redo the first move of the first branch
		board.Move(LongAlgebraicMove(move.Start, move.Destination));

		// Print the rest of the first branch (main line)
		PrintBranch(os, board, *branch.Variations[0], true);

		// Undo the moves of the first branch
		for (size_t j = branch.Variations[0]->Moves.size() - 1; j < branch.Variations[0]->Moves.size(); j--)
			board.UndoMove(branch.Variations[0]->Moves[j]);
	}
	
	return os;
}

std::ostream& operator<<(std::ostream& os, const Game& game) {
	// Output the header
	for (const auto& [key, value] : game.m_Header)
		os << "[" << key << " \"" << value << "\"]\n";
	
	if (game.m_Branches) {
		Board board;
		if (game.m_Header.count("FEN")) {
			if (game.m_Branches->Variations[0]->StartingPly % 2 == 1)
				os << "\n" << game.m_Branches->Variations[0]->StartingPly / 2 + 1 << "...";

			board.FromFEN(game.m_Header.at("FEN"));
		}
		PrintBranch(os, board, *game.m_Branches, false);
	}

	return os;
}
