#include "CChess.h"

#include <exception>
#include <algorithm>

#include "Engine.h"
#include "Move.h"



//engine instance
static Engine* engine{ nullptr };



//	ENGINE
CCHESS_BOOL engine_create() noexcept
{
	if (engine) return CCHESS_FALSE;

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
	if (!(engine && position)) return false;

	const std::string_view constView{ position };
	engine->setPositionFen(constView);

	return true;
}

CCHESS_BOOL engine_set_position_char(const char* position) noexcept
{
	if (!(engine && position)) return false;
	
	const std::string_view constView{ position };
	engine->setPositionChar(constView);

	return true;
}

const char* engine_get_position_fen() noexcept
{
	if (!engine) return nullptr;
	
	const std::string_view data{ engine->fenPosition() };

	return data.data();
}

const char* engine_get_position_char() noexcept
{
	if (!engine) return nullptr; 

	const std::string_view data{ engine->charPosition() };

	return data.data();
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

CCHESS_BOOL engine_search_info(CCHESS_BOOL* done, int* evaluation, int* depth, float* nodes_per_second, float* timeRemaining, const char** principal_variation)  noexcept
{
	if (!engine) return false;

	Engine::SearchInfo info{};
	
	if (engine->searchInfo(info))
	{
		*evaluation = info.evaluation;
		*depth = info.depth;
		*nodes_per_second = info.nodesPerSecond;
		*timeRemaining = info.timeRemaining;
	}
	else
	{
		return false;
	}
}

//	Get the best move after the search is done. If the search is not done or stopSearch() has not been called 'source' and 
//	'destination' are not modified and the function returns CCHESS_FALSE
CCHESS_BOOL engine_best_move(int* source, int* destination) CCHESS_NOEXCEPT
{
	if (!engine) return false;

	const Move move{ engine->bestMove() };

	*source = move.sourceIndex();
	*destination = move.destinationIndex();

	return move.move();
}



//	SEARCH

CCHESS_BOOL engine_move(int source, int destination) CCHESS_NOEXCEPT
{
	if (!engine) return false;

	return engine->move(source, destination);
}

void engine_move_unchecked(int source, int destination) CCHESS_NOEXCEPT
{
	if (engine) engine->moveUnchecked(source, destination);
}