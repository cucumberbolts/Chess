#include "Chess/Game.h"

#include <iostream>

bool TestLongAlgebraicMove() {
	Game game;

	try {
		game.Move(LongAlgebraicMove("e2e4"));
		game.Move(LongAlgebraicMove("e7e5"));
		game.Move(LongAlgebraicMove("g1f3"));
		game.Move(LongAlgebraicMove("b8c6"));
		game.Move(LongAlgebraicMove("f1c4"));
		game.Move(LongAlgebraicMove("f8c5"));
	}
	catch (IllegalMoveException&) {
		std::cout << "You idiot!!!\n";
	}

	std::cout << game << "\n";
	std::cout << "Position:\n";
	std::cout << game.GetPosition() << "\n";

	std::cout << "---------------------------------------------\n";

	game.Back();
	game.Back();

	try {
		game.Move(LongAlgebraicMove("f1b5"));
	}
	catch (IllegalMoveException&) {
		std::cout << "You idiot!!!\n";
	}

	std::cout << game << "\n";
	std::cout << "Position:\n";
	std::cout << game.GetPosition() << "\n";

	game.Back();

	try {
		game.Move(LongAlgebraicMove("b1c3"));
	}
	catch (IllegalMoveException&) {
		std::cout << "You idiot!!!\n";
	}

	std::cout << game << "\n";
	std::cout << "Position:\n";
	std::cout << game.GetPosition() << "\n";

	game.Back();
	game.Back();
	game.Back();

	try {
		game.Move(LongAlgebraicMove("d2d4"));
	}
	catch (IllegalMoveException&) {
		std::cout << "You idiot!!!\n";
	}

	std::cout << game << "\n";
	std::cout << "Position:\n";
	std::cout << game.GetPosition() << "\n";

	// Test forward and back
	game.Back();
	game.Back();
	std::cout << "Moving back...\n";
	std::cout << "Position:\n";
	std::cout << game.GetPosition() << "\n";
	game.Forward();
	std::cout << "Moving forward...\n";
	std::cout << "Position:\n";
	std::cout << game.GetPosition() << "\n";

	// Move a move that already exists in the tree (they should be the same)
	std::cout << game << "\n";
	game.Move(LongAlgebraicMove("g1f3"));
	std::cout << game << "\n";

	return true;
}

bool TestAlgebraicMove() {
	Game game1;

	try {
		game1.Move(AlgebraicMove("e4"));
		game1.Move(AlgebraicMove("e5"));
		game1.Move(AlgebraicMove("Nf3"));
		game1.Move(AlgebraicMove("Nc6"));
		game1.Move(AlgebraicMove("Bc4"));
		game1.Move(AlgebraicMove("Bc5"));
		game1.Move(AlgebraicMove("O-O"));
	}
	catch (IllegalMoveException&) {
		std::cout << "You idiot!!!\n";
	}

	std::cout << game1 << "\n";
	std::cout << "Position:\n";
	std::cout << game1.GetPosition().ToFEN() << "\n";

	std::cout << "-----------------------------------------------------------\n";
	std::cout << "Moving back thrice and creating a new variation with Bb5...\n";

	game1.Back();
	game1.Back();
	game1.Back();

	std::cout << game1.GetPosition() << "\n";

	try {
		game1.Move(AlgebraicMove("Bb5"));
	}
	catch (IllegalMoveException&) {
		std::cout << "You idiot!!!\n";
	}

	std::cout << game1 << "\n";
	std::cout << "Position:\n";
	std::cout << game1.GetPosition() << "\n";

	return true;
}

bool TestPgnInput() {
	std::string pgn = R"([Event "F/S Return Match"]
[Site "Belgrade, Serbia JUG"]
[Date "1992.11.04"]
[Round "29"]
[White "Fischer, Robert J."]
[Black "Spassky, Boris V."]
[Result "1/2-1/2"]

1. e4 (1. d4) e5 2. Nf3 Nc6 (2... Nf6 3. Nxe5 d6 (3... Nc6 Nxc6 dxc6) 4. Nf3 Nxe4) (2... d6)
3. Bb5 {This opening is called the Ruy Lopez.} 3... a6 4. Ba4 Nf6 5. O-O Be7
6. Re1 b5 7. Bb3 d6 8. c3 O-O 9. h3 Nb8 10. d4 Nbd7

11. c4 c6 12. cxb5 axb5 13. Nc3 Bb7 14. Bg5 b4 15. Nb1 h6 16. Bh4 c5 17. dxe5
Nxe4 18. Bxe7 Qxe7 19. exd6 Qf6 20. Nbd2 Nxd6 21. Nc4 Nxc4 22. Bxc4 Nb6
23. Ne5 Rae8 24. Bxf7+ Rxf7 25. Nxf7 Rxe1+ 26. Qxe1 Kxf7 27. Qe3 Qg5 28. Qxg5
hxg5 29. b3 Ke6 30. a3 Kd6 31. axb4 cxb4 32. Ra5 Nd5 33. f3 Bc8 34. Kf2 Bf5
35. Ra7 g6 36. Ra6+ Kc5 37. Ke1 Nf4 38. g3 Nxh3 39. Kd2 Kb5 40. Rd6 Kc5 41. Ra6
Nf2 42. g4 Bd3 43. Re6 1/2-1/2
)";

	try {
		Game game(pgn);
		std::cout << game << "\n";
	} catch (InvalidPgnException& e) {
		std::cout << e.what() << "\n";
		return false;
	} catch (std::exception& e) {
		std::cout << e.what() << "\n";
		return false;
	}

	return true;
}

void TestStartingFenPosition() {
	Game startingPosTest;
	std::cout << "Game:\n";
	std::cout << startingPosTest << "\n";

	std::cout << "----------------------------------------------\n";

	std::string pgn = R"([FEN "r1bqk1nr/pppp1ppp/2n5/2b1p3/1PB1P3/5N2/P1PP1PPP/RNBQK2R b KQkq - 0 4"]
4... Bxb4
5. c3 Ba5
)";

	Game game(pgn);

	std::cout << "Game:\n";
	std::cout << game << "\n";

	std::cout << "----------------------------------------------\n";

	pgn = R"([FEN "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3"]
3. Bc4 Bc5 4. b4
)";

	Game game2(pgn);

	std::cout << "Game 2:\n";
	std::cout << game2 << "\n";
}

bool TestEnPassant() {
	// Test en passant

	try {
		Game game("1. e4 h5 2. e5 d5 3. exd6 exd6 4. Nf3 h4 5. g4 hxg3 6. fxg3");

		std::cout << "En passant test:\n" << game << "\n";

		std::cout << "Position:\n" << game.GetPosition() << "\n\n";

		uint32_t plies = game.CurrentPly();

		for (int i = 0; i < plies; i++) {
			std::cout << "Moving back...\n";
			game.Back();
			std::cout << "Position:\n" << game.GetPosition() << "\n\n";
		}
		// Should end up on starting position

		for (int i = 0; i < plies; i++) {
			std::cout << "Moving forward...\n";
			game.Forward();
			std::cout << "Position:\n" << game.GetPosition() << "\n\n";
		}
	} catch (std::exception& e) {
		std::cout << e.what() << "\n";
		return false;
	}

	return true;
}

bool TestPromotion() {
	// Test promotion

	try {
		Game game("1. e4 d5 2. exd5 Qd6 3. Qf3 Qb6 4. d6 f6 5. d7+ Kf7");
		std::cout << "Position before promotion:\n" << game.GetPosition() << "\n";

		game.Move(AlgebraicMove("d8=N+"));
		std::cout << "Position after promotion:\n" << game.GetPosition() << "\n";

		game.Back();
		std::cout << "Position after moving back:\n" << game.GetPosition() << "\n";

		game.Forward();
		std::cout << "Position after moving forward:\n" << game.GetPosition() << "\n";

		std::cout << "Game PGN:\n" << game << "\n";

		std::cout << "-------------------------------------\nTESTING TAKING WITH PROMOTION\n";

		game.Move(AlgebraicMove("d8=R"));
		std::cout << "Position after new variation:\n" << game.GetPosition() << "\n";

		std::cout << "Game PGN:\n" << game << "\n";
	} catch (std::exception& e) {
		std::cout << e.what() << "\n";
		return false;
	}

	return true;
}

bool TestCastling() {
	// Test castling

	try {
		std::string pgn = R"(
1. e4 e5 2. Nf3 Nf6 3. Nc3 Nc6 4. d4 exd4
5. Nxd4 Bb4 (5... Bc5 6. Be3 Bb6 7. Qd2 d6
8. f3 Qe7 9. O-O-O Bd7 10. Bc4 O-O-O 11. Nxc6
Bxc6) 6. Nxc6 bxc6 7. Bd3 O-O 8. O-O d5 9. exd5 cxd5)";

		std::string temp = R"(
1. e4 e5 2. Nf3 Nf6 3. Nc3 Nc6 4. d4 exd4
5. Nxd4 Bb4 6. Nxc6 bxc6 7. Bd3 O-O
)";

		Game game(pgn);

		std::cout << "Position:\n" << game.GetPosition() << "\n";

		std::cout << "Game PGN:\n" << game << "\n";

		std::cout << "------------------------------------------------\n";
		std::cout << "Testing castling with LongAlgebraicMove class...\n";

		Game game2;

		std::array<LongAlgebraicMove, 18> mainLine = {
			"e2e4", "e7e5", "g1f3", "g8f6", "b1c3", "b8c6",
			"d2d4", "e5d4", "f3d4", "f8b4", "d4c6", "b7c6",
			"f1d3", "e8g8", "e1g1", "d7d5", "e4d5", "c6d5",
		};

		for (auto& move : mainLine)
			game2.Move(move);

		std::cout << "Position:\n" << game.GetPosition() << "\n";
		std::cout << "Game PGN:\n" << game2 << "\n";

		// Starting with 5... Bc5
		std::array<LongAlgebraicMove, 13> variation = {
			"f8c5",
			"c1e3", "c5b6", "d1d2", "d7d6", "f2f3", "d8e7",
			"e1c1", "c8d7", "f1c4", "e8c8", "d4c6", "d7c6",
		};

		std::cout << "Going back 9 moves...\n";
		for (int i = 0; i < 9; i++)
			game2.Back();

		std::cout << "Creating new variation...\n";
		for (auto& move : variation)
			game2.Move(move);

		std::cout << "Position:\n" << game.GetPosition() << "\n";
		std::cout << "Game PGN:\n" << game2 << "\n";
	}
	catch (std::exception& e) {
		std::cout << e.what() << "\n";
		return false;
	}

	return true;
}

int main() {
	//TestLongAlgebraicMove();
	//TestAlgebraicMove();
	//TestPgnInput();
	//TestStartingFenPosition();
	//TestEnPassant();
	//TestPromotion();
	TestCastling();
}
