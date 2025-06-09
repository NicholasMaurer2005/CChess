#include "BitBoard.h"

#include <iostream>

#include "ChessConstants.hpp"



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