#pragma once

#include <thread>

#include "Dispatcher.h"


//release - publish everything before me
//aquire - dont read until after me
//relazed - compiler does what it wants



namespace async
{
	class Engine
	{
	private:

		State m_currentState{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", Castle::All };
		std::jthread m_worker; //initialized in source file

		Engine() noexcept;
	};


	static void worker()
	{

	}

	Engine::Engine() noexcept
		: m_worker(worker)
	{

	}
}



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

	//async
	bool m_stopping;
	bool m_searching;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	Move m_bestMove;
	std::jthread m_worker;



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

	~Engine();


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

	//async
	void startAsyncSearch() noexcept;

	void stopAsyncSearch() noexcept;

	void getAsyncResults() const noexcept;

	bool getAsyncDone(Move& move) noexcept;


	
	//perft
	void perft(int depth) noexcept;

	void benchmark(double seconds) noexcept;
};

