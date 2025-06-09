#include "PreGen.h"

#include <chrono>
#include <iostream>


//static helpers
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

	const std::chrono::duration<double> elapsed{ std::chrono::high_resolution_clock::now() - start };
	std::cout << "tables generated in " << elapsed.count() << "seconds\n" << std::endl;
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
	for (int rank{}; rank < rankSize; rank++)
	{
		for (int file{}; file < fileSize; file++)
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

void PreGen::generateBishopRelevantBits()
{
	for (int rank{}; rank < rankSize; rank++)
	{
		for (int file{}; file < fileSize; file++)
		{
			for (int newRank{ rank + 1 }, int newFile{ file + 1 }; newRank <= 6 && newFile <= 6; newRank++, newFile++)
			{
				m_bishopRelevantBits[arrayIndex(rank, file)].set(newRank, newFile);
			}
			for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) m_bishopRelevantBits[indexAttackTable(tr, tf)].set_rf(r, f);
			for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) m_bishopRelevantBits[indexAttackTable(tr, tf)].set_rf(r, f);
			for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) m_bishopRelevantBits[indexAttackTable(tr, tf)].set_rf(r, f);
		}
	}
}

void PreGen::generateRookRelevantBits()
{

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