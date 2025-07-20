#pragma once

#include <thread>
#include <array>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <functional>

#include "Move.h"
#include "State.h"

constexpr int myThreadCount{ 16 };

struct SearchTask
{
	const State& state;
	int& score;
	int depth;
	bool white;
	int alpha;
	int beta;
};

class ThreadPool
{
private:

	std::array<std::jthread, myThreadCount> m_threads;
	std::queue<SearchTask> m_tasks;
	std::mutex m_mutex;
	std::condition_variable m_threadCV;
	std::condition_variable m_doneCV;
	std::atomic_bool m_stopping;
	std::atomic_bool m_searchStop;
	int m_taskCount;
	int m_activeThreads;
	std::function<void(const State&, int&, int, bool, int, int)> m_searchRun;

	void worker() noexcept;

public:

	ThreadPool(std::function<void(const State& state, int& score, int depth, bool white, int alpha, int beta)> searchRun) noexcept;

	~ThreadPool() noexcept;

	void assign(const State& state, int& score, int depth, bool white, int alpha, int beta) noexcept;

	void start() noexcept;

	void wait() noexcept;
};

