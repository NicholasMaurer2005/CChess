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

	int m_perftCount;



	//private methods
	void perftRun(int depth, bool white) noexcept;

	bool isWhiteKingInCheck() const noexcept;

	bool isBlackKingInCheck() const noexcept;



public:

	//constructor
	Engine(std::string_view fen) noexcept;


	
	//perft
	void perft(int depth) noexcept;
};

