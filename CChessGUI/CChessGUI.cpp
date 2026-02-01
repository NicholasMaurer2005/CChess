#include "CChessGUI.h"

#include <CChess.h>
#include <memory>
#include <thread>
#include <atomic>
#include <iostream>
#include <format>



static constexpr int noPiece{ 64 };
static constexpr bool flipped{ false };


/* Private Methods */

void CChessGUI::moveCallback(int source, int destination) noexcept
{
	
}

void CChessGUI::moveBackCallback() noexcept
{
	
}

void CChessGUI::moveForwardCallback() noexcept
{
	
}

void CChessGUI::bufferPosition() noexcept
{
	
}

void CChessGUI::play() noexcept
{
	m_window.bufferBoard(flipped, noPiece, noPiece);

	while (m_window.open())
	{
		m_window.draw();
	}
}



/* Public Methods */

//constructor
CChessGUI::CChessGUI() noexcept
{
	engine_create();

	play();
}

CChessGUI::~CChessGUI() noexcept
{
	engine_destroy();
}