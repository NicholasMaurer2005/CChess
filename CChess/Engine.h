#pragma once

#include <string_view>
#include <atomic>

#include "ChessConstants.hpp"
#include "MoveGen.h"
#include "State.h"
#include "ThreadPool.h"
#include "KillerMoveHistory.h"



class Engine
{
private:

	struct ScoredMove
	{
		Move move;
		int score;
	};

	struct EngineInfo
	{
		Move lastMove;
		int searchDepth;
		int evaluation;
	};



	//engine
	MoveGen m_moveGen; //TODO: maybe make static?
	State m_state;
	bool m_whiteToMove;
	EngineInfo m_info;

	//search
	bool m_gameOver;
	std::atomic_bool m_stopSearch;
	KillerMoveHistory m_killerMoves;


	//thead pool
	//ThreadPool m_threadPool;



	//private methods
	std::uint64_t perftRun(int depth, bool white) noexcept;

	void findWhiteSquares(State& state) noexcept;

	void findBlackSquares(State& state) noexcept;

	bool makeLegalMove(State& state, bool white, Move move) noexcept;

	//int quiescenceSearch(const State& state, int alpha, int beta, bool white) noexcept;

	int searchRun(const State& state, int depth, int alpha, int beta, bool white) noexcept;

	ScoredMove searchStart(int depth) noexcept;

public:

	//constructor
	Engine(std::string_view fen, Castle castle = Castle::All) noexcept;



	//search
	//Move searchStartAsync(int depth) noexcept;

	Move search() noexcept;



	//game
	void play() noexcept;


	
	//perft
	void perft(int depth) noexcept;
};

