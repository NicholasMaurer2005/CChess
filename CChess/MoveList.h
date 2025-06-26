#pragma once

#include <array>

#include "ChessConstants.hpp"
#include "Move.h"

constexpr std::size_t maxLegalMoves{ 218 };

class cachealign MoveList
{
private:

	std::array<Move, maxLegalMoves> m_moves;
	std::size_t m_back;

public:

	MoveList() noexcept;

	//quiet
	template<Piece piece>
	void pushQuiet(int sourceIndex, int destinationIndex) noexcept
	{
		m_moves[m_back] = makeQuiet<piece>(sourceIndex, destinationIndex);
		++m_back;
	}

	//attack
	template<Piece piece>
	void pushAttack(Piece attackPiece, int sourceIndex, int destinationIndex) noexcept
	{
		m_moves[m_back] = makeAttack<piece>(attackPiece, sourceIndex, destinationIndex);
		++m_back;
	}

	//quiet promote
	template<Piece sourcePiece, Piece promotePiece>
	void pushQuietPromote(int sourceIndex, int destinationIndex) noexcept
	{
		m_moves[m_back] = makeQuietPromote<sourcePiece, promotePiece>(sourceIndex, destinationIndex);
		++m_back;
	}

	//attack promote
	template<Piece sourcePiece, Piece promotePiece>
	void pushAttackPromote(Piece attackPiece, int sourceIndex, int destinationIndex) noexcept
	{
		m_moves[m_back] = makeAttackPromote<sourcePiece, promotePiece>(attackPiece, sourceIndex, destinationIndex);
		++m_back;
	}

	//enpassant
	template<Piece sourcePiece, Piece attackPiece>
	void pushEnpassant(int sourceIndex, int destinationIndex, int enpassantIndex) noexcept//TODO: make more efficient if possible(no conditional constructor)
	{
		m_moves[m_back] = makeEnpassant<sourcePiece, attackPiece>(sourceIndex, destinationIndex, enpassantIndex);
		++m_back;
	}

	//double pawn push
	template<Piece piece>
	void pushQuiet(int sourceIndex, int destinationIndex) noexcept
	{
		m_moves[m_back] = makeDoublePawn<piece>(sourceIndex, destinationIndex);
		++m_back;
	}

	//castle
	template<Castle castle>
	void pushCastle()
	{

	}

	void sort() noexcept;

	std::array<Move, maxLegalMoves>::const_iterator begin() const noexcept;

	std::array<Move, maxLegalMoves>::const_iterator end() const noexcept;
};

