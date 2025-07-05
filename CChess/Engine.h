#pragma once

#include <string_view>

#include "ChessConstants.hpp"
#include "MoveGen.h"
#include "State.h"



class Engine
{
private:

	MoveGen m_moveGen; //TODO: maybe make static?

	State m_state;



	//private methods
	std::uint64_t perftRun(int depth, bool white) noexcept;

	bool whiteKingInCheck() const noexcept;

	bool blackKingInCheck() const noexcept;

	void findWhiteSquares() noexcept;

	void findBlackSquares() noexcept;

	bool makeLegalMove(bool white, Move move, unmakeMoveInfo& info) noexcept;



public:

	//constructor
	Engine(std::string_view fen, Castle castle = Castle::All) noexcept;


	
	//perft
	void perft(int depth) noexcept;

	void printMoves(bool white) noexcept;

	void printMoves(bool white, int depth) noexcept;
};

