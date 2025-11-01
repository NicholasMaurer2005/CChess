#include "CChessGUI.h"

#include <CChess.h>
#include <memory>


//constants
constexpr int defaultWindowWidth{ 1000 };
constexpr int defaultWindowHeight{ 1000 };



//static helpers
static MoveCallback moveCallback(int source, int destination) noexcept
{

}



/* Private Methods */

void CChessGUI::play() noexcept
{
	while (m_window.open())
	{
		if (m_newPosition)
		{
			std::unique_ptr<const char> position{ engine_get_char_position() };
			m_window.bufferPieces(false, position.get());
			m_newPosition = false;
		}

		m_window.draw();
	}
}



/* Public Methods */

//constructor
CChessGUI::CChessGUI() noexcept
	: m_window(defaultWindowWidth, defaultWindowHeight), m_newPosition(true)
{
	engine_create();

	play();
}

CChessGUI::~CChessGUI() noexcept
{
	engine_destroy();
}