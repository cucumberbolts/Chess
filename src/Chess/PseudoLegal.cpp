#include "PseudoLegal.h"

#include <array>

// Source:
// https://www.chessprogramming.org/Kindergarten_Bitboards
//

namespace {

    constexpr BitBoard A_FILE = 0x0101010101010101;
    constexpr BitBoard B_FILE = 0x0202020202020202;
    constexpr BitBoard RANK_1 = 0x00000000000000FF;
    constexpr BitBoard RANK_1_TO_A_FILE      = 0x8040201008040201;  // A1-H8 diagonal
    constexpr BitBoard VERTICAL_BITBOARD_KEY = 0x0080402010080400;  // C2-H7 diagonal

    // Just 10752 bytes of lookup tables...

    /// <summary>
    /// An array of pawn moves for *both sides*.
    ///
    /// Example: a pawn on pawns[E2] will return the following BitBoard:
    /// 8 00000000
	/// 7 00000000
    /// 6 00000000
    /// 5 00000000
    /// 4 00001000
    /// 3 00011100  <===== white's pawn moves
    /// 2 0000X000  <===== pawn is on square 'X'
    /// 1 00011100  <===== black's pawn moves
    ///   abcdefgh
    ///
    /// To get the squares for the right colour:
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

    // From top-left to bottom-right
    constexpr std::array<BitBoard, 64> antiDiagonals = []() -> auto
    {
        std::array<BitBoard, 64> result = { 0 };

        for (Square s = 0; s < 64; s++) {
            const Square distanceTop = (FileOf(s) + 56 - s) / 8;
            const Square distanceBottom = (s - FileOf(s)) / 8;
            const Square distanceLeft = s - (RankOf(s) * 8);
            const Square distanceRight = (RankOf(s) * 8) + 7 - s;

            if ((1ull << s) & 0x0103070F1F3F7FFF)
                result[s] = 0x0102040810204080ull >> (8 * (distanceTop - distanceLeft));
            else
                result[s] = 0x0102040810204080ull << (8 * (distanceBottom - distanceRight));

            // Get rid of the square the piece is on
            result[s] ^= 1ull << s;
        }

        return result;
    }();

    // From bottom-left to top-right
    constexpr std::array<BitBoard, 64> diagonals = []() -> auto
    {
        std::array<BitBoard, 64> result = { 0 };

        for (Square s = 0; s < 64; s++) {
            const Square distanceTop = (FileOf(s) + 56 - s) / 8;
            const Square distanceBottom = (s - FileOf(s)) / 8;
            const Square distanceLeft = s - (RankOf(s) * 8);
            const Square distanceRight = (RankOf(s) * 8) + 7 - s;

            if ((1ull << s) & 0x80C0E0F0F8FCFEFF)
                result[s] = 0x8040201008040201ull >> (8 * (distanceTop - distanceRight));
            else
                result[s] = 0x8040201008040201ull << (8 * (distanceBottom - distanceLeft));

            // Get rid of the square the piece is on
            result[s] ^= 1ull << s;
        }

        return result;
    }();

    // 64 combinations of blockers (2^6) (we're only intersted in the inner
    // 6 blockers since the outer squares will be counted regardless)
    //
    // 8 possible squares for the piece to be (across the file from a-h)
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

                // Get rid of the square the piece is on
                attacks ^= 1ull << s;

                result[s][b >> 1] = attacks * A_FILE;
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
                // Since we're mapping the bits to the a-file, we need to mirror the square (a5 becomes a4)
                Square mirroredSquare = 7 - s;

                // Copies the data from the 'rankAttacks' array
                BitBoard attacked = rankAttacks[mirroredSquare][b >> 1] & RANK_1;

                // Maps the bits to the a-file where the square h1 corresponds to a1 and a1 corresponds to a8
                result[s][b >> 1] = ((attacked * RANK_1_TO_A_FILE) >> 7) & A_FILE;
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


    //constexpr size_t LOOKUPS_SIZE = sizeof(pawns) + sizeof(knights) + sizeof(antiDiagonals)
	//    + sizeof(diagonals) + sizeof(rankAttacks) + sizeof(aFileAttacks) + sizeof(kings);


    BitBoard HorizontalAttack(Square square, BitBoard blockers) {
        BitBoard relevantBits = BitBoardRank(square);
        size_t blockerIndex = ((blockers & relevantBits) * B_FILE) >> 58;

        return rankAttacks[FileOf(square)][blockerIndex] & relevantBits;
    }

    BitBoard VerticalAttack(Square square, BitBoard blockers) {
        size_t blockerIndex = (((blockers & BitBoardFile(square)) >> FileOf(square)) * VERTICAL_BITBOARD_KEY) >> 58;

        return aFileAttacks[RankOf(square)][blockerIndex] << FileOf(square);
    }
    
    // From bottom-left to top-right
    BitBoard DiagonalAttack(Square square, BitBoard blockers) {
        BitBoard relevantBits = diagonals[square];
        size_t index = ((blockers & relevantBits) * B_FILE) >> 58;
    
        return rankAttacks[FileOf(square)][index] & relevantBits;
    }

    // From top-left to bottom-right
    BitBoard AntiDiagonalAttack(Square square, BitBoard blockers) {
        BitBoard relevantBits = antiDiagonals[square];
        size_t index = ((blockers & relevantBits) * B_FILE) >> 58;

        return rankAttacks[FileOf(square)][index] & relevantBits;
    }

} // anonymous namespace



namespace PseudoLegal {

    BitBoard PawnMoves(Square square, Colour colour, BitBoard blockers, Square enPassant) {
        // Since the 'pawns' bitboard returns the pawn moves
        // for both colours, a mask is used to get only the
        // moves according to the colour that is moving
        BitBoard colourMask = (1ull << square) - 1;

        if (colour == White) {
            colourMask = ~colourMask;
            blockers |= (blockers << 8) & (1ull << (square + 16));
        } else {
            blockers |= (blockers >> 8) & (1ull << (square - 16));
        }

        BitBoard pawnMoves = pawns[square] & colourMask;

        // Add the en-passant square to the list of moves
        // The square doesn't actually block the pawn, which is
        // why it is added after the above if-statement
        // (Blockers are attacked by pawns)
        blockers |= 1ull << enPassant;

        pawnMoves &= ~(blockers & BitBoardFile(square));
        pawnMoves &= ~(blockers ^ ~BitBoardFile(square));

        return pawnMoves;
    }

    BitBoard PawnAttack(Square square, Colour colour) {
        // Same as this
        //BitBoard colourMask = (colour == White) ? ~((1ull << square) - 1) : ((1ull << square) - 1);
        BitBoard colourMask = ~(colour * 0xFFFFFFFFFFFFFFFF) ^ ((1ull << square) - 1);
        BitBoard pawnMoves = pawns[square] & colourMask;
        return pawnMoves & ~BitBoardFile(square);
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
            return inBetween & BitBoardFile(s1);
        if (RankOf(s1) == RankOf(s2))
            return inBetween & BitBoardRank(s1);
        if (diagonals[s1] & square2)
            return inBetween & diagonals[s1];
        if (antiDiagonals[s1] & square2)
            return inBetween & antiDiagonals[s1];

        return 0;
    }

} // namespace PseudoLegal
