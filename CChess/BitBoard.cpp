#include "BitBoard.h"

#include <iostream>
#include <bit>

#include "ChessConstants.hpp"



//getters
int BitBoard::leastSignificantBit() const
{
	unsigned long result{};
	_BitScanForward64(&result, m_board);
	return static_cast<int>(result);
}

std::size_t BitBoard::bitCount() const
{
	return std::popcount(m_board);
}



//setters
void BitBoard::set(int pos)
{
	const std::uint64_t bit{ 1ULL << pos };
	m_board |= bit;
}

void BitBoard::set(int rank, int file)
{
	const int index{ rank * fileSize + file };
	const std::uint64_t bit{ 1ULL << index };
	m_board |= bit;
}

void BitBoard::reset(int index)
{
	const std::uint64_t bit{ 1ULL << index };
	m_board &= ~bit;
}

void BitBoard::reset(int rank, int file)
{
	const int index{ rank * fileSize + file };
	const std::uint64_t bit{ 1ULL << index };
	m_board &= ~bit;
}

void BitBoard::resetLeastSignificantBit()
{
	const int index{ leastSignificantBit() };
	reset(index);
}



//helpers
void BitBoard::print() const
{
	for (int rank{ fileSize - 1 }; rank >= 0; --rank)
	{
		std::cout << (rank + 1) << "  ";

		for (int file{}; file < fileSize; ++file)
		{
			std::cout << test(rank, file) << " ";
		}

		std::cout << "\n";
	}

	std::cout << "\n   A B C D E F G H\n";
}