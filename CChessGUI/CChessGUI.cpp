#include "CChessGUI.h"

#include <CChess.h>
#include <memory>
#include <thread>
#include <atomic>
#include <iostream>
#include <format>



static constexpr int noPiece{ 64 };
static constexpr bool flipped{ true };


/* Private Methods */

void CChessGUI::moveCallback(int source, int destination) noexcept
{
	if (engine_move(source, destination))
	{
		m_whiteToMove = false;
		m_sourceIndex = source;
		m_destinationIndex = destination;
	}

	m_newPosition = true;
}

void CChessGUI::moveBackCallback() noexcept
{
	engine_move_back(&m_sourceIndex, &m_destinationIndex);
	m_newPosition = true;
}

void CChessGUI::moveForwardCallback() noexcept
{
	engine_move_forward(&m_sourceIndex, &m_destinationIndex);
	m_newPosition = true;
}

void CChessGUI::bufferPosition() noexcept
{
	const char* position{ engine_get_char_position() };
	m_window.bufferPieces(flipped, std::span(position, 64));
}

void CChessGUI::play() noexcept
{
	while (m_window.open())
	{
		if (m_newPosition)
		{
			bufferPosition();
			m_window.bufferBoard(flipped, m_sourceIndex, m_destinationIndex);
		}

		m_window.draw();

		if (!m_whiteToMove)
		{
			engine_search_and_move();
			m_newPosition = true;
			m_whiteToMove = true;
		}
	}
}



/* Public Methods */

//constructor
CChessGUI::CChessGUI() noexcept
	: m_window(
		[this](int width, int height) { this->moveCallback(width, height); },
		[this]() { this->moveBackCallback(); },
		[this]() { this->moveForwardCallback(); }
	), 
	m_newPosition(true), m_whiteToMove(true), m_sourceIndex(noPiece), m_destinationIndex(noPiece)
{
	engine_create();
	engine_set_search_milliseconds(500);

	play();
}

CChessGUI::~CChessGUI() noexcept
{
	engine_destroy();
}