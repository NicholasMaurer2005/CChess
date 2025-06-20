#include "PreGen.h"

#include <chrono>
#include <iostream>
#include <vector>
#include <cstdint>
#include <random>
#include <bitset>
#include <concepts>


//static helpers
template<typename T>
requires std::integral<T> && std::is_convertible_v<T, std::size_t>
static std::size_t arrayIndex(T rank, T file)
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

static std::vector<BitBoard> generateOccupancies(BitBoard relavantBitsMask, std::size_t bitCount, std::size_t occupanciesCount)
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

static BitBoard generateBishopAttack(int rank, int file, BitBoard occupancy)
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

static BitBoard generateRookAttack(int rank, int file, BitBoard occupancy)
{
	BitBoard attack;

	bool blocked{};
	for (int newRank{ rank + 1 }; newRank <= 7 && !blocked; ++newRank)
	{
		attack.set(newRank, rank);
		blocked = occupancy.test(newRank, rank);
	}

	blocked = false;
	for (int newRank{ rank - 1 }; newRank >= 0 && !blocked; --newRank)
	{
		attack.set(newRank, rank);
		blocked = occupancy.test(newRank, rank);
	}

	blocked = false;
	for (int newFile{ file + 1 }; newFile <= 7 && !blocked; ++newFile)
	{
		attack.set(file, newFile);
		blocked = occupancy.test(file, newFile);
	}

	blocked = false;
	for (int newFile{ file - 1 }; newFile >= 0 && !blocked; --newFile)
	{
		attack.set(file, newFile);
		blocked = occupancy.test(file, newFile);
	}

	return attack;
}



//constructor
PreGen::PreGen() : 

	//attack tables
	m_whitePawnAttacks(), m_blackPawnAttacks(), m_knightAttacks(), m_kingAttacks(), m_bishopAttacks(), m_rookAttacks(), 

	//magic numbers
	m_bishopRelevantBits(), m_rookRelevantBits(), m_bishopMagics(), m_rookMagics(), m_bishopBitCount(), m_rookBitCount()

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
	generateKightAttacks();
	generateKingAttacks();
	generateBishopAttacks();
	generateRookAttacks();


	const std::chrono::duration<double> elapsed{ std::chrono::high_resolution_clock::now() - start };
	std::cout << "tables generated in " << elapsed.count() << " seconds\n" << std::endl;
}



//magic numbers
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
			for (int newRank{ rank + 1 }; newRank <= 6; ++newRank)
			{
				m_rookRelevantBits[arrayIndex(file, rank)].set(newRank, rank);
			}

			for (int newRank{ rank - 1 }; newRank >= 1; --newRank)
			{
				m_rookRelevantBits[arrayIndex(file, rank)].set(newRank, rank);
			}

			for (int newFile{ file + 1 }; newFile <= 6; ++newFile)
			{
				m_rookRelevantBits[arrayIndex(file, rank)].set(file, newFile);
			}

			for (int newFile{ file - 1 }; newFile >= 1; --newFile)
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
		m_bishopBitCount[i] = bitCount;

		const std::size_t occupanciesCount{ 1ULL << bitCount };
		const std::size_t shift{ boardSize - bitCount };

		const std::vector<BitBoard> occupancies{ generateOccupancies(m_bishopRelevantBits[i], bitCount, occupanciesCount) };

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
		m_rookBitCount[i] = bitCount;

		const std::size_t occupanciesCount{ 1ULL << bitCount };
		const std::size_t shift{ boardSize - bitCount };

		const std::vector<BitBoard> occupancies{ generateOccupancies(m_rookRelevantBits[i], bitCount, occupanciesCount) };

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

void PreGen::generateBishopAttacks()
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			const std::size_t index{ arrayIndex(rank, file) };
			const std::size_t bitCount{ m_bishopRelevantBits[index].bitCount()};
			const std::size_t occupanciesCount{ 1ULL << bitCount };
			const std::size_t shift{ boardSize - bitCount };

			std::vector<BitBoard> occupancies{ generateOccupancies(m_bishopRelevantBits[index], bitCount, occupanciesCount) };

			for (std::size_t i{}; i < occupanciesCount; ++i)
			{
				const BitBoard attack{ generateBishopAttack(rank, file, occupancies[i]) };
				m_bishopAttacks[arrayIndex(index, i)] = attack;
			}
		}
	}
}

void PreGen::generateRookAttacks()
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			const std::size_t index{ arrayIndex(rank, file) };
			const std::size_t bitCount{ m_rookRelevantBits[index].bitCount()};
			const std::size_t occupanciesCount{ 1ULL << bitCount };
			const std::size_t shift{ boardSize - bitCount };

			std::vector<BitBoard> occupancies{ generateOccupancies(m_rookRelevantBits[index], bitCount, occupanciesCount) };

			for (std::size_t i{}; i < occupanciesCount; ++i)
			{
				const BitBoard attack{ generateRookAttack(rank, file, occupancies[i]) };
				m_rookAttacks[arrayIndex(index, i)] = attack;
			}
		}
	}
}



//getters
BitBoard PreGen::whitePawnAttack(std::size_t index) const
{
	return m_whitePawnAttacks[index];
}

BitBoard PreGen::blackPawnAttack(std::size_t index) const
{
	return m_blackPawnAttacks[index];
}

BitBoard PreGen::knightAttack(std::size_t index) const
{
	return m_knightAttacks[index];
}

BitBoard PreGen::bishopAttack(std::size_t index, BitBoard occupancy) const
{
	const std::size_t magic{ m_bishopMagics[index] };
	const std::size_t attackIndex{ (occupancy.board() * magic) << (boardSize - m_bishopBitCount[index])};
	return m_bishopAttacks[arrayIndex(index, attackIndex)];
}

BitBoard PreGen::rookAttack(std::size_t index, BitBoard occupancy) const
{
	const std::size_t magic{ m_rookMagics[index] };
	const std::size_t attackIndex{ (occupancy.board() * magic) << (boardSize - m_rookBitCount[index]) };
	return m_rookAttacks[arrayIndex(index, attackIndex)];
}