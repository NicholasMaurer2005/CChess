#include "CChessGUI.h"

#include <CChess.h>
#include <memory>
#include <thread>
#include <atomic>
#include <iostream>
#include <format>


//constants
constexpr int defaultWindowWidth{ 1000 };
constexpr int defaultWindowHeight{ 1000 };



//static helpers



/* Private Methods */

void CChessGUI::moveCallback(int source, int destination) noexcept
{
	if (engine_move(source, destination))
	{
		m_whiteToMove = false;
	}

	bufferPosition();
}

void CChessGUI::bufferPosition() noexcept
{
	std::unique_ptr<const char> position{ engine_get_char_position() };
	m_window.bufferPieces(false, std::span(position.get(), 64));
	m_window.draw();
}

void CChessGUI::play() noexcept
{
	engine_set_search_milliseconds(500);
	bufferPosition();

	while (m_window.open())
	{
		if (!m_whiteToMove)
		{
			engine_search_and_move();

			int depth{};
			int evaluation{};
			engine_search_info(&depth, &evaluation);
			std::cout << std::format("depth: {}, evaluation: {}\n", depth, evaluation);

			bufferPosition();
			m_whiteToMove = true;
		}

		m_window.draw();
	}
}



/* Public Methods */

//constructor
CChessGUI::CChessGUI() noexcept
	: m_window(defaultWindowWidth, defaultWindowHeight, [this](int width, int height) { this->moveCallback(width, height); }), 
	m_newPosition(true), m_whiteToMove(true)
{
	engine_create();
	engine_perft(6);

	//play();
}

CChessGUI::~CChessGUI() noexcept
{
	engine_destroy();
}