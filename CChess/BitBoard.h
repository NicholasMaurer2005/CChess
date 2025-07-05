#pragma once

#include <cstdint>

#include "ChessConstants.hpp"

class BitBoard //TODO: define operator overloads AFTERRRRRRR choosing to do it with .board(). rewrite everything
{
private:

	std::uint64_t m_board;

public:

	//constructors
	constexpr BitBoard() noexcept : m_board() {}

	constexpr BitBoard(std::uint64_t board) noexcept : m_board(board) {} //TODO



	//operators
	bool operator== (const BitBoard& other) const noexcept;



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

	std::size_t bitCount() const noexcept;

	int leastSignificantBit() const noexcept;

	int popLeastSignificantBit()noexcept;



	//setters
	void set(int pos) noexcept;

	void set(int rank, int file) noexcept;

	void reset(int pos) noexcept;

	void reset(int rank, int file) noexcept;



	//helpers
	void print() const;
};

