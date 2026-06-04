#pragma once

#include <array>
#include <utility>

#include "ChessConstants.hpp"
#include "Move.h"



class cachealign KillerMoveHistory
{
private:

	//	Private Definitions

	static constexpr int plyMovesCount{ 2 };

	struct KillerMoveData
	{
		std::size_t position;
		std::array<Move, plyMovesCount> moves;
	};



private:

	//	Private Members

	std::array<KillerMoveData, maxSearchDepth> m_moves;



public:

	//	Public Methods

	//constructors
	KillerMoveHistory() noexcept;



	//getters
	std::pair<Move, Move> killerMoves(int depth) const noexcept;



	//setters
	void push(int depth, Move move) noexcept;
};