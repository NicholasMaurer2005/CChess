#pragma once

#include <cstdint>

#include "ChessConstants.hpp"

class BitBoard
{
private:

	std::uint64_t m_board;

public:

	//constructors
	constexpr BitBoard() noexcept : m_board() {}

	constexpr BitBoard(std::uint64_t board) noexcept : m_board(board) {}



	//getters
	constexpr std::uint64_t board() const noexcept
	{ 
		return m_board; 
	}

	constexpr bool test(int pos) const noexcept
	{ 
		return static_cast<bool>(m_board & (1ULL << pos)); 
	}

	constexpr bool test(int rank, int file) const noexcept
	{
		const int index{ rank * fileSize + file };
		return static_cast<bool>(m_board & (1ULL << index));
	}

	int leastSignificantBit() const noexcept;

	std::size_t bitCount() const noexcept;



	//setters
	void set(int pos) noexcept;

	void set(int rank, int file) noexcept;

	void reset(int pos) noexcept;

	void reset(int rank, int file) noexcept;

	void resetLeastSignificantBit() noexcept;



	//helpers
	void print() const;
};

