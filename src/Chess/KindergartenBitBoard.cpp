#include "KindergartenBitBoard.h"

#include <array>

namespace KindergartenBitBoard {

    namespace {
        constexpr BitBoard aFile = 0x0101010101010101;
        constexpr BitBoard bFile = 0x0202020202020202;
        constexpr BitBoard hFile = 0x8080808080808080;
        constexpr BitBoard rank1 = 0x00000000000000FF;
        constexpr BitBoard rank1ToAFile = 0x8040201008040201;
        constexpr BitBoard verticalBitBoardKey = 0x0080402010080402;

        constexpr std::array<BitBoard, 64> diagonals = []() -> std::array<BitBoard, 64>
        {
            std::array<BitBoard, 64> result;

            for (Square s = 0; s < 64; s++) {
                const Square distanceTop = (FileOf(s) + 56 - s) / 8;
                const Square distanceBottom = (s - FileOf(s)) / 8;
                const Square distanceLeft = s - RankOf(s);
                const Square distanceRight = RankOf(s) + 7 - s;

                if ((1ull << s) & 0x0103070F1F3F7FFF)
                    result[s] = 0x0102040810204080 >> (8 * (distanceTop - distanceLeft));
                else
                    result[s] = 0x0102040810204080 << (8 * (distanceBottom - distanceRight));
            }

            return result;
        }();

        constexpr std::array<BitBoard, 64> antiDiagonals = []() -> std::array<BitBoard, 64>
        {
            std::array<BitBoard, 64> result;

            for (Square s = 0; s < 64; s++) {
                const Square distanceTop = (FileOf(s) + 56 - s) / 8;
                const Square distanceBottom = (s - FileOf(s)) / 8;
                const Square distanceLeft = s - RankOf(s);
                const Square distanceRight = RankOf(s) + 7 - s;

                if ((1ull << s) & 0x80C0E0F0F8FCFEFF)
                    result[s] = 0x8040201008040201 >> (8 * (distanceTop - distanceRight));
                else
                    result[s] = 0x8040201008040201 << (8 * (distanceBottom - distanceLeft));
            }

            return result;
        }();

        constexpr std::array<std::array<BitBoard, 64>, 8> files = []() -> auto
        {
            std::array<std::array<BitBoard, 64>, 8> result;

            // loop through the attacker's squares
            for (Square s = 0; s < 8; s++) {
                // loop through the blockers (only the inner 6 bits)
                for (size_t b = 0; b < 128; b += 2) {  // TODO: Get rid of BitBoard class and replace size_t with BitBoard
                    BitBoard attacks = 0;

                    // Calculate the attacked squares when attacker is on square 's' and blockers 'b'
                    for (Square i = s; i < 8; i++) {
                        attacks |= 1ull << i;
                        if (b & (1ull << i))
                            break;
                    }

                    for (Square i = s; i < 8; i--) {
                        attacks |= 1ull << i;
                        if (b & (1ull << i))
                            break;
                    }

                    result[s][b >> 1] = attacks * aFile;
                }
            }

            return result;
        }();

        constexpr std::array<std::array<BitBoard, 64>, 8> aFileAttacks = []() -> auto
        {
            std::array<std::array<BitBoard, 64>, 8> result;

            // loop through the attacker's squares
            for (Square s = 0; s < 8; s++) {
                // loop through the blockers
                for (size_t b = 0; b < 128; b += 2) {
                    BitBoard attacked = files[s][b >> 1] & rank1;
                    result[s][b >> 1] = ((attacked * rank1ToAFile) & hFile) >> 7;
                }
            }

            return result;
        }();

        inline BitBoard HorizontalAttack(Square square, BitBoard blockers) {
            BitBoard relevantBits = rank1 << RankOf(square);
            size_t index = ((blockers & relevantBits) * bFile) >> 58;

            return files[FileOf(square)][index] & relevantBits;
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

            return files[FileOf(square)][index] & relevantBits;
        }

        // From bottom-left to top-right
        inline BitBoard AntiDiagonalAttack(Square square, BitBoard blockers) {
            BitBoard relevantBits = antiDiagonals[square];
            size_t index = ((blockers & relevantBits) * bFile) >> 58;

            return files[FileOf(square)][index] & relevantBits;
        }
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

}
