#include "Chess/Game.h"

#include <iostream>

bool Test1() {
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

bool Test2() {
	Game game1;

	try {
		game1.Move(AlgebraicMove("e4"));
		game1.Move(AlgebraicMove("e5"));
		game1.Move(AlgebraicMove("Nf3"));
		game1.Move(AlgebraicMove("Nc6"));
		game1.Move(AlgebraicMove("Bc4"));
		game1.Move(AlgebraicMove("Bc5"));
	}
	catch (IllegalMoveException&) {
		std::cout << "You idiot!!!\n";
	}

	std::cout << game1 << "\n";
	std::cout << "Position:\n";
	std::cout << game1.GetPosition() << "\n";

	std::cout << "---------------------------------------------\n";

	game1.Back();
	game1.Back();

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

bool Test3() {
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

void Test4() {
	std::string startingPos = R"([FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"]
1. e4 e5
2. Nf3 Nc6
)";

	Game startingPosTest(startingPos);
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
}

int main() {
	//Test1();	
	//Test2();
	//Test3();
	Test4();
}
