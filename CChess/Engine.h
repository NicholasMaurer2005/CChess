#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <string_view>
#include <thread>

#include "ChessConstants.hpp"
#include "KillerMoveHistory.h"
#include "State.h"
#include "StackString.hpp"



class Engine
{
private:

	//	Private Definitions

	//constants
	static constexpr int bestValue{ 9999999 };
	static constexpr int worstValue{ -9999999 };
	static constexpr int checkmateScore{ -999999 };
	static constexpr int maxSearchDepth{ 50 };

	//usings
	using clock = std::chrono::high_resolution_clock;
	using duration = std::chrono::duration<float>;
	using PrincipalVariation = std::array<Move, maxLegalMoves>;



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
	State m_currentState;
	bool m_currentWhiteToMove{ true };
	FenPosition m_fenPosition;
	CharPosition m_charPosition;

	//worker
	std::jthread m_worker;
	std::mutex m_mutex;
	std::condition_variable m_cv;

	//search
	cachealign KillerMoveHistory m_killerMoves;
	cachealign PrincipalVariation m_principalVariation{};
	int m_searchMilliseconds{ 500 };
	int m_currentSearchDepth{};
	std::atomic_bool m_stopSearch{ true };

	//info
	SearchInfo m_searchInfo;
	std::atomic_bool m_newInfo;
	clock::time_point m_searchStart;
	std::uint64_t m_nodeCount;



private:

	//	Private Methods

	int search(const State& state, bool whiteToMove, int depth, int alpha, int beta) noexcept;

	void logSearchInfo() noexcept;
	
	std::string_view principalVariation() noexcept;

	void generateCharPosition() noexcept;	



public:

	//Public Methods
	
	//constructors
	Engine() noexcept;

	~Engine();



	//search
	void startSearch() noexcept;

	void stopSearch() noexcept;

	void searchRun() noexcept;



	//getters
	bool searchInfo(SearchInfo& info) noexcept;

	std::string_view fenPosition() noexcept;

	std::string_view charPosition() noexcept;



	//setters
	void setStartState() noexcept;

	void setPositionFen(std::string_view position) noexcept;

	void setPositionChar(std::string_view position) noexcept;
};