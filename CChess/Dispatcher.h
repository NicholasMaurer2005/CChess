#pragma once

#include <array>
#include <vector>
#include <atomic>


#include "ChessConstants.hpp"
#include "State.h"



class cachealign Dispatcher
{
private:

	struct alignas(8) MoveString
	{
		std::array<char, 8> data;
	};

	using PrincipalVariation = std::vector<MoveString>;

	struct SearchInfo
	{
		PrincipalVariation principalVariation;
		int ply;
		int evaluation;
		int evalutaions;
		int mates;
		int nodesPerSecond;
	};

private:

	std::atomic_bool m_newMessage{ true };
	bool newState{ false };
	bool doneSearching{ true };
	SearchInfo m_searchInfo{};

public:

	//constructors
	Dispatcher() noexcept;



	//search
	void search(const State* state) noexcept;



	//getters
	bool message(const State* state) noexcept;

	int ply() noexcept;

	int evaluation() noexcept;

	int evaluations() noexcept;

	int mates() noexcept;

	int nodesPerSecond() noexcept;

	//setters
	void setPly(int ply) noexcept;

	void setEvaluation(int evaluation) noexcept;

	void setEvaluations(int evaluations) noexcept;

	void setMates(int mates) noexcept;

	void setNodesPerSecond(int nodesPerSecond) noexcept;
};