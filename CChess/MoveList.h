#pragma once

#include <array>

#include "ChessConstants.hpp"
#include "Move.h"

constexpr std::size_t maxLegalMoves{ 218 };
constexpr std::size_t maxLegalCaptures{ 15 };

template<std::size_t listSize>
class cachealign MoveListT 
{
private:

	std::array<Move, listSize> m_moves;
	std::size_t m_back;

public:

	MoveListT() noexcept : m_back() {}

	std::size_t size() const noexcept 
	{
		return m_back;
	}

	void sort() noexcept 
	{
		// sorting logic
	}

	std::array<Move, listSize>::const_iterator begin() const noexcept 
	{
		return m_moves.begin();
	}

	std::array<Move, listSize>::const_iterator end() const noexcept 
	{
		return m_moves.begin() + m_back;
	}

	template<Piece piece>
	void pushQuiet(int sourceIndex, int destinationIndex) noexcept 
	{
		m_moves[m_back] = makeQuiet<piece>(sourceIndex, destinationIndex);
		++m_back;
	}

	template<Piece piece>
	void pushAttack(Piece attackPiece, int sourceIndex, int destinationIndex) noexcept 
	{
		m_moves[m_back] = makeAttack<piece>(attackPiece, sourceIndex, destinationIndex);
		++m_back;
	}

	template<Piece sourcePiece, Piece promotePiece>
	void pushQuietPromote(int sourceIndex, int destinationIndex) noexcept 
	{
		m_moves[m_back] = makeQuietPromote<sourcePiece, promotePiece>(sourceIndex, destinationIndex);
		++m_back;
	}

	template<Piece sourcePiece, Piece promotePiece>
	void pushAttackPromote(Piece attackPiece, int sourceIndex, int destinationIndex) noexcept 
	{
		m_moves[m_back] = makeAttackPromote<sourcePiece, promotePiece>(attackPiece, sourceIndex, destinationIndex);
		++m_back;
	}

	template<Piece sourcePiece, Piece attackPiece>
	void pushEnpassant(int sourceIndex, int destinationIndex, int enpassantIndex) noexcept 
	{
		m_moves[m_back] = makeEnpassant<sourcePiece, attackPiece>(sourceIndex, destinationIndex, enpassantIndex);
		++m_back;
	}

	template<Piece piece>
	void pushDoublePawn(int sourceIndex, int destinationIndex) noexcept 
	{
		m_moves[m_back] = makeDoublePawn<piece>(sourceIndex, destinationIndex);
		++m_back;
	}

	template<Castle castle>
	void pushCastle() 
	{
		constexpr Move castleMove{ 0b00000100000000000000000000000000 | static_cast<std::uint32_t>(castle) };
		m_moves[m_back] = castleMove;
		++m_back;
	}
};


using MoveList = MoveListT<maxLegalMoves>;
using CaptureList = MoveListT<maxLegalCaptures>;
