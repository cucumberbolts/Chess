#include "PseudoLegal.h"

#include <array>

namespace PseudoLegal {

    namespace {

        constexpr BitBoard aFile = 0x0101010101010101;
        constexpr BitBoard bFile = 0x0202020202020202;
        constexpr BitBoard hFile = 0x8080808080808080;
        constexpr BitBoard rank1 = 0x00000000000000FF;
        constexpr BitBoard rank1ToAFile = 0x8040201008040201;
        constexpr BitBoard verticalBitBoardKey = 0x0080402010080402;

        // Just 10572 bytes of lookup tables...

        /// <summary>
        /// An array of pawn moves for *both sides*.
        ///
        /// BitBoard square = 1 << x;
        /// To get moves for white, & it with ~(square - 1).
        /// To get moves for black, & it with square - 1.
        ///
        /// (subtracting one from a bitboard with one bit set will set all bits below it)
        /// </summary>
        constexpr std::array<BitBoard, 64> pawns = []() -> auto
        {
            std::array<BitBoard, 64> result = { 0 };

            for (Square s = 8; s < 56; s++) {
                // White pawns
                // Diagonal captures
                if (RankOf(s + 8) == RankOf(s + 9) && (s + 9) < 64)
                    result[s] |= 1ull << (s + 9);
                if (RankOf(s + 8) == RankOf(s + 7) && (s + 7) < 64)
                    result[s] |= 1ull << (s + 7);
                // Calculates the square in front of the pawn
                if (s + 8 < 64)
                    result[s] |= 1ull << (s + 8);
                // Calculates two squares in frot of the pawn for only the first push
                if ((1ull << s) & 0x000000000000FF00)
                    result[s] |= 1ull << (s + 16);

                // Black pawns
                // Diagonal captures
                if (RankOf(s - 8) == RankOf(s - 9) && (s - 9) < 64)
                    result[s] |= 1ull << (s - 9);
                if (RankOf(s - 8) == RankOf(s - 7) && (s - 7) < 64)
                    result[s] |= 1ull << (s - 7);
                // Calculates one square in front of the pawn
                if (s - 8 < 64)
                    result[s] |= 1ull << (s - 8);
                // Calculates two squares in frot of the pawn for only the first push
                if ((1ull << s) & 0x00FF000000000000)
                    result[s] |= 1ull << (s - 16);
            }

            return result;
        }();

        constexpr std::array<BitBoard, 64> knights = []() -> auto
        {
            std::array<BitBoard, 64> result = { 0 };

            constexpr int8_t s_KnightSquares[8] = {
                -17, -15, -10, -6, 6, 10, 15, 17
            };

            constexpr int8_t s_Rows[8] = {
                -16, -16, -8, -8, 8, 8, 16, 16
            };

            for (Square s = 0; s < 64; s++) {
                for (size_t i = 0; i < 8; i++) {
                    const Square knightSquare = s + s_KnightSquares[i];

                    if (RankOf(knightSquare) == RankOf(s + s_Rows[i]) && knightSquare < 64)
                        result[s] |= 1ull << knightSquare;
                }
            }

            return result;
        }();

        constexpr std::array<BitBoard, 64> diagonals = []() -> auto
        {
            std::array<BitBoard, 64> result = { 0 };

            for (Square s = 0; s < 64; s++) {
                const Square distanceTop = (FileOf(s) + 56 - s) / 8;
                const Square distanceBottom = (s - FileOf(s)) / 8;
                const Square distanceLeft = s - RankOf(s);
                const Square distanceRight = RankOf(s) + 7 - s;

                if ((1ull << s) & 0x0103070F1F3F7FFF)
                    result[s] = 0x0102040810204080 >> (8 * (distanceTop - distanceLeft));
                else
                    result[s] = 0x0102040810204080 << (8 * (distanceBottom - distanceRight));

                result[s] ^= 1ull << s;
            }

            return result;
        }();

        constexpr std::array<BitBoard, 64> antiDiagonals = []() -> auto
        {
            std::array<BitBoard, 64> result = { 0 };

            for (Square s = 0; s < 64; s++) {
                const Square distanceTop = (FileOf(s) + 56 - s) / 8;
                const Square distanceBottom = (s - FileOf(s)) / 8;
                const Square distanceLeft = s - RankOf(s);
                const Square distanceRight = RankOf(s) + 7 - s;

                if ((1ull << s) & 0x80C0E0F0F8FCFEFF)
                    result[s] = 0x8040201008040201 >> (8 * (distanceTop - distanceRight));
                else
                    result[s] = 0x8040201008040201 << (8 * (distanceBottom - distanceLeft));

                result[s] ^= 1ull << s;
            }

            return result;
        }();

        constexpr std::array<std::array<BitBoard, 64>, 8> rankAttacks = []() -> auto
        {
            std::array<std::array<BitBoard, 64>, 8> result = { 0 };

            // loop through the attacker's squares
            for (Square s = 0; s < 8; s++) {
                // loop through the blockers (only the inner 6 bits)
                for (BitBoard b = 0; b < 128; b += 2) {
                    BitBoard attacks = 0;

                    // Remove the current square from the blockers
                    BitBoard blockers = b & ~(1ull << s);

                    // Calculate the attacked squares when attacker is on square 's' and blockers 'b'
                    for (Square i = s; i < 8; i++) {
                        attacks |= 1ull << i;
                        if (blockers & (1ull << i))
                            break;
                    }

                    for (Square i = s; i < 8; i--) {
                        attacks |= 1ull << i;
                        if (blockers & (1ull << i))
                            break;
                    }

                    result[s][b >> 1] = attacks * aFile;
                }
            }

            return result;
        }();

        constexpr std::array<std::array<BitBoard, 64>, 8> aFileAttacks = []() -> auto
        {
            std::array<std::array<BitBoard, 64>, 8> result = { 0 };

            // loop through the attacker's squares
            for (Square s = 0; s < 8; s++) {
                // loop through the blockers (only the inner 6 bits)
                for (BitBoard b = 0; b < 128; b += 2) {
                    BitBoard attacked = rankAttacks[s][b >> 1] & rank1;
                    result[s][b >> 1] = ((attacked * rank1ToAFile) & hFile) >> 7;
                }
            }

            return result;
        }();

        constexpr std::array<BitBoard, 64> kings = []() -> auto
        {
            std::array<BitBoard, 64> result = { 0 };

            constexpr int8_t s_KingSquares[8] = {
                -9, -8, -7, -1, 1, 7, 8, 9
            };

            constexpr int8_t s_Rows[8] = {
                -8, -8, -8, 0, 0, 8, 8, 8
            };

            for (Square s = 0; s < 64; s++) {
                for (size_t i = 0; i < 8; i++) {
                    const Square kingSquare = s + s_KingSquares[i];

                    if (RankOf(kingSquare) == RankOf(s + s_Rows[i]) && kingSquare < 64)
                        result[s] |= 1ull << kingSquare;
                }
            }

            return result;
        }();

        inline BitBoard HorizontalAttack(Square square, BitBoard blockers) {
            BitBoard relevantBits = rank1 << RankOf(square);
            size_t index = ((blockers & relevantBits) * bFile) >> 58;

            return rankAttacks[FileOf(square)][index] & relevantBits;
        }

        inline BitBoard VerticalAttack(Square square, BitBoard blockers) {
            BitBoard relevantBits = aFile << FileOf(square);
            size_t index = (((blockers & relevantBits) >> FileOf(square)) * verticalBitBoardKey) >> 58;

            return aFileAttacks[7 - (RankOf(square) / 8)][index] << FileOf(square);
        }

        // From top-left to bottom-right
        inline BitBoard DiagonalAttack(Square square, BitBoard blockers) {
            BitBoard relevantBits = diagonals[square];
            size_t index = ((blockers & relevantBits) * bFile) >> 58;

            return rankAttacks[FileOf(square)][index] & relevantBits;
        }

        // From bottom-left to top-right
        inline BitBoard AntiDiagonalAttack(Square square, BitBoard blockers) {
            BitBoard relevantBits = antiDiagonals[square];
            size_t index = ((blockers & relevantBits) * bFile) >> 58;

            return rankAttacks[FileOf(square)][index] & relevantBits;
        }

    } // anonymous namespace

    BitBoard PawnMoves(Square square, Colour colour, BitBoard blockers, Square enPassant) {
        blockers |= 1ull << enPassant;

        BitBoard colourMask;
        if (colour == White) {
            colourMask = ~((1ull << square) - 1);
            blockers |= (blockers << 8) & (1ull << (square + 16));
        } else {
            colourMask = (1ull << square) - 1;
            blockers |= (blockers >> 8) & (1ull << (square - 16));
        }

        BitBoard pawnMoves = pawns[square] & colourMask;

        pawnMoves &= ~(blockers & (aFile << FileOf(square)));
        pawnMoves &= ~(blockers ^ ~(aFile << FileOf(square)));

        return pawnMoves;
    }

    BitBoard PawnAttack(Square square, Colour colour) {
        BitBoard colourMask = (colour == White) ? ~((1ull << square) - 1) : ((1ull << square) - 1);
        BitBoard pawnMoves = pawns[square] & colourMask;
        return pawnMoves & ~(aFile << FileOf(square));
    }

    BitBoard KnightAttack(Square square) {
        return knights[square];
    }

    BitBoard BishopAttack(Square square, BitBoard blockers) {
        return DiagonalAttack(square, blockers) | AntiDiagonalAttack(square, blockers);
    }

    BitBoard RookAttack(Square square, BitBoard blockers) {
        return HorizontalAttack(square, blockers) | VerticalAttack(square, blockers);
    }

    BitBoard QueenAttack(Square square, BitBoard blockers) {
        return BishopAttack(square, blockers) | RookAttack(square, blockers);
    }

    BitBoard KingAttack(Square square) {
        return kings[square];
    }

    BitBoard Line(BitBoard square1, BitBoard square2) {
        // Only count the least significant bit, in case there are multiple bits set
        square1 &= (~square1) + 1;
        square2 &= (~square2) + 1;

        // If one of them is 0, return 0
        if (square1 && square2 == 0)
            return 0;

        BitBoard inBetween = (square1 - 1) ^ (square2 - 1);  // Gets all the squares between square1 and square2
        inBetween |= (square1 | square2);  // Include both squares; the larger one will disappear

        // Gets the relevant squares
        Square s1 = GetSquare(square1);
        Square s2 = GetSquare(square2);

        if (FileOf(s1) == FileOf(s2))
            return inBetween & (aFile << FileOf(s1));
        if (RankOf(s1) == RankOf(s2))
            return inBetween & (rank1 << RankOf(s1));
        if (diagonals[s1] & square2)
            return inBetween & diagonals[s1];
        if (antiDiagonals[s1] & square2)
            return inBetween & antiDiagonals[s1];

        return 0;
    }

}
