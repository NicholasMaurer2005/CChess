#include "PreGen.h"

#include <chrono>
#include <iostream>
#include <vector>
#include <cstdint>
#include <random>
#include <bitset>
#include <concepts>
#include <algorithm>


//static helpers
template<typename T>
requires std::integral<T> && std::is_convertible_v<T, std::size_t>
static std::size_t boardIndex(T rank, T file) noexcept
{
	return static_cast<std::size_t>(rank * fileSize + file);
}

static std::size_t bishopMagicIndex(std::size_t square, std::size_t magicIndex)
{
	return square * maxBishopAttacks + magicIndex;
}

static std::size_t rookMagicIndex(std::size_t square, std::size_t magicIndex)
{
	return square * maxRookAttacks + magicIndex;
}

static void setSafe(BitBoard& board, int rank, int file) noexcept
{
	if (rank >= 0 && rank < rankSize && file >= 0 && file < fileSize)
	{
		board.set(rank, file);
	}
}

static std::vector<BitBoard> generateOccupancies(BitBoard relavantBitsMask, std::size_t bitCount, std::size_t occupanciesCount) noexcept
{
	std::vector<BitBoard> occupancies;
	occupancies.resize(occupanciesCount);

	for (std::size_t i{}; i < occupanciesCount; ++i)
	{
		BitBoard occupancy;
		BitBoard relavantBitsCopy{ relavantBitsMask };

		for (std::size_t square{}; square < bitCount; ++square)
		{
			const int index{ relavantBitsCopy.popLeastSignificantBit() };

			if (i & (1ULL << square))
			{
				occupancy.set(index);
			}
		}

		occupancies[i] = occupancy;
	}

	return occupancies;
}

//outside of function to remove initialization guard
//namespace rnd
//{
//	static std::mt19937_64 gen{ std::random_device{}() };
//	static std::uniform_int_distribution<std::uint64_t> dist;
//}
//static std::uint64_t randomMagic() noexcept
//{
//	return rnd::dist(rnd::gen) & rnd::dist(rnd::gen) & rnd::dist(rnd::gen);
//}

static std::uint64_t randomMagic() noexcept
{
	static std::mt19937_64 gen{ std::random_device{}() };
	static std::uniform_int_distribution<std::uint64_t> dist;

	return dist(gen) & dist(gen) & dist(gen);
}

static BitBoard generateBishopAttack(int rank, int file, BitBoard occupancy) noexcept
{
	BitBoard attack;

	bool blocked{};
	for (int newRank{ rank + 1 }, newFile{ file + 1 }; newRank <= 7 && newFile <= 7 && !blocked; ++newRank, ++newFile)
	{
		attack.set(newRank, newFile);
		blocked = occupancy.test(newRank, newFile);
	}

	blocked = false;
	for (int newRank{ rank + 1 }, newFile{ file - 1 }; newRank <= 7 && newFile >= 0 && !blocked; ++newRank, --newFile)
	{
		attack.set(newRank, newFile);
		blocked = occupancy.test(newRank, newFile);
	}

	blocked = false;
	for (int newRank{ rank - 1 }, newFile{ file - 1 }; newRank >= 0 && newFile >= 0 && !blocked; --newRank, --newFile)
	{
		attack.set(newRank, newFile);
		blocked = occupancy.test(newRank, newFile);
	}

	blocked = false;
	for (int newRank{ rank - 1 }, newFile{ file + 1 }; newRank >= 0 && newFile <= 7 && !blocked; --newRank, ++newFile)
	{
		attack.set(newRank, newFile);
		blocked = occupancy.test(newRank, newFile);
	}

	return attack;
}

static BitBoard generateRookAttack(int rank, int file, BitBoard occupancy) noexcept
{
	BitBoard attack;

	bool blocked{};
	for (int newRank{ rank + 1 }; newRank <= 7 && !blocked; ++newRank)
	{
		attack.set(newRank, file);
		blocked = occupancy.test(newRank, file);
	}

	blocked = false;
	for (int newRank{ rank - 1 }; newRank >= 0 && !blocked; --newRank)
	{
		attack.set(newRank, file);
		blocked = occupancy.test(newRank, file);
	}

	blocked = false;
	for (int newFile{ file + 1 }; newFile <= 7 && !blocked; ++newFile)
	{
		attack.set(rank, newFile);
		blocked = occupancy.test(rank, newFile);
	}

	blocked = false;
	for (int newFile{ file - 1 }; newFile >= 0 && !blocked; --newFile)
	{
		attack.set(rank, newFile);
		blocked = occupancy.test(rank, newFile);
	}

	return attack;
}



//constructor
PreGen::PreGen() noexcept :

	//attack tables
	m_whitePawnAttacks(), m_blackPawnAttacks(), m_knightMoves(), m_kingMoves(), m_bishopMoves(), m_rookMoves(),

	//magic numbers
	m_bishopMagicData(), m_rookMagicData(), m_bishopMagics(), m_rookMagics()

{
	std::cout << "generating tables\n";
	const auto start{ std::chrono::high_resolution_clock::now() };

	//magic numbers
	generateBishopRelevantBits();
	generateRookRelevantBits();
	generateBishopMagics();
	generateRookMagics();

	//tables
	generatePawnAttacks();
	generateKightMoves();
	generateKingMoves();
	generateBishopMoves();
	generateRookMoves();

	const std::chrono::duration<double> elapsed{ std::chrono::high_resolution_clock::now() - start };
	std::cout << "tables generated in " << elapsed.count() << " seconds\n" << std::endl;
}



//magic numbers
void PreGen::generateBishopRelevantBits() noexcept
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			for (int newRank{ rank + 1 }, newFile{ file + 1 }; newRank <= 6 && newFile <= 6; ++newRank, ++newFile)
			{
				m_bishopMagicData[boardIndex(rank, file)].relevantBits.set(newRank, newFile);
			}

			for (int newRank{ rank + 1 }, newFile{ file - 1 }; newRank <= 6 && newFile >= 1; ++newRank, --newFile)
			{
				m_bishopMagicData[boardIndex(rank, file)].relevantBits.set(newRank, newFile);
			}

			for (int newRank{ rank - 1 }, newFile{ file - 1 }; newRank >= 1 && newFile >= 1; --newRank, --newFile)
			{
				m_bishopMagicData[boardIndex(rank, file)].relevantBits.set(newRank, newFile);
			}

			for (int newRank{ rank - 1 }, newFile{ file + 1 }; newRank >= 1 && newFile <= 6; --newRank, ++newFile)
			{
				m_bishopMagicData[boardIndex(rank, file)].relevantBits.set(newRank, newFile);
			}
		}
	}
}

void PreGen::generateRookRelevantBits() noexcept
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			for (int newRank{ rank + 1 }; newRank <= 6; ++newRank)
			{
				m_rookMagicData[boardIndex(rank, file)].relevantBits.set(newRank, file);
			}

			for (int newRank{ rank - 1 }; newRank >= 1; --newRank)
			{
				m_rookMagicData[boardIndex(rank, file)].relevantBits.set(newRank, file);
			}

			for (int newFile{ file + 1 }; newFile <= 6; ++newFile)
			{
				m_rookMagicData[boardIndex(rank, file)].relevantBits.set(rank, newFile);
			}

			for (int newFile{ file - 1 }; newFile >= 1; --newFile)
			{
				m_rookMagicData[boardIndex(rank, file)].relevantBits.set(rank, newFile);
			}
		}
	}
}

void PreGen::generateBishopMagics() noexcept
{
	for (std::size_t i{}; i < boardSize; ++i)
	{
		const std::size_t bitCount{ m_bishopMagicData[i].relevantBits.bitCount() };
		m_bishopMagicData[i].magicShift = boardSize - static_cast<int>(bitCount);

		const std::size_t occupanciesCount{ 1ULL << bitCount };
		const std::size_t shift{ boardSize - bitCount };

		const std::vector<BitBoard> occupancies{ generateOccupancies(m_bishopMagicData[i].relevantBits, bitCount, occupanciesCount) };

		while (true)
		{
			const std::uint64_t magic{ randomMagic() };
			bool valid{ true };
			std::bitset<maxBishopAttacks> indexes;

			for (BitBoard occupancy : occupancies)
			{
				const std::size_t index{ (occupancy.board() * magic) >> shift };

				if (indexes.test(index))
				{
					valid = false;
					break;
				}
				else
				{
					indexes.set(index);
				}
			}

			if (valid)
			{
				m_bishopMagics[i] = magic;
				break;
			}
		}
	}
}

void PreGen::generateRookMagics() noexcept
{
	for (std::size_t i{}; i < boardSize; ++i)
	{
		const std::size_t bitCount{ m_rookMagicData[i].relevantBits.bitCount() };
		m_rookMagicData[i].magicShift = boardSize - static_cast<int>(bitCount);

		const std::size_t occupanciesCount{ 1ULL << bitCount };
		const std::size_t shift{ boardSize - bitCount };

		const std::vector<BitBoard> occupancies{ generateOccupancies(m_rookMagicData[i].relevantBits, bitCount, occupanciesCount) };
		
		while (true)
		{
			const std::uint64_t magic{ randomMagic() };
			bool valid{ true };
			std::bitset<maxRookAttacks> indexes;

			for (BitBoard occupancy : occupancies)
			{
				const std::size_t index{ (occupancy.board() * magic) >> shift };

				if (indexes.test(index))
				{
					valid = false;
					break;
				}
				else
				{
					indexes.set(index);
				}
			}

			if (valid)
			{
				m_rookMagics[i] = magic;
				break;
			}
		}
	}
}



//tables
void PreGen::generatePawnAttacks() noexcept
{
	//white
	for (int rank{}; rank < rankSize - 1; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			const int newRank{ rank + 1 };
			setSafe(m_whitePawnAttacks[boardIndex(rank, file)], newRank, file + 1);
			setSafe(m_whitePawnAttacks[boardIndex(rank, file)], newRank, file - 1);
		}
	}

	//black
	for (int rank{ rankSize - 1 }; rank >= 1; --rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			const int newRank{ rank - 1 };
			setSafe(m_blackPawnAttacks[boardIndex(rank, file)], newRank, file + 1);
			setSafe(m_blackPawnAttacks[boardIndex(rank, file)], newRank, file - 1);
		}
	}
}

void PreGen::generateKightMoves() noexcept
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			//north
			setSafe(m_knightMoves[boardIndex(rank, file)], rank + 2, file + 1);
			setSafe(m_knightMoves[boardIndex(rank, file)], rank + 2, file - 1);
																 
			//east												 
			setSafe(m_knightMoves[boardIndex(rank, file)], rank + 1, file + 2);
			setSafe(m_knightMoves[boardIndex(rank, file)], rank - 1, file + 2);
																 
			//south												 
			setSafe(m_knightMoves[boardIndex(rank, file)], rank - 2, file + 1);
			setSafe(m_knightMoves[boardIndex(rank, file)], rank - 2, file - 1);
																 
			//west												 
			setSafe(m_knightMoves[boardIndex(rank, file)], rank + 1, file - 2);
			setSafe(m_knightMoves[boardIndex(rank, file)], rank - 1, file - 2);
		}
	}
}

void PreGen::generateKingMoves() noexcept
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			setSafe(m_kingMoves[boardIndex(rank, file)], rank + 1, file);		//N
			setSafe(m_kingMoves[boardIndex(rank, file)], rank + 1, file + 1);	//NE
			setSafe(m_kingMoves[boardIndex(rank, file)], rank, file + 1);		//E
			setSafe(m_kingMoves[boardIndex(rank, file)], rank - 1, file + 1);	//SE
			setSafe(m_kingMoves[boardIndex(rank, file)], rank - 1, file);		//S
			setSafe(m_kingMoves[boardIndex(rank, file)], rank - 1, file - 1);	//SW
			setSafe(m_kingMoves[boardIndex(rank, file)], rank, file - 1);		//W
			setSafe(m_kingMoves[boardIndex(rank, file)], rank + 1, file - 1);	//NW
		}
	}
}				

void PreGen::generateBishopMoves() noexcept
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			const std::size_t square{ boardIndex(rank, file) };
			const std::size_t bitCount{ m_bishopMagicData[square].relevantBits.bitCount() };
			const std::size_t occupanciesCount{ 1ULL << bitCount };
			const std::size_t shift{ boardSize - bitCount };

			std::vector<BitBoard> occupancies{ generateOccupancies(m_bishopMagicData[square].relevantBits, bitCount, occupanciesCount) };
			
			for (BitBoard occupancy : occupancies)
			{
				const BitBoard attack{ generateBishopAttack(rank, file, occupancy) };
				const std::size_t magicIndex{ (occupancy.board() * m_bishopMagics[square]) >> shift };
				m_bishopMoves[bishopMagicIndex(square, magicIndex)] = attack;
			}
		}
	}
}

void PreGen::generateRookMoves() noexcept
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			const std::size_t square{ boardIndex(rank, file) };
			const std::size_t bitCount{ m_rookMagicData[square].relevantBits.bitCount() };
			const std::size_t occupanciesCount{ 1ULL << bitCount };
			const std::size_t shift{ boardSize - bitCount };

			std::vector<BitBoard> occupancies{ generateOccupancies(m_rookMagicData[square].relevantBits, bitCount, occupanciesCount) };

			for (BitBoard occupancy : occupancies)
			{
				const BitBoard attack{ generateRookAttack(rank, file, occupancy) };

				const std::size_t magicIndex{ (occupancy.board() * m_rookMagics[square]) >> shift };
				m_rookMoves[rookMagicIndex(square, magicIndex)] = attack;
			}
		}
	}
}



//getters
BitBoard PreGen::whitePawnAttack(std::size_t index) const noexcept
{
	return m_whitePawnAttacks[index];
}

BitBoard PreGen::blackPawnAttack(std::size_t index) const noexcept
{
	return m_blackPawnAttacks[index];
}

BitBoard PreGen::knightMove(std::size_t index) const noexcept
{
	return m_knightMoves[index];
}

BitBoard PreGen::kingMove(std::size_t index) const noexcept
{
	return m_kingMoves[index];
}

BitBoard PreGen::bishopMove(std::size_t index, BitBoard occupancy) const noexcept
{
	const std::size_t magic{ m_bishopMagics[index] };
	const std::uint64_t relevantBits{ occupancy.board() & m_bishopMagicData[index].relevantBits.board() };
	const std::size_t attackIndex{ (relevantBits * magic) >> m_bishopMagicData[index].magicShift };

	return m_bishopMoves[bishopMagicIndex(index, attackIndex)];
}

BitBoard PreGen::rookMove(std::size_t index, BitBoard occupancy) const noexcept
{
	const std::size_t magic{ m_rookMagics[index] };
	const std::uint64_t relevantBits{ occupancy.board() & m_rookMagicData[index].relevantBits.board() };
	const std::size_t attackIndex{ (relevantBits * magic) >> m_rookMagicData[index].magicShift };

	return m_rookMoves[rookMagicIndex(index, attackIndex)];
}