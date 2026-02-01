#include "CChessGUI.h"

#include <CChess.h>
#include <iostream>
#include <format>
#include <span>



static constexpr int noPiece{ 64 };
static constexpr bool flipped{ false };


/* Private Methods */

void CChessGUI::moveCallback(int source, int destination) noexcept
{
	if (engine_move(source, destination))
	{
		m_whiteToMove = false;
	}

	bufferPosition();
}

void CChessGUI::moveBackCallback() noexcept
{
	
}

void CChessGUI::moveForwardCallback() noexcept
{
	
}

void CChessGUI::bufferPosition() noexcept
{
	const std::string_view position{ engine_get_position_char() };
	m_window.bufferPieces(flipped, position);
}

void CChessGUI::play() noexcept
{
	m_window.bufferBoard(flipped, noPiece, noPiece);
	bufferPosition();

	while (m_window.open())
	{
		if (!m_whiteToMove)
		{
			if (!m_searching)
			{
				m_searching = true;
				engine_start_search();
			}
			else 
			{
				int done{};
				int depth{};
				float nodesPerSecond{};
				float timeRemaining{};
				const char* pv{};
				engine_search_info(&done, &depth, &nodesPerSecond, &timeRemaining, &pv);
				std::cout << std::format("depth: {}  nodesPerSecond: {}  timeRemaining: {}\n", depth, nodesPerSecond, timeRemaining);

				int source{};
				int destination{};

				if (engine_best_move(&source, &destination))
				{
					m_searching = false;
					m_whiteToMove = true;
					engine_move_unchecked(source, destination);
					bufferPosition();
				}
			}
		}

		m_window.draw();
	}
}



/* Public Methods */

//constructor
CChessGUI::CChessGUI()
{
	if (engine_create() == CCHESS_FALSE) throw std::runtime_error("unable to create engine");

	play();
}

CChessGUI::~CChessGUI() noexcept
{
	engine_destroy();
}