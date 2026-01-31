#pragma once

#include <thread>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <array>
#include <atomic>

#include "State.h"
#include "ChessConstants.hpp"



class Engine
{
private:

	template<std::size_t Size>
	class HeapString
	{
	private:
		
		std::array<char, Size> m_data{};

	public:

		HeapString() noexcept
		{
			m_data.back() = '\0';
		}

		const char* data() const noexcept
		{
			return m_data;
		}
	};

public:

	struct SearchInfo
	{
		int depth;
		int evaluation;
		float nodesPerSecond;
		float timeRemaining;
		std::string_view principalVariation;
	};

	using FenPosition = HeapString<128>;
	using CharPosition = HeapString<boardSize + 1>;

private:

	static constexpr int bestValue{ 9999999 };
	static constexpr int worstValue{ -9999999 };
	static constexpr int checkmateScore{ -999999 };

	using clock = std::chrono::high_resolution_clock;
	using duration = std::chrono::duration<float>;

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
	std::atomic_bool m_stopSearch{ true };
	int m_searchMilliseconds{ 500 };
	KillerMoveHistory m_killerMoves;

	//info
	SearchInfo m_searchInfo;
	std::atomic_bool m_newInfo;
	clock::time_point m_searchStart;
	std::uint64_t m_nodeCount;



private:

	int search(const State& state, bool whiteToMove, int depth, int alpha, int beta) noexcept;

	void logSearchInfo() noexcept;
	
	std::string_view principalVariation() noexcept;

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
	bool searchInfo(SearchInfo& info) noexcept;

	std::string_view fenPosition() noexcept;

	std::string_view charPosition() noexcept;



	//setters
	void setStartState() noexcept;

	void setPositionChar(std::string_view position) noexcept;

	void setPositionFen(std::string_view position) noexcept;

};