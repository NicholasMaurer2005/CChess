#include "CChess.h"

#include <cstring>

#include "Engine.h"
#include "Move.h"
#include "State.h"
#include "ChessConstants.hpp" //remove after finishing state fen


//engine instance
Engine* engine{ nullptr };

int engine_create() CCHESS_NOEXCEPT
{
	if (engine)
	{
		return 0;
	}
	else
	{
		engine = new Engine();
	}
}

void engine_destroy() CCHESS_NOEXCEPT
{
	delete engine;
}

void engine_position_start() CCHESS_NOEXCEPT
{
	engine->setStartState();
}

int engine_position_fen(const char* fen) CCHESS_NOEXCEPT
{
	try
	{
		const State newState{ fen, Castle::All }; //remove after finishing state fen
		engine->setState(newState);
		return true;
	}
	catch (std::exception&)
	{
		return false;
	}
}

const char* engine_get_position() CCHESS_NOEXCEPT
{
	const std::string fen{ engine->stateFen() };

	char* data{ new char[fen.length() + 1] };
	std::strcpy(data, fen.c_str());

	return data;
}

void engine_search(int* source, int* destination) CCHESS_NOEXCEPT
{
	const Move bestMove{ engine->search() };
	
	*source = bestMove.sourceIndex();
	*destination = bestMove.destinationIndex();
}

void engine_search_color(int white, int* source, int* destination) CCHESS_NOEXCEPT
{
	const Move bestMove{ engine->search(static_cast<bool>(white)) };

	*source = bestMove.sourceIndex();
	*destination = bestMove.destinationIndex();
}

int engine_move(int source, int destination) CCHESS_NOEXCEPT
{
	return engine->makeMove(source, destination);
}