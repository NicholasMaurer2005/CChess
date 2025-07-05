#include "BitBoard.h"

#include <iostream>
#include <bit>

#include "ChessConstants.hpp"



//operators
bool BitBoard::operator== (const BitBoard& other) const noexcept
{
	return m_board == other.m_board;
}



//getters
int BitBoard::leastSignificantBit() const noexcept
{
	unsigned long result{};
	_BitScanForward64(&result, m_board);
	return static_cast<int>(result);
}

std::size_t BitBoard::bitCount() const noexcept
{
	return std::popcount(m_board);
}

int BitBoard::popLeastSignificantBit() noexcept
{
	const int index{ leastSignificantBit() };
	reset(index);
	return index;
}



//setters
void BitBoard::set(int pos) noexcept
{
	const std::uint64_t bit{ 1ULL << pos };
	m_board |= bit;
}

void BitBoard::set(int rank, int file) noexcept
{
	const int index{ rank * fileSize + file };
	const std::uint64_t bit{ 1ULL << index };
	m_board |= bit;
}

void BitBoard::reset(int index) noexcept
{
	const std::uint64_t bit{ 1ULL << index };
	m_board ^= bit;
}

void BitBoard::reset(int rank, int file) noexcept
{
	const int index{ rank * fileSize + file };
	const std::uint64_t bit{ 1ULL << index };
	m_board ^= bit;
}



//helpers
void BitBoard::print() const
{
	for (int rank{ rankSize - 1 }; rank >= 0; --rank)
	{
		std::cout << (rank + 1) << "  ";

		for (int file{}; file < fileSize; ++file)
		{
			std::cout << test(rank, file) << ' ';
		}

		std::cout << '\n';
	}

	std::cout << "\n   A B C D E F G H\n";
}