#include "PreGen.h"

#include <chrono>
#include <iostream>
#include <vector>
#include <cstdint>
#include <random>
#include <bitset>


//shared helpers
static std::size_t arrayIndex(int rank, int file)
{
	return static_cast<std::size_t>(rank * fileSize + file);
}

static void setSafe(BitBoard& board, int rank, int file)
{
	if (rank >= 0 && rank < rankSize && file >= 0 && file < fileSize)
	{
		board.set(rank, file);
	}
}



//constructor
PreGen::PreGen() : 

	//attack tables
	m_whitePawnAttacks(), m_blackPawnAttacks(), m_knightAttacks(), m_kingAttacks(), m_bishopAttacks(), m_rookAttacks(), 

	//magic numbers
	m_bishopRelevantBits(), m_rookRelevantBits(), m_bishopMagics(), m_rookMagics()

{
	std::cout << "generating tables\n";
	const auto start{ std::chrono::high_resolution_clock::now() };

	generatePawnAttacks();
	generateKightAttacks();
	generateKingAttacks();

	generateBishopRelevantBits();
	generateRookRelevantBits();
	generateBishopMagics();
	generateRookMagics();

	const std::chrono::duration<double> elapsed{ std::chrono::high_resolution_clock::now() - start };
	std::cout << "tables generated in " << elapsed.count() << " seconds\n" << std::endl;
}



//private
void PreGen::generatePawnAttacks()
{
	//white
	for (int rank{}; rank < rankSize - 1; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			const int newRank{ rank + 1 };
			setSafe(m_whitePawnAttacks[arrayIndex(rank, file)], newRank, file + 1);
			setSafe(m_whitePawnAttacks[arrayIndex(rank, file)], newRank, file - 1);
		}
	}

	//black
	for (int rank{ rankSize - 1 }; rank >= 1; --rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			const int newRank{ rank - 1 };
			setSafe(m_blackPawnAttacks[arrayIndex(rank, file)], newRank, file + 1);
			setSafe(m_blackPawnAttacks[arrayIndex(rank, file)], newRank, file - 1);
		}
	}
}

void PreGen::generateKightAttacks()
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			//north
			setSafe(m_knightAttacks[arrayIndex(rank, file)], rank + 2, file + 1);
			setSafe(m_knightAttacks[arrayIndex(rank, file)], rank + 2, file - 1);
																 
			//east												 
			setSafe(m_knightAttacks[arrayIndex(rank, file)], rank + 1, file + 2);
			setSafe(m_knightAttacks[arrayIndex(rank, file)], rank - 1, file + 2);
																 
			//south												 
			setSafe(m_knightAttacks[arrayIndex(rank, file)], rank - 2, file + 1);
			setSafe(m_knightAttacks[arrayIndex(rank, file)], rank - 2, file - 1);
																 
			//west												 
			setSafe(m_knightAttacks[arrayIndex(rank, file)], rank + 1, file - 2);
			setSafe(m_knightAttacks[arrayIndex(rank, file)], rank - 1, file - 2);
		}
	}
}

void PreGen::generateKingAttacks()
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			setSafe(m_kingAttacks[arrayIndex(rank, file)], rank + 1, file);		//N
			setSafe(m_kingAttacks[arrayIndex(rank, file)], rank + 1, file + 1);	//NE
			setSafe(m_kingAttacks[arrayIndex(rank, file)], rank, file + 1);		//E
			setSafe(m_kingAttacks[arrayIndex(rank, file)], rank - 1, file + 1);	//SE
			setSafe(m_kingAttacks[arrayIndex(rank, file)], rank - 1, file);		//S
			setSafe(m_kingAttacks[arrayIndex(rank, file)], rank - 1, file - 1);	//SW
			setSafe(m_kingAttacks[arrayIndex(rank, file)], rank, file - 1);		//W
			setSafe(m_kingAttacks[arrayIndex(rank, file)], rank + 1, file - 1);	//NW
		}
	}
}											  



//magic numbers
static std::vector<BitBoard> createOccupancy(BitBoard relavantBitsMask, std::size_t bitCount, std::size_t occupanciesCount)
{
	std::vector<BitBoard> occupancies;
	occupancies.reserve(occupanciesCount);

	for (std::size_t i{}; i < occupanciesCount; ++i)
	{
		BitBoard occupancy;
		BitBoard relavantBitsCopy{ relavantBitsMask };

		for (std::size_t square{}; square < bitCount; ++square)
		{
			const int index{ relavantBitsCopy.leastSignificantBit() };

			if (i & (1ULL << square))
			{
				occupancy.set(index);
			}
			
			relavantBitsCopy.resetLeastSignificantBit();
		}

		occupancies.push_back(occupancy);
	}

	return occupancies;
}

static std::uint64_t randomMagic()
{
	static std::mt19937_64 gen{ std::random_device{}() };
	static std::uniform_int_distribution<std::uint64_t> dist;

	return dist(gen) & dist(gen) & dist(gen);
}

void PreGen::generateBishopRelevantBits()
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			for (int newRank{ rank + 1 }, newFile{ file + 1 }; newRank <= 6 && newFile <= 6; ++newRank, ++newFile)
			{
				m_bishopRelevantBits[arrayIndex(rank, file)].set(newRank, newFile);
			}
			for (int newRank{ rank + 1 }, newFile{ file - 1 }; newRank <= 6 && newFile >= 1; ++newRank, --newFile)
			{
				m_bishopRelevantBits[arrayIndex(rank, file)].set(newRank, newFile);
			}
			for (int newRank{ rank - 1 }, newFile{ file - 1 }; newRank >= 1 && newFile >= 1; --newRank, --newFile)
			{
				m_bishopRelevantBits[arrayIndex(rank, file)].set(newRank, newFile);
			}
			for (int newRank{ rank - 1 }, newFile{ file + 1 }; newRank >= 1 && newFile <= 6; --newRank, ++newFile)
			{
				m_bishopRelevantBits[arrayIndex(rank, file)].set(newRank, newFile);
			}
		}
	}
}

void PreGen::generateRookRelevantBits()
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			for (int newRank{ file + 1 }; newRank <= 6; ++newRank)
			{
				m_rookRelevantBits[arrayIndex(file, rank)].set(newRank, rank);
			}

			for (int newRank{ file - 1 }; newRank >= 1; --newRank)
			{
				m_rookRelevantBits[arrayIndex(file, rank)].set(newRank, rank);
			}

			for (int newFile{ rank + 1 }; newFile <= 6; ++newFile)
			{
				m_rookRelevantBits[arrayIndex(file, rank)].set(file, newFile);
			}

			for (int newFile{ rank - 1 }; newFile >= 1; --newFile)
			{
				m_rookRelevantBits[arrayIndex(file, rank)].set(file, newFile);
			}
		}
	}
}

void PreGen::generateBishopMagics()
{
	for (std::size_t i{}; i < boardSize; ++i)
	{
		const std::size_t bitCount{ m_bishopRelevantBits[i].bitCount() };
		const std::size_t occupanciesCount{ 1ULL << bitCount };
		const std::size_t shift{ boardSize - bitCount };

		const std::vector<BitBoard> occupancies{ createOccupancy(m_bishopRelevantBits[i], bitCount, occupanciesCount) };

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

void PreGen::generateRookMagics()
{
	for (std::size_t i{}; i < boardSize; ++i)
	{
		const std::size_t bitCount{ m_rookRelevantBits[i].bitCount() };
		const std::size_t occupanciesCount{ 1ULL << bitCount };
		const std::size_t shift{ boardSize - bitCount };

		const std::vector<BitBoard> occupancies{ createOccupancy(m_rookRelevantBits[i], bitCount, occupanciesCount) };

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



//getters
const std::array<BitBoard, boardSize>& PreGen::whitePawnAttacks() const
{
	return m_whitePawnAttacks;
}

const std::array<BitBoard, boardSize>& PreGen::blackPawnAttacks() const
{
	return m_blackPawnAttacks;
}

const std::array<BitBoard, boardSize>& PreGen::knightAttacks() const
{
	return m_knightAttacks;
}

const std::array<BitBoard, maxBishopAttacks* boardSize>& PreGen::bishopAttacks() const
{
	return m_bishopAttacks;
}

const std::array<BitBoard, maxRookAttacks* boardSize>& PreGen::rookAttacks() const
{
	return m_rookAttacks;
}