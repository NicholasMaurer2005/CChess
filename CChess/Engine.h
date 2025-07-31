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

//private structs
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



//private properties
private:

	//engine
	MoveGen m_moveGen; //TODO: maybe make static?
	State m_state;
	bool m_whiteToMove;
	EngineInfo m_info;
	MoveList m_legalMoves;

	//search
	bool m_gameOver;
	std::atomic_bool m_stopSearch;
	KillerMoveHistory m_killerMoves;
	int m_searchMilliseconds;



//private methods
private:

	//private methods
	std::uint64_t perftRun(int depth, bool white) noexcept;

	void findWhiteSquares(State& state) noexcept;

	void findBlackSquares(State& state) noexcept;

	bool makeLegalMove(State& state, bool white, Move move) noexcept;

	int quiescenceSearch(const State& state, int alpha, int beta, bool white) noexcept;

	int searchRun(const State& state, int depth, int alpha, int beta, bool white) noexcept;

	ScoredMove searchStart(bool white, int depth) noexcept;



public:

	//constructor
	Engine() noexcept;



	//getters
	std::string stateFen() const noexcept;

	Move search(bool white) noexcept;

	Move search() noexcept;

	void setStartState() noexcept;

	std::string getCharPosition() const noexcept;



	//setters
	void setState(const State& state) noexcept;

	bool makeMove(int source, int destination) noexcept;


	
	//perft
	void perft(int depth) noexcept;
};

