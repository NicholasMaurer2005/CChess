#include "CChessGUI.h"

#include <CChess.h>
#include <memory>
#include <thread>
#include <atomic>
#include <iostream>
#include <format>



/* Private Methods */

void CChessGUI::moveCallback(int source, int destination) noexcept
{
	if (engine_move(source, destination))
	{
		m_whiteToMove = false;
		m_window.bufferBoard(false, source, destination);
	}

	bufferPosition();
}

void CChessGUI::moveBack() noexcept
{
	int source{};
	int destination{};

	engine_move_back(&source, &destination);
	m_window.bufferBoard(false, source, destination);
	m_newPosition = true;
}

void CChessGUI::moveForward() noexcept
{
	int source{};
	int destination{};

	engine_move_forward(&source, &destination);
	m_window.bufferBoard(false, source, destination);
	m_newPosition = true;
}

void CChessGUI::bufferPosition() noexcept
{
	const char* position{ engine_get_char_position() };
	m_window.bufferPieces(false, std::span(position, 64));
	m_window.draw();
}

void CChessGUI::play() noexcept
{
	engine_set_search_milliseconds(500);
	bufferPosition();

	while (m_window.open())
	{
		if (m_newPosition)
		{
			bufferPosition();
			m_window.draw();
			m_newPosition = false;
		}

		if (!m_whiteToMove)
		{
			engine_search_and_move();

			int depth{};
			int evaluation{};
			engine_search_info(&depth, &evaluation);
			std::cout << std::format("depth: {}, evaluation: {}\n", depth, evaluation);

			int source{};
			int destination{};
			engine_get_last_move(&source, &destination);
			m_window.bufferBoard(false, source, destination);

			bufferPosition();
			m_whiteToMove = true;
		}

		m_window.draw();
	}
}



/* Public Methods */

//constructor
CChessGUI::CChessGUI() noexcept
	: m_window(
		[this](int width, int height) { this->moveCallback(width, height); },
		[this]() { this->moveBack(); },
		[this]() { this->moveForward(); }
	), 
	m_newPosition(true), m_whiteToMove(true)
{
	engine_create();
	play();
}

CChessGUI::~CChessGUI() noexcept
{
	engine_destroy();
}