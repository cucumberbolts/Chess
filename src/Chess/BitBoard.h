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

    static constexpr BitBoard KnightAttack(Square square) {
        // . . . . . . . .
        // . . A . B . . .
        // . C . . . D . .
        // . . . N . . . .
        // . E . . . F . .
        // . . G . H . . .
        // . . . . . . . .
        // . . . . . . . .
        BitBoard attackedSquares;
        attackedSquares |= (0b101ull << (square - 17)) & (0xFFull << ((square - 16) & 0xF8ull));  // Calculates A and B
        attackedSquares |= (0b101ull << (square + 15)) & (0xFFull << ((square + 16) & 0xF8ull));  // Calcualtes G and H
        attackedSquares |= (0b10001ull << (square - 10)) & (0xFFull << ((square - 8) & 0xF8ull));  // Calculates C and D
        attackedSquares |= (0b10001ull << (square + 06)) & (0xFFull << ((square + 8) & 0xF8ull));  // Calcualtes E and F

        return attackedSquares;
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
