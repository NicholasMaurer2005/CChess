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



class Engine
{
private:

	//	Private Definitions

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

	//constants
	static constexpr int bestValue{ 9999999 };
	static constexpr int worstValue{ -9999999 };
	static constexpr int checkmateScore{ -999999 };

	//usings
	using clock = std::chrono::high_resolution_clock;
	using duration = std::chrono::duration<float>;



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

	//usings
	using FenPosition = HeapString<128>;
	using CharPosition = HeapString<boardSize + 1>;



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
	std::atomic_bool m_stopSearch{ true };
	int m_searchMilliseconds{ 500 };
	KillerMoveHistory m_killerMoves;

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