#include "CChessGUI.h"

#include <CChess.h>


//constants
constexpr int defaultWindowWidth{ 1280 };
constexpr int defaultWindowHeight{ 720 };



/* Private Methods */



/* Public Methods */

//constructor
CChessGUI::CChessGUI() noexcept
	: m_window(defaultWindowWidth, defaultWindowHeight)
{
	engine_create();
}

CChessGUI::~CChessGUI() noexcept
{
	engine_destroy();
}