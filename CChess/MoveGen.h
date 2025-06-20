#pragma once

#include "PreGen.h"
#include "State.h"
#include "MoveList.h"

class MoveGen
{
private:

	static const PreGen m_PreGen;



	//private
	void pawnMoves(const State& state, MoveList& moveList) const noexcept;

	void knightMoves(const State& state, MoveList& moveList) const noexcept;

	void bishopMoves(const State& state, MoveList& moveList) const noexcept;

	void rookMoves(const State& state, MoveList& moveList) const noexcept;

	void queenMoves(const State& state, MoveList& moveList) const noexcept;

	void kingMoves(const State& state, MoveList& moveList) const noexcept;



public:

	MoveGen() noexcept;

	MoveList generateMoves(const State& state) noexcept;
};

