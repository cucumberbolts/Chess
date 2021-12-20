#pragma once

#include <ostream>

#include "Move.h"

// Represents a chess board where square 0 is the top left and square 63 is the bottom right.
// Basically a uint64_t with a bunch of functions
struct BitBoard {
public:
    // For the [] operator so you can modify bits in BitBoard
    class BitSquareReference {
    public:
        BitSquareReference(BitBoard* board, size_t index) : m_Reference(board), m_Index(index) {}

        BitSquareReference& operator=(bool value) {
            m_Reference->m_Board ^= (-((int64_t)value) ^ m_Reference->m_Board) & (1ull << m_Index);
            return *this;
        }

        operator bool() const { return(m_Reference->m_Board & (1ull << m_Index)) >> m_Index; }
    private:
        BitBoard* m_Reference;
        size_t m_Index;
    };
public:
    BitBoard() = default;
    constexpr BitBoard(uint64_t b) : m_Board(b) {}
    explicit BitBoard(Square s) : m_Board(1ull << s) {}

    constexpr bool operator[](size_t index) const { return (m_Board & (1ull << index)) >> index; }
    BitSquareReference operator[](size_t index) { return BitSquareReference(this, index); }

    constexpr BitBoard operator~() const { return ~m_Board; }
    constexpr BitBoard operator&(BitBoard other) const { return m_Board & other.m_Board; }
    constexpr BitBoard operator|(BitBoard other) const { return m_Board | other.m_Board; }

    constexpr BitBoard& operator|=(const BitBoard& other) { m_Board |= other.m_Board; return *this; }
    constexpr BitBoard& operator^=(const BitBoard& other) { m_Board |= other.m_Board; return *this; }

    friend std::ostream& operator<<(std::ostream& os, BitBoard b);

    static constexpr BitBoard BishopAttack(Square square) {
        // I have no idea how it works it just does.
        BitBoard attackedSquares;

        const uint32_t row = (square & 0b11111000) >> 3;
        const uint32_t column = square & 0b00000111;

        // if 'square' is in top right half (excluding the centre line)
        if ((1ull << square) & 0x0080C0E0F0F8FCFEull)
            attackedSquares |= 0x8040201008040201ull >> ((column - row) * 8);
        else
            attackedSquares |= 0x8040201008040201ull << ((row - column) * 8);

        // if 'square' is in bottom right half (excluding the centre line)
        //if ((1ull << square) & 0xFEFCF8F0E0C08000ull)
        //    attackedSquares |= 0x0102040810204080ull << (((row + 1) - (8 - column)) * 8);
        //else
        //    attackedSquares |= 0x0102040810204080ull >> (((8 - column) - (row + 1)) * 8);

        // if 'square' is in bottom right half (excluding the centre line)
        if ((1ull << square) & 0xFEFCF8F0E0C08000ull)
            attackedSquares |= 0x0102040810204080ull << ((row + 1 - 8 + column) * 8);
        else
            attackedSquares |= 0x0102040810204080ull >> ((8 - column - row - 1) * 8);

        return attackedSquares;
    }

    static constexpr BitBoard RookAttack(Square square) {
        BitBoard attackedSquares = 0b11111111ull << (square & 0b11111000);  // Calculates the attacked row
        attackedSquares ^= 0x0101010101010101ull << (square & 0b00000111);  // Calculates the attacked column

        return attackedSquares;
    }

    static constexpr BitBoard QueenAttack(Square square) {
        return BishopAttack(square) | RookAttack(square);
    }
private:
    uint64_t m_Board = 0;
};

inline std::ostream& operator<<(std::ostream& os, BitBoard b) {
    for (size_t rank = 0; rank < 8; rank++) {
        for (size_t file = 0; file < 8; file++)
            os << b[rank * 8 + file];

        os << '\n';
    }

    return os;
}
