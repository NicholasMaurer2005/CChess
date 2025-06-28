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
	occupancies.reserve(occupanciesCount);

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

		occupancies.push_back(occupancy);
	}

	return occupancies;
}

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
void PreGen::generateBishopRelevantBits() noexcept
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			for (int newRank{ rank + 1 }, newFile{ file + 1 }; newRank <= 6 && newFile <= 6; ++newRank, ++newFile)
			{
				m_bishopRelevantBits[boardIndex(rank, file)].set(newRank, newFile);
			}

			for (int newRank{ rank + 1 }, newFile{ file - 1 }; newRank <= 6 && newFile >= 1; ++newRank, --newFile)
			{
				m_bishopRelevantBits[boardIndex(rank, file)].set(newRank, newFile);
			}

			for (int newRank{ rank - 1 }, newFile{ file - 1 }; newRank >= 1 && newFile >= 1; --newRank, --newFile)
			{
				m_bishopRelevantBits[boardIndex(rank, file)].set(newRank, newFile);
			}

			for (int newRank{ rank - 1 }, newFile{ file + 1 }; newRank >= 1 && newFile <= 6; --newRank, ++newFile)
			{
				m_bishopRelevantBits[boardIndex(rank, file)].set(newRank, newFile);
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
				m_rookRelevantBits[boardIndex(rank, file)].set(newRank, file);
			}

			for (int newRank{ rank - 1 }; newRank >= 1; --newRank)
			{
				m_rookRelevantBits[boardIndex(rank, file)].set(newRank, file);
			}

			for (int newFile{ file + 1 }; newFile <= 6; ++newFile)
			{
				m_rookRelevantBits[boardIndex(rank, file)].set(rank, newFile);
			}

			for (int newFile{ file - 1 }; newFile >= 1; --newFile)
			{
				m_rookRelevantBits[boardIndex(rank, file)].set(rank, newFile);
			}
		}
	}
}

void PreGen::generateBishopMagics() noexcept
{
	for (std::size_t i{}; i < boardSize; ++i)
	{
		const std::size_t bitCount{ m_bishopRelevantBits[i].bitCount() };
		m_bishopBitCount[i] = static_cast<int>(bitCount);

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

void PreGen::generateRookMagics() noexcept
{
	for (std::size_t i{}; i < boardSize; ++i)
	{
		const std::size_t bitCount{ m_rookRelevantBits[i].bitCount() };
		m_rookBitCount[i] = static_cast<int>(bitCount);

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

void PreGen::generateKightAttacks() noexcept
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			//north
			setSafe(m_knightAttacks[boardIndex(rank, file)], rank + 2, file + 1);
			setSafe(m_knightAttacks[boardIndex(rank, file)], rank + 2, file - 1);
																 
			//east												 
			setSafe(m_knightAttacks[boardIndex(rank, file)], rank + 1, file + 2);
			setSafe(m_knightAttacks[boardIndex(rank, file)], rank - 1, file + 2);
																 
			//south												 
			setSafe(m_knightAttacks[boardIndex(rank, file)], rank - 2, file + 1);
			setSafe(m_knightAttacks[boardIndex(rank, file)], rank - 2, file - 1);
																 
			//west												 
			setSafe(m_knightAttacks[boardIndex(rank, file)], rank + 1, file - 2);
			setSafe(m_knightAttacks[boardIndex(rank, file)], rank - 1, file - 2);
		}
	}
}

void PreGen::generateKingAttacks() noexcept
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			setSafe(m_kingAttacks[boardIndex(rank, file)], rank + 1, file);		//N
			setSafe(m_kingAttacks[boardIndex(rank, file)], rank + 1, file + 1);	//NE
			setSafe(m_kingAttacks[boardIndex(rank, file)], rank, file + 1);		//E
			setSafe(m_kingAttacks[boardIndex(rank, file)], rank - 1, file + 1);	//SE
			setSafe(m_kingAttacks[boardIndex(rank, file)], rank - 1, file);		//S
			setSafe(m_kingAttacks[boardIndex(rank, file)], rank - 1, file - 1);	//SW
			setSafe(m_kingAttacks[boardIndex(rank, file)], rank, file - 1);		//W
			setSafe(m_kingAttacks[boardIndex(rank, file)], rank + 1, file - 1);	//NW
		}
	}
}				

void PreGen::generateBishopAttacks() noexcept
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			const std::size_t square{ boardIndex(rank, file) };
			const std::size_t bitCount{ m_bishopRelevantBits[square].bitCount()};
			const std::size_t occupanciesCount{ 1ULL << bitCount };
			const std::size_t shift{ boardSize - bitCount };

			std::vector<BitBoard> occupancies{ generateOccupancies(m_bishopRelevantBits[square], bitCount, occupanciesCount) };
			
			for (BitBoard occupancy : occupancies)
			{
				const BitBoard attack{ generateBishopAttack(rank, file, occupancy) };
				const std::size_t magicIndex{ (occupancy.board() * m_bishopMagics[square]) >> shift };
				m_bishopAttacks[bishopMagicIndex(square, magicIndex)] = attack;
			}
		}
	}
}

void PreGen::generateRookAttacks() noexcept
{
	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			const std::size_t square{ boardIndex(rank, file) };
			const std::size_t bitCount{ m_rookRelevantBits[square].bitCount()};
			const std::size_t occupanciesCount{ 1ULL << bitCount };
			const std::size_t shift{ boardSize - bitCount };

			std::vector<BitBoard> occupancies{ generateOccupancies(m_rookRelevantBits[square], bitCount, occupanciesCount) };

			for (BitBoard occupancy : occupancies)
			{
				const BitBoard attack{ generateRookAttack(rank, file, occupancy) };

				const std::size_t magicIndex{ (occupancy.board() * m_rookMagics[square]) >> shift };
				m_rookAttacks[rookMagicIndex(square, magicIndex)] = attack;
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

BitBoard PreGen::knightAttack(std::size_t index) const noexcept
{
	return m_knightAttacks[index];
}

BitBoard PreGen::kingAttack(std::size_t index) const noexcept
{
	return m_kingAttacks[index];
}

BitBoard PreGen::bishopAttack(std::size_t index, BitBoard occupancy) const noexcept
{
	const std::size_t magic{ m_bishopMagics[index] };
	const std::uint64_t relevantBits{ occupancy.board() & m_bishopRelevantBits[index].board() };
	const std::size_t attackIndex{ (relevantBits * magic) >> (boardSize - m_bishopBitCount[index])};

	return m_bishopAttacks[bishopMagicIndex(index, attackIndex)];
}

BitBoard PreGen::rookAttack(std::size_t index, BitBoard occupancy) const noexcept
{
	if (!occupancy.board())
	{
		throw;
	}

	const std::size_t magic{ m_rookMagics[index] };
	const std::uint64_t relevantBits{ occupancy.board() & m_rookRelevantBits[index].board() };
	const std::size_t attackIndex{ (relevantBits * magic) >> (boardSize - m_rookBitCount[index]) };

	return m_rookAttacks[rookMagicIndex(index, attackIndex)];
}