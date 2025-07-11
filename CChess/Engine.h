#pragma once

#include <string_view>
#include <atomic>

#include "ChessConstants.hpp"
#include "MoveGen.h"
#include "State.h"



class Engine
{
private:

	MoveGen m_moveGen; //TODO: maybe make static?
	State m_state;
	bool m_whiteToMove;

	//search
	std::atomic_bool m_searching;
	bool m_gameOver;



	//private methods
	std::uint64_t perftRun(int depth, bool white) noexcept;

	bool whiteKingInCheck() const noexcept;

	bool blackKingInCheck() const noexcept;

	void findWhiteSquares() noexcept;

	void findBlackSquares() noexcept;

	bool makeLegalMove(State& state, bool white, Move move) noexcept;


public:

	//constructor
	Engine(std::string_view fen, Castle castle = Castle::All) noexcept;



	//search
	void searchRun(State& state, int& score, int depth, bool white, int alpha, int beta) noexcept;

	Move search() noexcept;



	//game
	void play() noexcept;


	
	//perft
	void perft(int depth) noexcept;

	void printMoves(bool white) noexcept;

	void printMoves(bool white, int depth) noexcept;
};

