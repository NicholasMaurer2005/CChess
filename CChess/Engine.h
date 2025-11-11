#pragma once

#include <string_view>
#include <atomic>
#include <vector>

#include "ChessConstants.hpp"
#include "MoveGen.h"
#include "State.h"
#include "ThreadPool.h"
#include "KillerMoveHistory.h"


struct SearchInfo
{
	int searchDepth;
	int evaluation;
};

class Engine
{

//private structs
private:

	struct ScoredMove
	{
		Move move;
		int score;
	};

	struct PositionHistroy
	{
		State state;
		Move move;
	};



//private properties
private:

	//engine
	MoveGen m_moveGen; //TODO: maybe make static?
	State m_state;
	bool m_whiteToMove;
	SearchInfo m_info;
	MoveList m_legalMoves;
	Move m_lastMove;
	std::array<char, 64> m_charPosition;

	//search
	bool m_gameOver;
	std::atomic_bool m_stopSearch;
	KillerMoveHistory m_killerMoves;
	int m_searchMilliseconds;

	//history
	std::vector<PositionHistroy> m_history;
	std::size_t m_historyPosition;



//private methods
private:

	//private methods
	std::uint64_t perftRun(int depth, bool white) noexcept;

	void findWhiteSquares(State& state) const noexcept;

	void findBlackSquares(State& state) const noexcept;

	bool makeLegalMove(State& state, bool white, Move move) const noexcept;

	int quiescenceSearch(const State& state, int alpha, int beta, bool white) noexcept;

	int searchRun(const State& state, int depth, int alpha, int beta, bool white) noexcept;

	ScoredMove searchStart(bool white, int depth) noexcept;

	void benchmarkRun(const State& state, std::uint64_t& nodes, std::atomic_bool& stopping, bool white, int depth) noexcept;

	void recordState(Move move) noexcept;



public:

	//constructor
	Engine() noexcept;



	//getters
	std::string stateFen() const noexcept;

	Move search(bool white) noexcept;

	Move search() noexcept;

	const char* getCharPosition() noexcept;

	int searchMilliseconds() const noexcept;

	bool gameOver() const noexcept;

	Move getLastMove() const noexcept;

	const SearchInfo& searchInfo() const noexcept;



	//setters
	void setState(const State& state) noexcept;

	bool makeMove(int source, int destination) noexcept;

	void setSearchMilliseconds(int milliseconds) noexcept;

	void setStartState() noexcept;

	void engineMove(bool white) noexcept;

	void engineMove() noexcept;

	Move moveBackCallback() noexcept;

	Move moveForwardCallback() noexcept;


	
	//perft
	void perft(int depth) noexcept;

	void benchmark(double seconds) noexcept;
};

