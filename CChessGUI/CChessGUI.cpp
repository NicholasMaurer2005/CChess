#include "CChessGUI.h"

#include <CChess.h>
#include <memory>


//constants
constexpr int defaultWindowWidth{ 1000 };
constexpr int defaultWindowHeight{ 1000 };

//FUCK IT
static bool newPosition{ true };

//static helpers
static void moveCallback(int source, int destination) noexcept
{
	engine_move(source, destination);
	newPosition = true;
}



/* Private Methods */

void CChessGUI::play() noexcept
{
	while (m_window.open())
	{
		if (newPosition)
		{
			std::unique_ptr<const char> position{ engine_get_char_position() };
			m_window.bufferPieces(false, std::span(position.get(), 64));
			newPosition = false;
		}

		m_window.draw();
	}
}



/* Public Methods */

//constructor
CChessGUI::CChessGUI() noexcept
	: m_window(defaultWindowWidth, defaultWindowHeight, moveCallback), m_newPosition(true)
{
	engine_create();

	play();
}

CChessGUI::~CChessGUI() noexcept
{
	engine_destroy();
}