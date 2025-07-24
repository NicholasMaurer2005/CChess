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

#include "MoveList.h"
#include "Move.h"
#include "Evaluate.h"



static constexpr int bestValue{ 999999 };
static constexpr int worstValue{ -999999 };
static constexpr int maxDepth{ std::numeric_limits<int>::max() };

constexpr int nick{ -worstValue };

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

//constructor
Engine::Engine(std::string_view fen, Castle castle) noexcept
	//engine
	: m_moveGen(), m_state(fen, castle), m_whiteToMove(true), 

	//search
	m_gameOver(), m_stopSearch(), m_searchDepth()

	//thread pool
	/*m_threadPool([this](const State& state, int& score, int depth, bool white, int alpha, int beta) {
			searchRun(state, score, depth, white, alpha, beta);
		})*/
{
	findWhiteSquares(m_state);
	findBlackSquares(m_state);
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

void Engine::findWhiteSquares(State& state) noexcept //TODO: maybe move to MoveGen?
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

void Engine::findBlackSquares(State& state) noexcept
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

bool Engine::makeLegalMove(State& state, bool white, Move move) noexcept
{
	state.makeMove(white, move);

	// Always update both sides //TODO: maybe go back to testing only one side?
	findWhiteSquares(state);
	findBlackSquares(state);

	return white
		? (state.pieceOccupancyT<Piece::BlackKing>().board() && !state.whiteKingInCheck())
		: (state.pieceOccupancyT<Piece::WhiteKing>().board() && !state.blackKingInCheck());
}




//search

//Move Engine::searchStartAsync(int depth) noexcept
//{
//	struct MoveResult 
//	{
//		State state;
//		Move move;
//		int score;
//	};
//
//	MoveList moves{ m_moveGen.generateMoves(m_whiteToMove, m_state) };
//	//TODO: maybe change to array? 
//	std::vector<MoveResult> moveResults;
//	moveResults.reserve(moves.size());
//
//	int legalMoves{};
//
//	for (Move move : moves)
//	{
//		moveResults.emplace_back(m_state, move, 0);
//
//		if (makeLegalMove(moveResults.back().state, m_whiteToMove, move))
//		{
//			++legalMoves;
//			m_threadPool.assign(moveResults.back().state, moveResults.back().score, depth, !m_whiteToMove, -bestValue, -worstValue);
//		}
//		else
//		{
//			moveResults.pop_back();
//		}
//	}
//
//	//check for mate
//	if (legalMoves == 0)
//	{
//		m_gameOver = true;
//	}
//
//	m_threadPool.start();
//	m_threadPool.wait();
//
//	return std::ranges::max_element(moveResults, [](MoveResult lhs, MoveResult rhs) { return lhs.score < rhs.score; })->move;
//}


//TODO: maybe check checks too get it?

//int Engine::quiescenceSearch(const State& state, int alpha, int beta, bool white) noexcept
//{
//	//track how many recursive calls have happened, 
//	thread_local int stopSearchCheck{};
//	++stopSearchCheck;
//
//	const int standardPat{ white ? evaluate(state) : -evaluate(state) };
//
//	if (standardPat >= beta)
//	{
//		return standardPat;
//	}
//
//	const CaptureList moves{ m_moveGen.generateCaptures(white, state) };
//	int legalMoves{};
//	int bestScore{ worstValue };
//
//	for (Move move : moves)
//	{
//		State stateCopy{ state };
//
//		if (makeLegalMove(stateCopy, white, move))
//		{
//			//check every 16384 calls if time is up, if it is stop the search
//			if ((stopSearchCheck & 0x3FFF) == 0 && m_stopSearch.load(std::memory_order_relaxed))
//			{
//				return 0;
//			}
//
//			++legalMoves;
//			
//			const int score{ -quiescenceSearch(stateCopy, -beta, -alpha, !white) };
//			bestScore = std::max(bestScore, score);
//			alpha = std::max(alpha, score);
//
//			if (alpha >= beta)
//			{
//				break;
//			}
//		}
//	}
//
//	if (legalMoves)
//	{
//		return bestScore;
//	}
//	else
//	{
//		return standardPat;
//	}
//}

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
	moves.sort();
	
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
			return worstValue + depth;
		}
		else
		{
			return 0;
		}
	}

	return bestScore;
}

ScoredMove Engine::searchStart(int depth) noexcept
{
	Move bestMove{ 0 };
	int bestScore{ worstValue };
	int alpha = worstValue;
	constexpr int beta = bestValue;

	MoveList moves{ m_moveGen.generateMoves(m_whiteToMove, m_state) };
	moves.sort();

	for (Move move : moves)
	{
		State stateCopy{ m_state };

		if (makeLegalMove(stateCopy, m_whiteToMove, move))
		{
			const int score{ -searchRun(stateCopy, depth - 1, -beta, -alpha, !m_whiteToMove) };

			if (score > bestScore)
			{
				bestScore = score;
				bestMove = move;
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

Move Engine::search() noexcept
{
	Move bestMove{ 0 };

	m_stopSearch = false;
	std::jthread timer{ [&]() {
		std::this_thread::sleep_for(std::chrono::seconds(10));
		m_stopSearch.store(true, std::memory_order_relaxed);
		}
	};

	for (int i{ 1 }; i < maxDepth; ++i)
	{
		const ScoredMove scoredMove{ searchStart(i) };

		if (m_stopSearch)
		{
			return bestMove;
		}
		else
		{
			bestMove = scoredMove.move;
			m_searchDepth = i;
			std::cout << std::format("{}: {}\n", scoredMove.move.string(), scoredMove.score);
		}
	}

	return bestMove;
}



//game
void Engine::play() noexcept
{
	while (true)
	{
		//draw board
		system("cls");
		m_state.print();
		std::cout << std::format("depth: {}\n", m_searchDepth);

		if (m_gameOver)
		{
			return;
		}
		else
		{
			if (!m_whiteToMove)
			{
				while (true)
				{
					//player move
					MoveList moves{ m_moveGen.generateMoves(m_whiteToMove, m_state) };
					const Move move{ playerMove(moves) };
					State stateCopy{ m_state };

					if (move.move() != 0 && makeLegalMove(stateCopy, m_whiteToMove, move))
					{
						m_state = stateCopy;
						m_whiteToMove = !m_whiteToMove;
						break;
					}
					else
					{
						std::cout << "not a legal move\n";
					}
				}
			}
			else
			{
				std::cout << "thinking\n";

				Move bestMove{ search() };

				if (bestMove.move() == 0)
				{
					m_gameOver = true;
				}
				else
				{
					bestMove.print();
					m_state.makeMove(m_whiteToMove, bestMove);
					m_whiteToMove = !m_whiteToMove;
				}
			}
		}
	}
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