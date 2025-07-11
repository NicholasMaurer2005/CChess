#include "ThreadPool.h"

#include <functional>

ThreadPool::ThreadPool(std::function<void(const State& state, int& score, int depth, bool white, int alpha, int beta)> searchRun) noexcept
	: m_tasks(), m_mutex(), m_threadCV(), m_doneCV(), m_stopping(), m_taskCount(), m_searchRun(searchRun)
{
	for (auto& thread : m_threads)
	{
		thread = std::jthread(&ThreadPool::worker, this);
	}
}

ThreadPool::~ThreadPool() noexcept
{
	{
		std::lock_guard lock{ m_mutex };
		m_stopping = true;
		m_threadCV.notify_all();
	}

	for (auto& thread : m_threads)
	{
		thread.join();
	}
}

void ThreadPool::assign(const State& state, int& score, int depth, bool white, int alpha, int beta) noexcept
{
	std::lock_guard lock{ m_mutex };
	m_tasks.emplace(state, score, depth, white, alpha, beta);
	++m_taskCount;
}

void ThreadPool::start() noexcept
{
	std::lock_guard lock{ m_mutex };
	m_threadCV.notify_all();
}

void ThreadPool::worker() noexcept
{  
	while (true)  
	{  
		std::unique_lock lock{ m_mutex };  
		m_threadCV.wait(lock, [this] { return !m_tasks.empty() || m_stopping; });

		if (m_tasks.empty() && m_stopping)  
		{  
			return;  
		}  

		const SearchTask task{ std::move(m_tasks.front()) };  
		m_tasks.pop();  
		lock.unlock();  

		m_searchRun(task.state, task.score, task.depth, task.white, task.alpha, task.beta);  

		{
			std::lock_guard lock{ m_mutex };

			--m_taskCount;

			if (m_taskCount == 0)
			{
				m_doneCV.notify_one();
			}
		}
	}  
}

void ThreadPool::wait() noexcept
{
	std::unique_lock lock{ m_mutex };
	m_doneCV.wait(lock, [this] { return m_taskCount == 0; });
}