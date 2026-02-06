#include "Engine.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <span>
#include <stop_token>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

#include "BitBoard.h"
#include "Castle.hpp"
#include "Evaluate.h"
#include "KillerMoveHistory.h"
#include "Move.h"
#include "MoveGen.h"
#include "MoveList.hpp"
#include "State.h"



//	Static Helpers

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

static Move getCastleMove(int sourceSquare, int destinationSquare)
{
	constexpr Move whiteKingSide{ 0b00000100000000000000000000000000 | static_cast<std::uint32_t>(Castle::WhiteKingSide) };
	constexpr Move whiteQueenSide{ 0b00000100000000000000000000000000 | static_cast<std::uint32_t>(Castle::WhiteQueenSide) };
	constexpr Move blackKingSide{ 0b00000100000000000000000000000000 | static_cast<std::uint32_t>(Castle::BlackKingSide) };
	constexpr Move blackQueenSide{ 0b00000100000000000000000000000000 | static_cast<std::uint32_t>(Castle::BlackQueenSide) };

	if (sourceSquare == e1)
	{
		if (destinationSquare == g1)
		{
			return whiteKingSide;
		}
		else if (destinationSquare == c1)
		{
			return whiteQueenSide;
		}
	}
	else if (sourceSquare == e8)
	{
		if (destinationSquare == g8)
		{
			return blackKingSide;
		}
		else if (destinationSquare == c8)
		{
			return blackQueenSide;
		}
	}

	return 0;
}



//	Private Methods

static thread_local std::uint32_t logCounter{};
int Engine::search(const State& state, int color, int depth, int alpha, int beta) noexcept
{
	++m_nodeCount;

	if (!(logCounter & 0x00100000)) logSearchInfo();
	++logCounter;

	if (depth == m_currentSearchDepth || m_stopSearch.load(std::memory_order_relaxed))
	{
		return color * evaluate(state);
	}

	MoveList moves{ MoveGen::generateMoves(color > 0, state) };
	moves.sort(m_killerMoves.killerMoves(depth), m_principalVariation[depth]);

	int legalMoves{};
	int bestScore{ worstValue };
	Move bestMove{};

	for (Move move : moves) 
	{
		State stateCopy{ state };

		if (makeLegalMove(stateCopy, move, color > 0))
		{
			++legalMoves;
			const int score{ -search(stateCopy, -color, depth + 1, -beta, -alpha) };
			bestMove = score > bestScore ? move : bestMove;
			bestScore = std::max(bestScore, score);
			alpha = std::max(alpha, score);

			if (alpha >= beta)
			{
				m_killerMoves.push(depth, move);
				break;
			}
		}
	}

	if (legalMoves == 0)
	{
		//check for white or black checkmate
		if (color > 0 ? state.whiteKingInCheck() : state.blackKingInCheck())
		{
			return checkmateScore + depth;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		m_principalVariation[depth] = bestMove;
		return bestScore;
	}
}

void Engine::logSearchInfo() noexcept
{
	const clock::time_point now{ clock::now() };
	const std::chrono::duration<float> elapsed{ now - m_searchStart };

	m_searchInfo.nodesPerSecond = m_nodeCount / elapsed.count();
	m_searchInfo.timeRemaining = static_cast<float>(m_searchMilliseconds) - std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
	m_newInfo.store(true, std::memory_order_release);
}

std::string_view Engine::principalVariation() noexcept
{
	m_pvString.clear();

	const std::span<const Move> moves{
		m_principalVariation.begin(), std::ranges::find_if(m_principalVariation, [](Move move) {
			return !move.move();
		})
	};

	std::ranges::for_each(moves, [this](Move move) {
		const std::string moveString{ move.string() };
		m_pvString.push(moveString);
		m_pvString.push(",");
		});

	return m_pvString.view();
}



//Public Methods

// constructors
Engine::Engine() noexcept
	: m_worker(worker, std::ref(m_mutex), std::ref(m_cv), std::ref(*this)) 
{
	m_history.front().state = startState;
	m_history.front().whiteToMove = true;

	m_currentLegalMoves = MoveGen::generateMoves(true, m_history.front().state);
}

Engine::~Engine()
{
	m_cv.notify_one();
}



//search
void Engine::startSearch(bool whiteToMove) noexcept
{
	if (!m_stopSearch.load(std::memory_order_relaxed) || m_currentState + 1 == m_history.end()) return;

	m_currentState->whiteToMove = whiteToMove;
	m_nodeCount = 0;
	m_searchStart = clock::now();
	m_stopSearch.store(false, std::memory_order_relaxed);
	m_cv.notify_one();

	std::thread([](std::atomic_bool& stopSearch, int searchMilliseconds) {
		std::this_thread::sleep_for(std::chrono::milliseconds(searchMilliseconds));
		stopSearch.store(true, std::memory_order_relaxed);
		}, std::ref(m_stopSearch), m_searchMilliseconds).detach();
}

void Engine::stopSearch() noexcept
{
	m_stopSearch.store(true, std::memory_order_relaxed);
}

void Engine::searchRun() noexcept
{
	m_killerMoves = KillerMoveHistory();
	m_principalVariation.fill(0);

	m_searchInfo.principalVariation = "no pv";
	m_newInfo.store(true, std::memory_order_release);

	for (int depth{ 1 }; depth <= maxSearchDepth; ++depth)
	{

		m_currentSearchDepth = depth; //had the idea to use this variable in the for loop but apparently it is considered bad practice
		const int score{ search(m_currentState->state, m_currentState->whiteToMove ? 1 : -1, 0, worstValue, bestValue) };

		if (m_stopSearch.load(std::memory_order_relaxed)) break;

		m_bestMove = m_principalVariation.front();
		m_searchInfo.depth = depth;
		m_searchInfo.evaluation = m_currentState->whiteToMove ? score : -score;
		m_searchInfo.principalVariation = principalVariation();
		m_newInfo.store(true, std::memory_order_release);
	}
}



//getters
bool Engine::searchInfo(SearchInfo& info) noexcept
{
	if (m_newInfo.load(std::memory_order_acquire))
	{
		info = m_searchInfo;
		m_newInfo.store(false, std::memory_order_relaxed);

		return true;
	}
	else
	{
		return false;
	}
}

std::string_view Engine::fenPosition() noexcept
{
	m_fenPosition = m_currentState->state.fenPosition();
	return m_fenPosition.data();
}

std::string_view Engine::charPosition() noexcept
{
	m_charPosition = m_currentState->state.charPosition();
	return m_charPosition.data();
}

Move Engine::bestMove() const noexcept
{
	if (m_stopSearch.load(std::memory_order_relaxed))
	{
		return m_bestMove;
	}
	else
	{
		return 0;
	}
}

std::pair<int, int> Engine::lastMove() noexcept
{
	return std::pair(m_currentState->moveSource, m_currentState->moveDestination);
}


//setters
bool Engine::setPositionChar(std::string_view position) noexcept
{
	if (!m_stopSearch.load(std::memory_order_relaxed)) return false;

	m_currentState->state = State::fromChar(position);
	m_currentLegalMoves = MoveGen::generateMoves(m_currentState->whiteToMove, m_currentState->state);

	return true;
}

bool Engine::setPositionFen(std::string_view position) noexcept
{
	if (!m_stopSearch.load(std::memory_order_relaxed)) return false;

	m_currentState->state = State::fromFen(position);
	m_currentLegalMoves = MoveGen::generateMoves(m_currentState->whiteToMove, m_currentState->state);

	return true;
}

bool Engine::move(bool white, int source, int destination) noexcept
{
	if (!m_stopSearch.load(std::memory_order_relaxed) || m_currentState + 1 == m_history.end()) return false;

	const Move castleMove{ getCastleMove(source, destination) };

	const auto it{ std::ranges::find_if(m_currentLegalMoves, [source, destination, castleMove](Move move) {
		   return (move.sourceIndex() == source && move.destinationIndex() == destination) || (move.move() == castleMove.move());
	   }) };

	State stateCopy{ m_currentState->state };

	if (it == m_currentLegalMoves.end() || !makeLegalMove(stateCopy, *it, white)) return false;
	
	const Move move{ *it };

	++m_currentState;
	m_historyBack = m_currentState + 1;

	m_currentState->state = stateCopy;
	m_currentState->whiteToMove = !white;
	m_currentState->moveSource = source;
	m_currentState->moveDestination = destination;
	m_currentLegalMoves = MoveGen::generateMoves(m_currentState->whiteToMove, stateCopy);

	return true;
}

bool Engine::moveForward() noexcept
{
	if (!m_stopSearch.load(std::memory_order_relaxed) || m_currentState + 1 == m_historyBack) return false;

	++m_currentState;
	m_currentLegalMoves = MoveGen::generateMoves(m_currentState->whiteToMove, m_currentState->state);

	return true;
}

bool Engine::moveBack() noexcept
{
	if (!m_stopSearch.load(std::memory_order_relaxed) || m_currentState == m_history.begin()) return false;

	--m_currentState;
	m_currentLegalMoves = MoveGen::generateMoves(m_currentState->whiteToMove, m_currentState->state);

	return true;
}

void Engine::reset() noexcept
{
	if (!m_stopSearch.load(std::memory_order_relaxed)) return;

	m_currentState = m_history.begin();
	m_historyBack = m_history.begin() + 1;

	m_history.fill(HistoryPosition());
	m_history.front().state = startState;
	m_history.front().whiteToMove = true;

	m_currentLegalMoves = MoveGen::generateMoves(m_currentState->whiteToMove, m_currentState->state);
}
