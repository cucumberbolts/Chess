#include "Chess/Game.h"

#include <iostream>

int main() {
	Game game;

	try {
		game.Move(LongAlgebraicMove("e2e4"));
		game.Move(LongAlgebraicMove("e7e5"));
		game.Move(LongAlgebraicMove("g1f3"));
		game.Move(LongAlgebraicMove("b8c6"));
		game.Move(LongAlgebraicMove("f1c4"));
		game.Move(LongAlgebraicMove("f8c5"));
	} catch (IllegalMoveException&) {
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
	} catch (IllegalMoveException&) {
		std::cout << "You idiot!!!\n";
	}

	std::cout << game << "\n";
	std::cout << "Position:\n";
	std::cout << game.GetPosition() << "\n";

	game.Back();

	try {
		game.Move(LongAlgebraicMove("b1c3"));
	} catch (IllegalMoveException&) {
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

	///////////////////////////////////////////////////////////////////////
	std::cout << "\n\n################### BOARD 2 ###################\n\n";
	///////////////////////////////////////////////////////////////////////
	
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
}
