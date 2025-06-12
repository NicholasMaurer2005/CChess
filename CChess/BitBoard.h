#pragma once

#include <cstdint>

#include "ChessConstants.hpp"

class BitBoard
{
private:

	std::uint64_t m_board;

public:

	//constructors
	constexpr BitBoard() : m_board() {}

	constexpr BitBoard(std::uint64_t board) : m_board() {}



	//getters
	constexpr std::uint64_t board() const
	{ 
		return m_board; 
	}

	constexpr bool test(int pos) const 
	{ 
		return static_cast<bool>(m_board & (1ULL << pos)); 
	}

	constexpr bool test(int rank, int file) const
	{
		const int index{ rank * fileSize + file };
		return static_cast<bool>(m_board & (1ULL << index));
	}

	constexpr int getLeastSignificantBit() const
	{
		unsigned long result{};
		_BitScanForward64(&result, m_board);
		return static_cast<int>(result);
	}



	//setters
	void set(int pos);

	void set(int rank, int file);

	void reset(int pos);

	void reset(int rank, int file);



	//helpers
	void print() const;
};

