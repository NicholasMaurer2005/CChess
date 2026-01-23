#include "Engine.h"

#include "MoveGen.h"
#include "Evaluate.h"

#include <array>



//static helpers
static State startState{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", Castle::All };

static void findWhiteSquares(State& state) noexcept
{
	std::uint64_t squares{};

	BitBoard pawns{ state.pieceOccupancyT<Piece::WhitePawn>() };
	BitBoard knights{ state.pieceOccupancyT<Piece::WhiteKnight>() };
	BitBoard bishops{ state.pieceOccupancyT<Piece::WhiteBishop>() };
	BitBoard rooks{ state.pieceOccupancyT<Piece::WhiteRook>() };
	BitBoard queens{ state.pieceOccupancyT<Piece::WhiteQueen>() };
	BitBoard kings{ state.pieceOccupancyT<Piece::WhiteKing>() };

	while (pawns.board())
	{
		const int index{ pawns.popLeastSignificantBit() };
		squares |= MoveGen::whitePawnMoves(index).board();
	}

	while (knights.board())
	{
		const int index{ knights.popLeastSignificantBit() };
		squares |= MoveGen::knightMoves(index).board();
	}

	while (bishops.board())
	{
		const int index{ bishops.popLeastSignificantBit() };
		squares |= MoveGen::bishopMoves(index, state.occupancy()).board();
	}

	while (rooks.board())
	{
		const int index{ rooks.popLeastSignificantBit() };
		squares |= MoveGen::rookMoves(index, state.occupancy()).board();
	}

	while (queens.board())
	{
		const int index{ queens.popLeastSignificantBit() };
		squares |= MoveGen::bishopMoves(index, state.occupancy()).board() | MoveGen::rookMoves(index, state.occupancy()).board();
	}

	const int kingIndex{ kings.popLeastSignificantBit() };
	squares |= MoveGen::kingMoves(kingIndex).board();

	state.setWhiteSquares(squares & ~state.whiteOccupancy().board());
}

static void findBlackSquares(State& state) noexcept
{
	std::uint64_t squares{};

	BitBoard pawns{ state.pieceOccupancyT<Piece::BlackPawn>() };
	BitBoard knights{ state.pieceOccupancyT<Piece::BlackKnight>() };
	BitBoard bishops{ state.pieceOccupancyT<Piece::BlackBishop>() };
	BitBoard rooks{ state.pieceOccupancyT<Piece::BlackRook>() };
	BitBoard queens{ state.pieceOccupancyT<Piece::BlackQueen>() };
	BitBoard kings{ state.pieceOccupancyT<Piece::BlackKing>() };

	while (pawns.board())
	{
		const int index{ pawns.popLeastSignificantBit() };
		squares |= MoveGen::blackPawnMoves(index).board();
	}

	while (knights.board())
	{
		const int index{ knights.popLeastSignificantBit() };
		squares |= MoveGen::knightMoves(index).board();
	}

	while (bishops.board())
	{
		const int index{ bishops.popLeastSignificantBit() };
		squares |= MoveGen::bishopMoves(index, state.occupancy()).board();
	}

	while (rooks.board())
	{
		const int index{ rooks.popLeastSignificantBit() };
		squares |= MoveGen::rookMoves(index, state.occupancy()).board();
	}

	while (queens.board())
	{
		const int index{ queens.popLeastSignificantBit() };
		squares |= MoveGen::bishopMoves(index, state.occupancy()).board() | MoveGen::rookMoves(index, state.occupancy()).board();
	}

	const int kingIndex{ kings.popLeastSignificantBit() };
	squares |= MoveGen::kingMoves(kingIndex).board();

	state.setBlackSquares(squares & ~state.blackOccupancy().board());
}

static bool makeLegalMove(State& state, Move move, bool whiteToMove)
{
	state.makeMove(whiteToMove, move);

	findWhiteSquares(state);
	findBlackSquares(state);

	return whiteToMove
		? (state.pieceOccupancyT<Piece::BlackKing>().board() && !state.whiteKingInCheck())
		: (state.pieceOccupancyT<Piece::WhiteKing>().board() && !state.blackKingInCheck());
}

static void worker(std::stop_token token, std::mutex& mutex, std::condition_variable& cv, Engine& engine) noexcept
{
	while (!token.stop_requested())
	{
		{
			std::unique_lock lock(mutex);

			cv.wait(lock);
		}

		engine.searchRun();
	}
}

static char pieceToChar(Piece piece)
{
	static constexpr std::array<char, pieceCount> table{ '.', 'P','N','B','R','Q','K','p','n','b','r','q','k' };

	return table[static_cast<std::size_t>(piece)];
}



/// constructors
Engine::Engine() noexcept
	: m_currentState(startState), m_worker(worker, std::ref(m_mutex), std::ref(m_cv), std::ref(*this)) 
{
	m_charPosition.back() = '\0';
	generateCharPosition();
}

Engine::~Engine()
{
	m_cv.notify_one();
}


//private methods
void Engine::generateCharPosition() noexcept
{
	for (int i{}; i < boardSize; ++i)
	{
		const Piece whitePiece{ m_currentState.findPiece<true>(i) };
		const Piece blackPiece{ m_currentState.findPiece<true>(i) };
		const Piece piece{ whitePiece != Piece::NoPiece ? whitePiece : blackPiece };

		m_charPosition[i] = pieceToChar(piece);
	}
}



//search
void Engine::logNodesPerSecond() noexcept
{
	const clock::time_point now{ clock::now() };
	const duration elapsed{ now - m_searchStart };
	m_info.nodesPerSecond = m_nodeCount / elapsed.count();
}

static thread_local std::uint32_t logCounter{};
int Engine::search(const State& state, bool whiteToMove, int depth, int alpha, int beta) noexcept
{
	if (logCounter & 0x00010000) logNodesPerSecond();
	++logCounter;

	if (depth == 0)
	{
		++m_nodeCount;
		return evaluate(state);
	}

	if (m_stopSearch) return;

	const MoveList moves{ MoveGen::generateMoves(whiteToMove, state) };
	int legalMoves{};
	int bestScore{ worstValue };

	std::ranges::for_each(moves, [this, &state, whiteToMove, &legalMoves, depth, beta, &alpha, &bestScore](Move move) {
		State stateCopy{ state };

		if (makeLegalMove(stateCopy, move, whiteToMove))
		{
			++legalMoves;
			const int score{ -search(stateCopy, !whiteToMove, depth - 1, -beta, -alpha) };
			bestScore = std::max(bestScore, score);
			alpha = std::max(alpha, score);

			if (alpha >= beta)
			{
				//m_killerMoves.push(depth, move);
				return;
			}
		}
		});

	if (legalMoves == 0)
	{
		//check for white or black checkmate
		if (whiteToMove ? state.whiteKingInCheck() : state.blackKingInCheck())
		{
			return checkmateScore - depth;
		}
		else
		{
			return 0;
		}
	}
}

void Engine::searchRun() noexcept
{
	static constexpr int maxDepth{ 50 };

	for (int depth{ 1 }; depth <= maxDepth; ++depth)
	{
		const int score{ search(m_currentState, m_currentWhiteToMove, depth) };
		m_info.depth = depth;
		m_info.evaluation = score;
	}
}

void Engine::startSearch() noexcept
{
	m_stopSearch = false;
	m_cv.notify_one();
}

void Engine::startSearch(const State& state, bool whiteToMove) noexcept
{
	m_currentState = state;
	m_currentWhiteToMove = whiteToMove;
	startSearch();
}

void Engine::stopSearch() noexcept
{
	m_stopSearch = true;
}



//getters
Engine::SearchInfo Engine::info() const noexcept
{
	return m_info;
}

const char* Engine::charPosition() noexcept
{
	generateCharPosition();

	return m_charPosition.data();
}


//setters
void Engine::setState(const State& state) noexcept
{
	m_currentState = state;
}

void Engine::setStartState() noexcept
{
	m_currentState = startState;
}