#include "CChess.h"

#include "Engine.h"



//engine instance
static Engine* engine{ nullptr };



CCHESS_BOOL engine_create() CCHESS_NOEXCEPT
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
		catch(std::exception& e)
		{
			return CCHESS_FALSE;
		}
	}
}

void engine_destroy() CCHESS_NOEXCEPT
{
	delete engine;
	engine = nullptr;
}

void engine_position_start() CCHESS_NOEXCEPT
{
	if (engine)
	{
		engine->setStartState();
	}
}

CCHESS_BOOL engine_position_fen(const char* fen) CCHESS_NOEXCEPT;
{
	if (engine)
	{
		try
		{
			const State newState{ fen, Castle::All }; //remove after finishing state fen
			engine->setState(newState);
			return CCHESS_TRUE;
		}
		catch (std::exception&)
		{
			return CCHESS_FALSE;
		}
	}
	else
	{
		return CCHESS_FALSE;
	}
}

const char* engine_get_position() CCHESS_NOEXCEPT
{
	throw std::runtime_error("Not implemented");
}

const char* engine_get_char_position() CCHESS_NOEXCEPT
{
	if (engine)
	{
		return engine->getCharPosition();
	}
	else
	{
		return nullptr;
	}
}

void engine_set_search_milliseconds(int milliseconds) CCHESS_NOEXCEPT
{
	if (engine)
	{
		engine->setSearchMilliseconds(milliseconds);
	}
}

int engine_get_search_milliseconds() CCHESS_NOEXCEPT
{
	if (engine)
	{
		return engine->searchMilliseconds();
	}
	else
	{
		return 0;
	}
}

void engine_search(int* source, int* destination) CCHESS_NOEXCEPT
{
	if (engine)
	{
		const Move bestMove{ engine->search() };

		*source = bestMove.sourceIndex();
		*destination = bestMove.destinationIndex();
	}
}

void engine_search_and_move() CCHESS_NOEXCEPT
{
	if (engine)
	{
		engine->engineMove();
	}
}

void engine_search_color(int white, int* source, int* destination) CCHESS_NOEXCEPT
{
	if (engine)
	{
		const Move bestMove{ engine->search(static_cast<bool>(white)) };

		*source = bestMove.sourceIndex();
		*destination = bestMove.destinationIndex();
	}
}

void engine_search_color_and_move(int white) CCHESS_NOEXCEPT
{
	if (engine)
	{
		engine->engineMove(white);
	}
}

int engine_move(int source, int destination) CCHESS_NOEXCEPT
{
	if (engine)
	{
		return static_cast<int>(engine->makeMove(source, destination));
	}
	else
	{
		return 0;
	}
}

void engine_get_last_move(int* source, int* destination) CCHESS_NOEXCEPT
{
	if (engine)
	{
		const Move lastMove{ engine->getLastMove() };
		*source = lastMove.sourceIndex();
		*destination = lastMove.destinationIndex();
	}
}

void engine_search_info(int* depth, int* evaluation) CCHESS_NOEXCEPT
{
	if (engine)
	{
		const SearchInfo& info{ engine->searchInfo() };

		*depth = info.searchDepth;
		*evaluation = info.evaluation;
	}
}

void engine_perft(int depth) CCHESS_NOEXCEPT
{
	if (engine)
	{
		engine->perft(depth);
	}
}

void engine_benchmark(double seconds) CCHESS_NOEXCEPT
{
	if (engine)
	{
		engine->benchmark(seconds);
	}
}

void engine_move_back(int* source, int* destination) CCHESS_NOEXCEPT
{
	if (engine)
	{
		const Move move{ engine->moveBackCallback() };
		*source = move.sourceIndex();
		*destination = move.destinationIndex();
	}
}

void engine_move_forward(int* source, int* destination) CCHESS_NOEXCEPT
{
	if (engine)
	{
		const Move move{ engine->moveBackCallback() };
		*source = move.sourceIndex();
		*destination = move.destinationIndex();
	}
}

void engine_search_async() CCHESS_NOEXCEPT
{
	if (engine)
	{
		engine->startAsyncSearch();
	}
}

void engine_stop_search_async() CCHESS_NOEXCEPT
{
	if (engine)
	{
		engine->stopAsyncSearch();
	}
}

int engine_get_asynce_done(int* source, int* destination) CCHESS_NOEXCEPT
{
	if (engine)
	{
		Move move{ 0 };

		if (engine->getAsyncDone(move))
		{
			*source = move.sourceIndex();
			*destination = move.destinationIndex();
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

int engine_get_async_search_stats() CCHESS_NOEXCEPT
{
	if (engine)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}