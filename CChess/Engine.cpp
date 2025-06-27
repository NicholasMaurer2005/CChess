#include "Engine.h"

#include <iostream>

#include "MoveList.h"
#include "Move.h"



//constructor
Engine::Engine(std::string_view fen) noexcept
	: m_moveGen(), m_state(fen), m_perftCount() { }



//perft
void Engine::perftRun(int depth, bool white) noexcept
{
	if (depth == 0)
	{
		++m_perftCount;
		return;
	}

	const MoveList moves{ m_moveGen.generateMoves(white, m_state) };

	for (Move move : moves)
	{
		State stateCopy{ m_state };

		m_state.makeMove(white, move);

		perftRun(depth - 1, !white);

		//unmake move
		m_state = stateCopy;
	}
}

void Engine::perft(int depth) noexcept
{
	for (int i{ 1 }; i < depth; i++)
	{
		perftRun(true, i);
		
		std::cout << "perft depth " << i << ": " << m_perftCount << '\n';
		m_perftCount = 0;
	}

	std::cout << "done" << std::endl;
}