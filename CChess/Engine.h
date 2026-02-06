#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <string_view>
#include <thread>
#include <utility>

#include "ChessConstants.hpp"
#include "KillerMoveHistory.h"
#include "Move.h"
#include "MoveGen.h"
#include "MoveList.hpp"
#include "StackString.hpp"
#include "State.h"



class Engine
{
private:

	//	Private Definitions

	//constants
	static constexpr int bestValue{ 9999999 };
	static constexpr int worstValue{ -9999999 };
	static constexpr int checkmateScore{ -999999 };
	static constexpr int maxSearchDepth{ 50 };
	static constexpr int maxMoveStringSize{ 5 };
	static constexpr int maxHalfMoveCount{ 100 };

	//types
	struct HistoryPosition
	{
		State state;
		bool whiteToMove;
		int moveSource;
		int moveDestination;
	};


	//usings
	using clock = std::chrono::high_resolution_clock;
	using PrincipalVariation = std::array<Move, maxSearchDepth>;
	using PrincipalVariationString = StackString<maxSearchDepth * maxMoveStringSize>;
	using StateHistory = std::array<HistoryPosition, maxHalfMoveCount>;



public:

	//	Public Definitions

	struct SearchInfo
	{
		int depth;
		int evaluation;
		float nodesPerSecond;
		float timeRemaining;
		std::string_view principalVariation;
	};



private:

	//	Private Members
	
	//state
	StateHistory m_history{};
	StateHistory::iterator m_currentState{ m_history.begin() };
	StateHistory::iterator m_historyBack{ m_history.begin() + 1 };
	State::FenPosition m_fenPosition;
	State::CharPosition m_charPosition;
	MoveList m_currentLegalMoves{ MoveGen::generateMoves(m_currentState->whiteToMove, m_currentState->state) };

	//worker
	std::mutex m_mutex;
	std::condition_variable m_cv;
	std::jthread m_worker;

	//search
	cachealign KillerMoveHistory m_killerMoves;
	cachealign PrincipalVariation m_principalVariation{};
	int m_searchMilliseconds{ 500 };
	int m_currentSearchDepth{};
	std::atomic_bool m_stopSearch{ true };

	//info
	SearchInfo m_searchInfo{};
	std::atomic_bool m_newInfo;
	clock::time_point m_searchStart;
	std::uint64_t m_nodeCount{};
	PrincipalVariationString m_pvString{};
	Move m_bestMove{ 0 };



private:

	//	Private Methods

	int search(const State& state, int color, int depth, int alpha, int beta) noexcept;

	void logSearchInfo() noexcept;
	
	std::string_view principalVariation() noexcept;



public:

	//	Public Methods
	
	//constructors
	Engine() noexcept;

	~Engine();



	//search
	void startSearch(bool whiteToMove) noexcept;

	void stopSearch() noexcept;

	void searchRun() noexcept;



	//getters
	bool searchInfo(SearchInfo& info) noexcept;

	std::string_view fenPosition() noexcept;

	std::string_view charPosition() noexcept;

	Move bestMove() const noexcept;

	std::pair<int, int> lastMove() noexcept;



	//setters
	bool setPositionFen(std::string_view position) noexcept;

	bool setPositionChar(std::string_view position) noexcept;

	bool move(bool white, int source, int destination) noexcept;

	bool moveForward() noexcept;

	bool moveBack() noexcept;

	void reset() noexcept;
};