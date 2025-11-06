#include "Engine.h"

#include <iostream>
#include <limits>
#include <cstdlib>
#include <vector>
#include <ranges>
#include <thread>
#include <chrono>
#include <format>
#include <string>
#include <algorithm>
#include <cassert>
#include <string_view>

#include "MoveList.hpp"
#include "Move.h"
#include "Evaluate.h"
#include "Castle.hpp"



//constants
static constexpr int bestValue{ 9999999 };
static constexpr int worstValue{ -9999999 };
static constexpr int checkmateScore{ -999999 };
static constexpr int defaultSearchMilliseconds{ 3000 };
static constexpr std::string_view startFen{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" };

static State startState{ startFen, Castle::All };


//static helpers
static int squareToIndex(std::string_view square)
{
	const int rank{ static_cast<int>(square[1] - '1') };
	const int file{ static_cast<int>(square[0] - 'a') };

	return rank * fileSize + file;
}

static Move playerMove(MoveList& moves)
{
	std::string in;
	std::getline(std::cin, in);

	if (in.length() == 4)
	{
		const int source{ squareToIndex(in.substr(0, 2)) };
		const int destination{ squareToIndex(in.substr(2, 2)) };

        const auto it{ std::find_if(moves.begin(), moves.end(), [source, destination](Move move) {
            return move.sourceIndex() == source && move.destinationIndex() == destination; 
        }) };

		if (it != moves.end())
		{
			return *it;
		}
		else
		{
			return 0;
		}
	}
	else if (in.length() == 2)
	{
		constexpr Move castleMove{ 0b00000100000000000000000000000000 };

		if (in == "wk")
		{
			return castleMove.move() | static_cast<std::uint32_t>(Castle::WhiteKingSide);
		}
		else if (in == "wq")
		{
			return castleMove.move() | static_cast<std::uint32_t>(Castle::WhiteQueenSide);
		}
		else if (in == "bk")
		{
			return castleMove.move() | static_cast<std::uint32_t>(Castle::BlackKingSide);
		}
		else if (in == "bq")
		{
			return castleMove.move() | static_cast<std::uint32_t>(Castle::BlackQueenSide);
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



//constructor
Engine::Engine() noexcept :
	//engine
	 m_moveGen(), m_state(startState), m_whiteToMove(true), m_info(), m_legalMoves(), m_lastMove(), m_charPosition(),
	//search
	m_gameOver(), m_stopSearch(), m_killerMoves(), m_searchMilliseconds(defaultSearchMilliseconds)
{
	findWhiteSquares(m_state);
	findBlackSquares(m_state);

	m_legalMoves = m_moveGen.generateMoves(m_whiteToMove, m_state);
}



//private methods
std::uint64_t Engine::perftRun(int depth, bool white) noexcept
{
	if (depth == 0)
	{
		return 1ULL;
	}

	const MoveList moves{ m_moveGen.generateMoves(white, m_state) };

	std::uint64_t perftCount{};

	for (Move move : moves)
	{
		const State stateCopy{ m_state };

		if (makeLegalMove(m_state, white, move))
		{
			perftCount += perftRun(depth - 1, !white);
		}

		//unmake move
		m_state = stateCopy;
	}

	return perftCount;
}

void Engine::findWhiteSquares(State& state) const noexcept
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
		squares |= m_moveGen.getWhitePawnMoves(index).board();
	}

	while (knights.board())
	{
		const int index{ knights.popLeastSignificantBit() };
		squares |= m_moveGen.getKnightMoves(index).board();
	}

	while (bishops.board())
	{
		const int index{ bishops.popLeastSignificantBit() };
		squares |= m_moveGen.getBishopMoves(index, state.occupancy()).board();
	}

	while (rooks.board())
	{
		const int index{ rooks.popLeastSignificantBit() };
		squares |= m_moveGen.getRookMoves(index, state.occupancy()).board();
	}

	while (queens.board())
	{
		const int index{ queens.popLeastSignificantBit() };
		squares |= m_moveGen.getBishopMoves(index, state.occupancy()).board() | m_moveGen.getRookMoves(index, state.occupancy()).board();
	}

	const int kingIndex{ kings.popLeastSignificantBit() };
	squares |= m_moveGen.getKingMoves(kingIndex).board();

	state.setWhiteSquares(squares & ~state.whiteOccupancy().board());
}

void Engine::findBlackSquares(State& state) const noexcept
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
		squares |= m_moveGen.getBlackPawnMoves(index).board();
	}

	while (knights.board())
	{
		const int index{ knights.popLeastSignificantBit() };
		squares |= m_moveGen.getKnightMoves(index).board();
	}

	while (bishops.board())
	{
		const int index{ bishops.popLeastSignificantBit() };
		squares |= m_moveGen.getBishopMoves(index, state.occupancy()).board();
	}

	while (rooks.board())
	{
		const int index{ rooks.popLeastSignificantBit() };
		squares |= m_moveGen.getRookMoves(index, state.occupancy()).board();
	}

	while (queens.board())
	{
		const int index{ queens.popLeastSignificantBit() };
		squares |= m_moveGen.getBishopMoves(index, state.occupancy()).board() | m_moveGen.getRookMoves(index, state.occupancy()).board();
	}

	const int kingIndex{ kings.popLeastSignificantBit() };
	squares |= m_moveGen.getKingMoves(kingIndex).board();

	state.setBlackSquares(squares & ~state.blackOccupancy().board());
}

bool Engine::makeLegalMove(State& state, bool white, Move move) const noexcept
{
	state.makeMove(white, move);

	findWhiteSquares(state);
	findBlackSquares(state);

	return white
		? (state.pieceOccupancyT<Piece::BlackKing>().board() && !state.whiteKingInCheck())
		: (state.pieceOccupancyT<Piece::WhiteKing>().board() && !state.blackKingInCheck());
}

int Engine::quiescenceSearch(const State& state, int alpha, int beta, bool white) noexcept
{
	//track how many recursive calls have happened, 
	thread_local int stopSearchCheck{};
	++stopSearchCheck;

	const int standardPat{ white ? evaluate(state) : -evaluate(state) };

	if (standardPat >= beta)
	{
		return standardPat;
	}

	const CaptureList moves{ m_moveGen.generateCaptures(white, state) };
	int legalMoves{};
	int bestScore{ worstValue };

	for (Move move : moves)
	{
		State stateCopy{ state };

		if (makeLegalMove(stateCopy, white, move))
		{
			//check every 16384 calls if time is up, if it is stop the search
			if ((stopSearchCheck & 0x3FFF) == 0 && m_stopSearch.load(std::memory_order_relaxed))
			{
				return 0;
			}

			++legalMoves;
			
			const int score{ -quiescenceSearch(stateCopy, -beta, -alpha, !white) };
			bestScore = std::max(bestScore, score);
			alpha = std::max(alpha, score);

			if (alpha >= beta)
			{
				break;
			}
		}
	}

	if (legalMoves)
	{
		return bestScore;
	}
	else
	{
		return standardPat;
	}
}

int Engine::searchRun(const State& state, int depth, int alpha, int beta, bool white) noexcept
{
	//track how many recursive calls have happened, 
	thread_local int stopSearchCheck{};
	++stopSearchCheck;

	//leaf of search, evaluate position
	if (depth == 0)
	{
		//return quiescenceSearch(state, alpha, beta, white);
		return white ? evaluate(state) : -evaluate(state);
	}

	int bestScore{ worstValue };
	int legalMoves{};

	MoveList moves{ m_moveGen.generateMoves(white, state) };
	moves.sort(m_killerMoves.killerMoves(depth));
	
	for (Move move : moves)
	{
		//check every 16384 calls if time is up, if it is stop the search
		if ((stopSearchCheck & 0x3FFF) == 0 && m_stopSearch.load(std::memory_order_relaxed))
		{
			return 0;
		}

		State stateCopy{ state };

		if (makeLegalMove(stateCopy, white, move))
		{
			++legalMoves;

			const int score{ -searchRun(stateCopy, depth - 1, -beta, -alpha, !white) };
			bestScore = std::max(bestScore, score);
			alpha = std::max(alpha, score);

			if (alpha >= beta)
			{
				m_killerMoves.push(depth, move);
				break;
			}
		}
	}

	//check if there is a checkmate or stalemate
	if (legalMoves == 0)
	{
		//check for white or black checkmate
		if (white ? state.whiteKingInCheck() : state.blackKingInCheck())
		{
			return checkmateScore - depth;
		}
		else
		{
			return 0;
		}
	}

	return bestScore;
}

Engine::ScoredMove Engine::searchStart(bool white, int depth) noexcept
{
	Move bestMove{ 0 };
	int bestScore{ worstValue };
	int alpha = worstValue;
	constexpr int beta = bestValue;


	MoveList moves{ m_moveGen.generateMoves(white, m_state) };
	moves.sort(m_killerMoves.killerMoves(depth));

	for (Move move : moves)
	{
		State stateCopy{ m_state };

		if (makeLegalMove(stateCopy, white, move))
		{
			const int score{ -searchRun(stateCopy, depth - 1, -beta, -alpha, !white) };

			if (score > bestScore)
			{
				bestScore = score;
				bestMove = move;
				m_killerMoves.push(depth, move);
			}

			alpha = std::max(alpha, score);

			if (alpha >= beta) [[unlikely]]
			{
				break;
			}
		}
	}

	return { bestMove, bestScore };
}

void Engine::benchmarkRun(const State& state, std::uint64_t& nodes, std::atomic_bool& stopping, bool white, int depth) noexcept
{
	if (depth == 0)
	{
		return;
	}

	const MoveList moves{ m_moveGen.generateMoves(white, m_state) };

	for (Move move : moves)
	{
		[[unlikely]]
		if (stopping.load(std::memory_order_relaxed))
		{
			return;
		}

		State stateCopy{ state };

		if (makeLegalMove(stateCopy, white, move))
		{
			++nodes;
			benchmarkRun(stateCopy, nodes, stopping, !white, depth - 1);
		}
	}
}



//getters
std::string Engine::stateFen() const noexcept
{
	std::string fen;



	return fen;
}

Move Engine::search(bool white) noexcept
{
	m_killerMoves = KillerMoveHistory();

	ScoredMove bestMove{ 0, 0 };

	m_stopSearch = false;
	std::jthread timer{ [&]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(m_searchMilliseconds));
		m_stopSearch.store(true, std::memory_order_relaxed);
		}
	};

	for (int i{ 1 }; i < maxSearchDepth; ++i)
	{
		m_killerMoves.setPlyDepth(i);

		const ScoredMove scoredMove{ searchStart(white, i) };
		bool noLegalMoves{ scoredMove.move.move() == 0 };

		if (m_stopSearch || noLegalMoves)
		{
			if (noLegalMoves)
			{
				m_gameOver = true;
			}

			return bestMove.move;
		}
		else
		{
			bestMove = scoredMove;

			m_info.searchDepth = i;
			m_info.evaluation = white ? scoredMove.score : -scoredMove.score;
		}
	}

	//this should never excecute
	return bestMove.move;
}

Move Engine::search() noexcept
{
	return search(m_whiteToMove);
}

const char* Engine::getCharPosition() noexcept
{
	constexpr std::array<char, pieceCount> pieceToChar{ ' ', 'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k' };

	m_charPosition.fill(' ');

	for (std::uint32_t i{ whitePieceOffset }; i < pieceCount; ++i)
	{
		BitBoard occupancy{ m_state.pieceOccupancy(static_cast<Piece>(i)) };

		while (occupancy.board())
		{
			const int index{ occupancy.popLeastSignificantBit() };
			m_charPosition[index] = pieceToChar[i];
		}
	}

	return m_charPosition.data();
}

int Engine::searchMilliseconds() const noexcept
{
	return m_searchMilliseconds;
}

bool Engine::gameOver() const noexcept
{
	if (m_gameOver)
	{
		return true;
	}
	else
	{
		for (Move move : m_legalMoves)
		{
			State stateCopy{ m_state };

			if (makeLegalMove(stateCopy, m_whiteToMove, move))
			{
				return true;
			}
		}

		return false;
	}
}

Move Engine::getLastMove() const noexcept
{
	return m_lastMove;
}

const SearchInfo& Engine::searchInfo() const noexcept
{
	return m_info;
}



//setters
void Engine::setState(const State& state) noexcept
{
	m_state = state;
}

void Engine::setStartState() noexcept
{
	m_state = startState;
}

bool Engine::makeMove(int source, int destination) noexcept
{
	const Move castleMove{ getCastleMove(source, destination) };

	std::cout << m_legalMoves.size();

	const auto it{ std::find_if(m_legalMoves.begin(), m_legalMoves.end(), [source, destination, castleMove](Move move) {
		   return (move.sourceIndex() == source && move.destinationIndex() == destination) || (move.move() == castleMove.move());
	   }) };

	State stateCopy{ m_state };

	if (it != m_legalMoves.end() && makeLegalMove(stateCopy, m_whiteToMove, *it))
	{
		const Move move{ *it };

		m_state = stateCopy;
		m_whiteToMove = !m_whiteToMove;
		m_legalMoves = m_moveGen.generateMoves(m_whiteToMove, m_state);
		m_lastMove = move;
		
		return true;
	}
	else 
	{
		return false;
	}
}

void Engine::setSearchMilliseconds(int milliseconds) noexcept
{
	m_searchMilliseconds = milliseconds;
}

void Engine::engineMove(bool white) noexcept
{
	const Move bestMove{ search(white) };

	if (bestMove.move() != 0)
	{
		m_state.makeMove(white, bestMove);
		m_whiteToMove = !m_whiteToMove;
		m_legalMoves = m_moveGen.generateMoves(m_whiteToMove, m_state);
		m_lastMove = bestMove;
	}
}

void Engine::engineMove() noexcept
{
	engineMove(m_whiteToMove);
}



//perft
void Engine::perft(int depth) noexcept
{
	for (int i{ 1 }; i <= depth; i++)
	{
		std::cout << std::format("perft depth: {} - {}\n", i, perftRun(i, true));
	}

	std::cout << "done" << std::endl;
}

void Engine::benchmark(double seconds) noexcept
{
	std::uint64_t nodes{};
	std::atomic_bool stopping{ false };

	std::jthread timer{ 
		[seconds, &stopping]() {
			std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
			stopping.store(true, std::memory_order_relaxed);
		} 
	};

	benchmarkRun(m_state, nodes, stopping, true, 20);

	std::cout << std::format("estimated nodes per second: {}\n", static_cast<double>(nodes) / seconds);
}