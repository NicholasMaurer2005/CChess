#pragma once

#include <thread>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <array>

#include "State.h"
#include "ChessConstants.hpp"



class Engine
{
public:

	struct SearchInfo
	{
		int depth;
		int evaluation;
		float nodesPerSecond;
		float timeRemaining;
	};

	using CharPosition = std::array<char, boardSize + 1>;

private:

	static constexpr int bestValue{ 9999999 };
	static constexpr int worstValue{ -9999999 };
	static constexpr int checkmateScore{ -999999 };

	using clock = std::chrono::high_resolution_clock;
	using duration = std::chrono::duration<float>;

	//state
	State m_currentState;
	bool m_currentWhiteToMove{ true };
	CharPosition m_charPosition;

	//worker
	std::jthread m_worker;
	std::mutex m_mutex;
	std::condition_variable m_cv;

	//search
	bool m_stopSearch{};
	SearchInfo m_info;
	int m_searchMilliseconds{ 500 };
	clock::time_point m_searchStart;
	std::uint64_t m_nodeCount;



private:

	int search(const State& state, bool whiteToMove, int depth, int alpha, int beta) noexcept;

	void logNodesPerSecond() noexcept;

	void generateCharPosition() noexcept;	


public:

	//constructors
	Engine() noexcept;

	~Engine();

	//search
	void startSearch() noexcept;

	void startSearch(const State& state, bool whiteToMove) noexcept;

	void stopSearch() noexcept;

	void searchRun() noexcept;

	//getters
	SearchInfo info() const noexcept;

	const char* charPosition() noexcept;

	//setters
	void setState(const State& state) noexcept;

	void setStartState() noexcept;
};