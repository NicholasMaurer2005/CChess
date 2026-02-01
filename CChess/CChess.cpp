#include "CChess.h"

#include <exception>
#include <algorithm>

#include "Engine.h"



//engine instance
static Engine* engine{ nullptr };



//	ENGINE
CCHESS_BOOL engine_create() noexcept
{
	if (engine)
	{
		return CCHESS_FALSE;
	}
	else
	{
		try
		{
			engine = new Engine();
			return CCHESS_TRUE;
		}
		catch (const std::exception&)
		{
			return CCHESS_FALSE;
		}
	}
}

void engine_destroy() noexcept
{
	delete engine;
	engine = nullptr;
}



//	POSITION
void engine_set_position_start() noexcept
{
	if (engine) engine->setStartState();
}

CCHESS_BOOL engine_set_position_fen(const char* position) noexcept
{
	if (engine && position)
	{
		const std::string_view view{ position };
		engine->setPositionFen(view);

		return true;
	}
	else
	{
		return false;
	}
}

CCHESS_BOOL engine_set_position_char(const char* position) noexcept
{
	if (engine && position)
	{
		const std::string_view view{ position };
		engine->setPositionChar(view);

		return true;
	}
	else
	{
		return false;
	}
}

const char* engine_get_position_fen() noexcept
{
	if (engine)
	{
		const std::string_view data{ engine->fenPosition() };

		return data.data();
	}
	else
	{
		return nullptr;
	}
}

const char* engine_get_position_char() noexcept
{
	if (engine)
	{
		const std::string_view data{ engine->charPosition() };

		return data.data();
	}
	else
	{
		return nullptr;
	}
}



//	SEARCH
void engine_start_search() noexcept
{
	if (engine) engine->startSearch();
}

void engine_stop_search()  noexcept
{
	if (engine) engine->stopSearch();
}

CCHESS_BOOL engine_search_info(CCHESS_BOOL& done, int& depth, float& nodes_per_second, float& timeRemaining, const char** principal_variation)  noexcept
{
	if (engine)
	{
		Engine::SearchInfo info{};
		
		return engine->searchInfo(info);
	}
	else
	{
		return false;
	}
}