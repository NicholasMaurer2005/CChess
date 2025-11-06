#pragma once

#include <array>
#include <algorithm>

#include "ChessConstants.hpp"
#include "Move.h"
#include "KillerMoveHistory.h"
#include "Castle.hpp"



constexpr std::size_t maxLegalMoves{ 218 };
constexpr std::size_t maxLegalCaptures{ 30 };



template<std::size_t listSize>
class cachealign MoveListT 
{
private:

	static int moveScore(Move move) noexcept
	{
		static constexpr std::array<int, pieceCount* pieceCount> mvvLva{
			//   X   P   N   B   R   Q   K     p,  n,  b,  r,  q,  k
			/*X*/00, 00, 00, 00, 00, 00, 00,   00, 00, 00, 00, 00, 00,
			/*P*/00, 00, 00, 00, 00, 00, 00,   61, 51, 41, 31, 21, 11,
			/*N*/00, 00, 00, 00, 00, 00, 00,   62, 52, 42, 32, 22, 12,
			/*B*/00, 00, 00, 00, 00, 00, 00,   63, 53, 43, 33, 23, 13,
			/*R*/00, 00, 00, 00, 00, 00, 00,   64, 54, 44, 34, 24, 14,
			/*Q*/00, 00, 00, 00, 00, 00, 00,   65, 55, 45, 35, 25, 15,
			/*K*/00, 00, 00, 00, 00, 00, 00,   00, 00, 00, 00, 00, 00,

			/*p*/00, 61, 51, 41, 31, 21, 11,   00, 00, 00, 00, 00, 00,
			/*n*/00, 62, 52, 42, 32, 22, 12,   00, 00, 00, 00, 00, 00,
			/*b*/00, 63, 53, 43, 33, 23, 13,   00, 00, 00, 00, 00, 00,
			/*r*/00, 64, 54, 44, 34, 24, 14,   00, 00, 00, 00, 00, 00,
			/*q*/00, 65, 55, 45, 35, 25, 15,   00, 00, 00, 00, 00, 00,
			/*k*/00, 00, 00, 00, 00, 00, 00,   00, 00, 00, 00, 00, 00
		};

		const std::size_t sourcePiece{ static_cast<std::size_t>(move.sourcePiece()) };
		const std::size_t capturePiece{ static_cast<std::size_t>(move.attackPiece()) };

		const int score{ mvvLva[capturePiece * pieceCount + sourcePiece] };
		return score + (move.promotePiece() != Piece::NoPiece ? 1 : 0);
	}

	static bool MoveGreater(Move lhs, Move rhs, KillerMoves killerMoves) noexcept
	{
		const bool lhsKiller{ lhs.move() == killerMoves.first.move() || lhs.move() == killerMoves.second.move() };
		const int lhsStaticScore{ moveScore(lhs) };
		const int lhsScore = lhsKiller ? 1000 : lhsStaticScore;

		const bool rhsKiller{ rhs.move() == killerMoves.first.move() || rhs.move() == killerMoves.second.move() };
		const int rhsStaticScore{ moveScore(rhs) };
		const int rhsScore = rhsKiller ? 1000 : rhsStaticScore;

		return lhsScore > rhsScore;
	}



private:

	std::array<Move, listSize>::iterator m_back;
	std::array<Move, listSize> m_moves;



public:

	MoveListT() noexcept : m_back(m_moves.begin()) {}

	std::size_t size() const noexcept 
	{
		return m_back - m_moves.begin();
	}

	void sort(KillerMoves killerMoves) noexcept
	{
		std::sort(m_moves.begin(), m_back, [killerMoves](Move lhs, Move rhs) { return MoveGreater(lhs, rhs, killerMoves); });
	}

	std::array<Move, listSize>::const_iterator begin() const noexcept 
	{
		return m_moves.begin();
	}

	std::array<Move, listSize>::const_iterator end() const noexcept 
	{
		return m_back;
	}



	template<Piece piece>
	void pushQuiet(int sourceIndex, int destinationIndex) noexcept 
	{
		*m_back = makeQuiet<piece>(sourceIndex, destinationIndex);
		++m_back;
	}

	template<Piece piece>
	void pushAttack(Piece attackPiece, int sourceIndex, int destinationIndex) noexcept 
	{
		*m_back = makeAttack<piece>(attackPiece, sourceIndex, destinationIndex);
		++m_back;
	}

	template<Piece sourcePiece, Piece promotePiece>
	void pushQuietPromote(int sourceIndex, int destinationIndex) noexcept 
	{
		*m_back = makeQuietPromote<sourcePiece, promotePiece>(sourceIndex, destinationIndex);
		++m_back;
	}

	template<Piece sourcePiece, Piece promotePiece>
	void pushAttackPromote(Piece attackPiece, int sourceIndex, int destinationIndex) noexcept 
	{
		*m_back = makeAttackPromote<sourcePiece, promotePiece>(attackPiece, sourceIndex, destinationIndex);
		++m_back;
	}

	template<Piece sourcePiece, Piece attackPiece>
	void pushEnpassant(int sourceIndex, int destinationIndex, int enpassantIndex) noexcept 
	{
		*m_back = makeEnpassant<sourcePiece, attackPiece>(sourceIndex, destinationIndex, enpassantIndex);
		++m_back;
	}

	template<Piece piece>
	void pushDoublePawn(int sourceIndex, int destinationIndex) noexcept 
	{
		*m_back = makeDoublePawn<piece>(sourceIndex, destinationIndex);
		++m_back;
	}

	template<Castle castle>
	void pushCastle() 
	{
		constexpr Move castleMove{ 0b00000100000000000000000000000000 | static_cast<std::uint32_t>(castle) };
		*m_back = castleMove;
		++m_back;
	}
};



using MoveList = MoveListT<maxLegalMoves>;
using CaptureList = MoveListT<maxLegalCaptures>;
