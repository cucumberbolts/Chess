#include "Chess/Board.h"

#include <iostream>
#include <sstream>

bool TestLegalMove() {
    // Interesting positions:
	// k7/1b5b/2N3N1/8/4K3/3N1N2/8/8 w - - 0 1  // Pinned knights
    // k7/8/8/4Q3/8/4Q1Q1/8/K7 w - - 0 1       // Multiple queens can go to the same square

    Board board("r1bqk1nr/pppp1ppp/2n5/b7/2BpP3/2P2N2/P4PPP/RNBQK2R/ w KQkq - 0 7");

    std::string inputString = "d1b3 d8f6 e1g1 a5b6 e4e5 f6f5 b1d2 g8e7 c3d4 c6d4 f3d4 b6d4 a1b1 d4e5 c1a3 d7d6 d2f3 e8g8 f3e5 f5e5 f1e1";

    try {
        std::istringstream input(inputString);
        std::string move;
        uint32_t iteration = 0;
        while (input >> move) {
            std::cout << "Iteration: " << iteration << "\n";
            LongAlgebraicMove lm(move);
            AlgebraicMove am = board.Move(lm);
            std::cout << "Moving " << am << "\n";
            iteration++;
        }
    }
    catch (IllegalMoveException& e) {
        std::cout << "Illegal move: " << e.move() << "\n";

        return false;
    }

    std::cout << board << std::endl;

    return true;
}

bool TestLegalMove1() {
    Board board2("r1b1k2r/ppp1nppp/3p4/4bq2/2B5/BQ3N2/P4PPP/1R3RK1 b kq - 1 15");
    try {
        board2.Move(LongAlgebraicMove("e8g8"));
    }
    catch (IllegalMoveException& e) {
        std::cout << "Illegal move: " << e.move() << "\n";

        return false;
    }

    return true;
}

bool TestAlgebraicMoveGeneration() {
    Board b;

    // With a pin
    std::cout << b.Move({ E2, E4 }) << std::endl;
    std::cout << b.Move({ E7, E5 }) << std::endl;
    std::cout << b.Move({ G1, F3 }) << std::endl;
    std::cout << b.Move({ B8, C6 }) << std::endl;
    std::cout << b.Move({ F1, B5 }) << std::endl;
    std::cout << b.Move({ D7, D6 }) << std::endl;
    std::cout << b.Move({ B1, C3 }) << std::endl;
    std::cout << b.Move({ G8, E7 }) << std::endl;
    std::cout << b.Move({ B5, C6 }) << std::endl;

    std::cout << "---------------------------------------------------\n";

    // Without a pin
    b.FromFEN("r1bqkbnr/pppp1ppp/2n5/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 3 3");
    std::cout << b << std::endl;
    std::cout << b.Move({ G8, E7 }) << std::endl;

    // Rank stuff
    std::cout << "---------------------------------------------------\n";

    b.FromFEN("1k6/8/8/2N5/8/2N5/8/1K6 w - - 0 1");
    std::cout << b.Move({ C3, E4 }) << std::endl;

    std::cout << "---------------------------------------------------\n";

    b.FromFEN("1k6/8/8/2N5/8/2N3N1/8/1K6 w - - 0 1");
    std::cout << b.Move({ C3, E4 }) << std::endl;

    return true;
}

bool TestAlgebraicMove() {
    Board board("k7/1p5P/5Q2/2P5/5Q1Q/8/8/K7 w - - 0 1");

    std::string inputString = "Q4f5 b5 cxb6 Kb7 Q5f4 Ka8 Qf4g5 Kb7 h8=Q";

    try {
        std::istringstream input(inputString);
        std::string move;
        uint32_t iteration = 0;
        while (input >> move) {
            std::cout << "Iteration: " << iteration << "\n";
            AlgebraicMove m(move);
            LongAlgebraicMove lm = board.Move(m);
            std::cout << "Moving " << lm << "\n";
            iteration++;
        }

        std::cout << board << "\n";
    }
    catch (InvalidAlgebraicMoveException& e) {
        std::cout << "Invalid move: " << e.what() << "\n";
    }
    catch (IllegalMoveException& e) {
        std::cout << "Illegal move: " << e.what() << "\n";

        return false;
    }

    return true;
}

int main() {
    //TestLegalMove();
    //TestLegalMove1();
    TestAlgebraicMove();
    //TestAlgebraicMoveGeneration();
}
