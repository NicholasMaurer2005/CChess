#pragma once

#include <thread>
#include <array>
#include <mutex>
#include <queue>

#include "Move.h"
#include "Engine.h"
#include "State.h"

constexpr int myThreadCount{ 16 };

using SearchFunction = Move(Engine::*)(State& state, int depth, bool white, int alpha, int beta);

class ThreadPool
{
private:

	std::array<std::jthread, myThreadCount> m_threads;
	std::queue<SearchFunction> m_tasks;
	std::mutex m_mutex;

public:

	ThreadPool() noexcept;

	void assign(SearchFunction function) noexcept;
};

