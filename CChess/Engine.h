#pragma once

#include <string_view>
#include <atomic>

#include "ChessConstants.hpp"
#include "MoveGen.h"
#include "State.h"
#include "ThreadPool.h"


struct SearchInfo {
	int depth;
	int evaluation;
};

class Engine
{
private:

	//engine
	MoveGen m_moveGen; //TODO: maybe make static?
	State m_state;
	bool m_whiteToMove;

	//search
	bool m_gameOver;
	std::atomic_bool m_stopSearch;
	int m_searchDepth;

	//thead pool
	ThreadPool m_threadPool;



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
	void searchRun(const State& state, int& score, int depth, bool white, int alpha, int beta) noexcept;

	Move searchStartAsync(int depth) noexcept;

	Move searchStart(int depth) noexcept;

	Move search() noexcept;



	//game
	void play() noexcept;


	
	//perft
	void perft(int depth) noexcept;

	void printMoves(bool white) noexcept;

	void printMoves(bool white, int depth) noexcept;
};

