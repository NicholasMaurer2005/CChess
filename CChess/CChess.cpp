#include "CChess.h"

#include <cstring>

#include "Engine.h"
#include "Move.h"
#include "State.h"
#include "ChessConstants.hpp" //remove after finishing state fen


//engine instance
static Engine* engine{ nullptr };



int engine_create() CCHESS_NOEXCEPT
{
	if (engine)
	{
		return 0;
	}
	else
	{
		try
		{
			engine = new Engine();
			return 1;
		}
		catch(std::exception& e)
		{
			return 0;
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

int engine_position_fen(const char* fen) CCHESS_NOEXCEPT
{
	if (engine)
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
	else
	{
		return 0;
	}
}

CCHESS_NODISCARD const char* engine_get_position() CCHESS_NOEXCEPT
{
	if (engine)
	{
		const std::string fen{ engine->stateFen() };

		const std::size_t length{ fen.length() + 1 };
		char* data{ new char[length] };
		std::memcpy(data, fen.c_str(), length);

		return data;
	}
	else
	{
		return nullptr;
	}
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
		const Move move{ engine->moveBack() };
		*source = move.sourceIndex();
		*destination = move.destinationIndex();
	}
}

void engine_move_forward(int* source, int* destination) CCHESS_NOEXCEPT
{
	if (engine)
	{
		const Move move{ engine->moveBack() };
		*source = move.sourceIndex();
		*destination = move.destinationIndex();
	}
}