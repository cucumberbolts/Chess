#include "Game.h"

#include <fstream>
#include <sstream>

Game::Game(const std::filesystem::path& pgn) {
	std::fstream input(pgn);
	std::string data(std::filesystem::file_size(pgn), ' ');
	input.read(data.data(), std::filesystem::file_size(pgn));
	input.close();

	FromPGN(data);
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
}

void Game::Move(GameMove move) {
	Branch* currentBranch = &m_Branches[m_Variation];

	const uint32_t maxPly = (uint32_t)m_Branches[m_Variation].Moves.size() + m_Branches[m_Variation].StartingPly - 1;

	// If variations exist and index is on the last move of the list
	// Or if the current branch is the top node
	if (!m_Branches[m_Variation].Variations.empty() && m_Ply == maxPly) {
		// check if the move is one of them
		for (const uint32_t var : m_Branches[m_Variation].Variations) {
			if (m_Branches[var].Moves[0] == move) {
				m_Variation = var;
				m_Ply++;
				return;
			}
		}

		// If it doesn't exist create new branch for new move
		currentBranch = &m_Branches.emplace_back(m_Variation, m_Ply + 1);
		// Add variation to old branch
		m_Branches[m_Variation].Variations.push_back((uint32_t)m_Branches.size() - 1);
		// Change index to new variation
		m_Variation = (uint32_t)m_Branches.size() - 1;
	}
	else if (m_Ply < maxPly) {
		// If the next move is already in the tree, advance
		// the index and return. Otherwise, split the tree
		if (m_Ply != 0) {
			if (m_Branches[m_Variation].Moves[m_Ply - m_Branches[m_Variation].StartingPly] == move) {
				m_Ply++;
				return;
			}
		}

		// Reserve space
		m_Branches.reserve(m_Branches.capacity() + 2);

		// Create new branch for old moves
		Branch& old = m_Branches.emplace_back(m_Variation, m_Ply + 1);
		// The index of the first move to copy
		size_t offset = m_Ply - m_Branches[m_Variation].StartingPly + 1;
		// Copy the old moves into the new branch
		std::copy(m_Branches[m_Variation].Moves.begin() + offset, m_Branches[m_Variation].Moves.end(), std::back_inserter(old.Moves));
		// Move old variations to new branch
		if (!m_Branches[m_Variation].Variations.empty())
			old.Variations = std::move(m_Branches[m_Variation].Variations);
		// Delete old moves from old branch
		m_Branches[m_Variation].Moves.erase(m_Branches[m_Variation].Moves.begin() + offset, m_Branches[m_Variation].Moves.end());
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

void Game::AddComment(const std::string& comment) {
	if (m_Branches.empty())
		return;

	Branch& branch = m_Branches[m_Variation];
	branch.Moves[m_Ply - branch.StartingPly].Comment = comment;
}

void Game::AddComment(std::string&& comment) {
	if (m_Branches.empty())
		return;

	Branch& branch = m_Branches[m_Variation];
	branch.Moves[m_Ply - branch.StartingPly].Comment = std::move(comment);
}

void Game::FromPGN(const std::string& pgn) {
	StringParser sp(pgn);
	while (sp.JumpPast("[")) {
		auto key = sp.Next<std::string_view>();
		sp.JumpPast("\"");
		auto value = sp.Next("\"");
		
		if (!(key || value))
			throw InvalidPgnException("Invalid tag in PGN header!");

		if (key == "FEN") {
			m_Position.FromFEN(std::string(value.value()));
			bool shouldZero = (m_Position.GetFullMoves() != 1);
			m_Ply = m_Position.GetFullMoves() * shouldZero + (m_Position.GetPlayerTurn() == Black);
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

				break;
			}

			Move(AlgebraicMove(move));
			moveCount++;
		}
	}
}


std::ostream& PrintBranch(std::ostream& os, Board& board, const Game& game, const Branch& branch) {
	os << "Ply: " << branch.StartingPly << ": ";
	
	uint32_t index = 0;
	for (; index < branch.Moves.size(); index++) {
		os << board.Move(LongAlgebraicMove(branch.Moves[index].Start, branch.Moves[index].Destination)) << " ";
		if (!branch.Moves[index].Comment.empty())
			os << "{" << branch.Moves[index].Comment << "} ";
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
	// Output the header
	for (const auto& [key, value] : game.m_Header)
		os << "[" << key << " \"" << value << "\"]\n";

#if 1
	if (!game.m_Branches.empty()) {
		os << "Variation: 0\n";
		Board board;
		if (game.m_Header.count("FEN"))
			board.FromFEN(game.m_Header.at("FEN"));
		PrintBranch(os, board, game, game.m_Branches[0]);
	}
#else // For debugging
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
