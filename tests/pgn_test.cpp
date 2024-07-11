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

bool TestGameTraversal() {
	try {
		std::string gameOfTheCentury = R"(
[Event "Third Rosenwald Trophy"]
[Site "New York, NY USA"]
[Date "1956.10.17"]
[Round "8"]
[White "Donald Byrne"]
[Black "Robert James Fischer"]
[Result "0-1"]
[ECO "D92"]
[PlyCount "82"]
[EventDate "1956.10.07"]
[WhiteElo ""]
[BlackElo ""]

1. Nf3 Nf6 2. c4 g6 3. Nc3 Bg7 4. d4 O-O
(4... d5 5. cxd5 Nxd5 6. e4 (6. Bd2 c5
7. Rc1 Nxc3 8. Bxc3 cxd4) Nxc3 7. bxc3 c5)
5. Bf4 d5 6. Qb3 dxc4 7. Qxc4 c6 8. e4 Nbd7
9. Rd1 Nb6 10. Qc5 Bg4 11. Bg5 Na4 12. Qa3 Nxc3
13. bxc3 Nxe4 14. Bxe7 Qb6 15. Bc4 Nxc3
16. Bc5 Rfe8+ 17. Kf1 Be6 18. Bxb6 Bxc4+
19. Kg1 Ne2+ 20. Kf1 Nxd4+ 21. Kg1 Ne2+
22. Kf1 Nc3+ 23. Kg1 axb6 24. Qb4 Ra4
25. Qxb6 Nxd1 26. h3 Rxa2 27. Kh2 Nxf2
28. Re1 Rxe1 29. Qd8+ Bf8 30. Nxe1 Bd5
31. Nf3 Ne4 32. Qb8 b5 33. h4 h5 34. Ne5 Kg7
35. Kg1 Bc5+ 36. Kf1 Ng3+ 37. Ke1 Bb4+ 38. Kd1 Bb3+
39. Kc1 Ne2+ 40. Kb1 Nc3+ 41. Kc1 Rc2# 0-1
		)";

		Game game = gameOfTheCentury;

		std::cout << "Game of the century:\n" << game << "\n";

		std::cout << "Current position:\n" << game.GetPosition() << "\n";

		std::cout << "Moving to the beginning of the game...\n";
		game.ToBeginning();
		std::cout << "Current position:\n" << game.GetPosition() << "\n";

		std::cout << "Moving to the end of the game...\n";
		game.ToEnd();
		std::cout << "Current position:\n" << game.GetPosition() << "\n";

		std::cout << "-------------------------\n";
		std::cout << "Testing Seek(uint32_t)...\n";

		// Note: Ply to white move = ply * 2 - 1

		std::cout << "Seeking to move 4...\n";
		game.Seek(7);
		std::cout << "Current position:\n" << game.GetPosition() << "\n";

		Branch* mainLine = game.CurrentVariation();

		std::cout << "Going the end of the Grunfeld variation...\n";
		game.Move(AlgebraicMove("d5"));
		game.Seek(14);
		std::cout << "Current position:\n" << game.GetPosition() << "\n";

		Branch* grunfeldVariation = game.CurrentVariation();

		try {
			game.Seek(15);
		} catch (SeekOutOfBoundsException&) {
			std::cout << "Successfully tested safety net!\n";
		}

		std::cout << "Seeking to move 8 in main line...\n";
		game.Seek(0);
		game.Seek(15);
		std::cout << "Current position:\n" << game.GetPosition() << "\n";

		std::cout << "----------------------------------\n";
		std::cout << "Testing Seek(uint32_t, Branch*)...\n";

		std::cout << "Going to move 6 in Grunfeld variation...\n";
		game.Seek(11, grunfeldVariation);
		std::cout << "Current position:\n" << game.GetPosition() << "\n";

		std::cout << "Going to move 41 in main line...\n";
		game.Seek(81, mainLine);
		std::cout << "Current position:\n" << game.GetPosition() << "\n";

		std::cout << "Seeking over multiple branches...\n";
		game.Seek(1, mainLine);
		game.Seek(82, mainLine);
		std::cout << "Current position:\n" << game.GetPosition() << "\n";
	}
	catch (std::exception& e) {
		std::cout << e.what() << "\n";
		return false;
	}

	return true;
}

bool TestGameDelete() {
	try {
		std::string operaGame = R"(
[Event "Paris"]
[Site "Paris FRA"]
[Date "1858.??.??"]
[Round "?"]
[White "Paul Morphy"]
[Black "Duke Karl / Count Isouard"]
[Result "1-0"]
[EventDate "?"]
[ECO "C41"]
[WhiteElo "?"]
[BlackElo "?"]
[PlyCount "33"]

1. e4 e5 2. Nf3 d6 (2... Nc6 3. Bc4 Nf6 4. d4 exd4 5. O-O) (2... Nf6 3. Nxe5 d6 4. Nf3 Nxe4)
3. d4 Bg4 {This is a weak move already. - Fischer} 4. dxe5
Bxf3 5. Qxf3 dxe5 6. Bc4 Nf6 7. Qb3 Qe7 8. Nc3 c6 9. Bg5 {Black is in what's
like a zugzwang position here. He can't develop the queen's knight because the
pawn is hanging, the bishop is blocked because of the queen. - Fischer} 9... b5
10. Nxb5 cxb5 11. Bxb5+ Nbd7 12. O-O-O Rd8 13. Rxd7 Rxd7 4. Rd1 Qe6 15. Bxd7+
Nxd7 {And now for the memorable checkmating combination:} 16. Qb8+ Nxb8 17. Rd8# 1-0)";

		Game game(operaGame);
		
		std::cout << "Game PGN:\n" << game << "\n";

		std::cout << "Deleting from move 16...\n";
		game.Delete(31, game.CurrentVariation());
		std::cout << "Game PGN:\n" << game << "\n";

		std::cout << "Clearing comment...\n";
		game.SetComment("");
		std::cout << "Game PGN:\n" << game << "\n";

		game.Seek(3);
		std::cout << "Deleting main line from ply 4...\n";
		game.Delete(4, game.CurrentVariation());
		std::cout << "Game PGN:\n" << game << "\n";

		// Get pointer for petrov branch
		game.Move(AlgebraicMove("Nf6"));
		Branch* petrov = game.CurrentVariation();
		game.Back();

		std::cout << "Deleting move 4 from petrov variation...\n";
		game.Delete(7, petrov);
		std::cout << "Game PGN: " << game << "\n";

		std::cout << "Going to end and deleting from move 2...\n";
		game.ToEnd();
		game.Delete(3, game.CurrentVariation());
		std::cout << "Game PGN: " << game << "\n";
		std::cout << game.GetPosition() << "\n";
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
	//TestCastling();
	//TestGameTraversal();
	TestGameDelete();
}
