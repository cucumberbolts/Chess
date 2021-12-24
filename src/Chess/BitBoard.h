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

    constexpr operator bool() const { return m_Board; }

    constexpr bool operator[](size_t index) const { return (m_Board & (1ull << index)) >> index; }
    BitSquareReference operator[](size_t index) { return BitSquareReference(this, index); }

    constexpr BitBoard operator~() const { return ~m_Board; }
    constexpr BitBoard operator&(BitBoard other) const { return m_Board & other.m_Board; }
    constexpr BitBoard operator|(BitBoard other) const { return m_Board | other.m_Board; }

    constexpr BitBoard& operator|=(const BitBoard& other) { m_Board |= other.m_Board; return *this; }
    constexpr BitBoard& operator^=(const BitBoard& other) { m_Board |= other.m_Board; return *this; }

    friend std::ostream& operator<<(std::ostream& os, BitBoard b);

    // Generates pseudo-legal moves
    // We could use magic bitboards, but I don't think that extra performace is needed here
    // The attacked squares include the blockers, so they can be removed later
    static BitBoard PawnAttack(Square square, BitBoard blockers, Colour colour);
    static BitBoard KnightAttack(Square square);
    static BitBoard BishopAttack(Square square, BitBoard blockers);
    static BitBoard RookAttack(Square square, BitBoard blockers);
    static BitBoard QueenAttack(Square square, BitBoard blockers);
    static BitBoard KingAttack(Square square);
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
