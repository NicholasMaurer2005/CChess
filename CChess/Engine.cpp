#include "Engine.h"

#include <iostream>
#include <limits>
#include <cstdlib>
#include <vector>
#include <ranges>

#include "MoveList.h"
#include "Move.h"
#include "Evaluate.h"


static constexpr int bestValue{ std::numeric_limits<int>::max() - 1 };
static constexpr int worstValue{ std::numeric_limits<int>::min() + 1 };


//constructor
Engine::Engine(std::string_view fen, Castle castle) noexcept
	: m_moveGen(), m_state(fen, castle), m_searching(), m_whiteToMove(true), m_gameOver(), 
	m_threadPool([this](const State& state, int& score, int depth, bool white, int alpha, int beta) 
		{
			searchRun(state, score, depth, white, alpha, beta);
		})
{
	findWhiteSquares();
	findBlackSquares();
}



//search
void Engine::searchRun(const State& state, int& score, int depth, bool white, int alpha, int beta) noexcept
{
	if (depth == 0)
	{
		score = white ? -evaluate(state) : evaluate(state);
		return;
	}

	MoveList moves{ m_moveGen.generateMoves(white, state) };
	int bestScore{ worstValue };
	int legalMoves{};

	for (Move move : moves)
	{
		State stateCopy{ state };

		if (makeLegalMove(stateCopy, white, move))
		{
			++legalMoves;
			int score{};
			searchRun(stateCopy, score, depth - 1, !white, -beta, -alpha);
			bestScore = std::max(bestScore, score);
			alpha = std::max(alpha, score);

			if (alpha >= beta)
			{
				break;
			}
		}
	}

	//check for mate
	if (legalMoves == 0)
	{
		const bool kingInCheck{ white ? whiteKingInCheck() : blackKingInCheck() };
		score = kingInCheck ? -worstValue : 0;
	}
	else
	{
		score = -bestScore;
	}

	return;
}

//exception in release mode. king missing when trying to call popLeastSignificantBit()
Move Engine::search() noexcept
{
	struct MoveResult 
	{
		State state;
		Move move;
		int score;
	};

	MoveList moves{ m_moveGen.generateMoves(m_whiteToMove, m_state) };
	//TODO: maybe change to array? 
	std::vector<MoveResult> moveResults;
	moveResults.reserve(moves.size());

	int legalMoves{};

	for (Move move : moves)
	{
		moveResults.emplace_back(m_state, move, 0);

		if (makeLegalMove(moveResults.back().state, m_whiteToMove, move))
		{
			++legalMoves;
			m_threadPool.assign(moveResults.back().state, moveResults.back().score, 6, !m_whiteToMove, -bestValue, -worstValue);
		}
		else
		{
			moveResults.pop_back();
		}
	}

	//check for mate
	if (legalMoves == 0)
	{
		m_gameOver = true;
	}

	m_threadPool.start();
	m_threadPool.wait();

	return std::ranges::max_element(moveResults, [](MoveResult lhs, MoveResult rhs) { return lhs.score < rhs.score; })->move;
}



//game
void Engine::play() noexcept
{
	while (true)
	{
		//draw board
		system("cls");
		m_state.print();

		const Move bestMove{ search() };
		
		if (m_gameOver)
		{
			return;
		}
		else
		{
			if (bestMove.move() == 0)
			{
				m_gameOver = true;
			}
			else
			{
				m_state.makeMove(m_whiteToMove, bestMove);
				m_whiteToMove = !m_whiteToMove;
			}
		}
	}
}



//perft
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

void Engine::perft(int depth) noexcept
{
	for (int i{ 1 }; i <= depth; i++)
	{
		std::cout << "perft ply " << i << ": " << perftRun(i, true) << '\n';
	}

	std::cout << "done" << std::endl;
}

//void Engine::printMoves(bool white) noexcept
//{
//	const MoveList moves{ m_moveGen.generateMoves(white, m_state) };
//
//	for (Move move : moves)
//	{
//		unmakeMoveInfo info{};
//
//		if (makeLegalMove(white, move, info))
//		{
//			move.print();
//		}
//
//		//unmake move
//		m_state.unmakeMove(white, move, info);
//	}
//}
//
//void Engine::printMoves(bool white, int depth) noexcept
//{
//	const MoveList moves{ m_moveGen.generateMoves(white, m_state) };
//
//	for (Move move : moves)
//	{
//		unmakeMoveInfo info{};
//
//		if (makeLegalMove(white, move, info))
//		{
//			std::cout << move.string() << ": " << perftRun(depth, !white) << '\n';
//		}
//
//		//unmake move
//		m_state.unmakeMove(white, move, info);
//	}
//}



//private methods
bool Engine::whiteKingInCheck() const noexcept
{
	return m_state.pieceOccupancyT<Piece::WhiteKing>().board() & m_state.blackSquares().board();
}

bool Engine::blackKingInCheck() const noexcept
{
	return m_state.pieceOccupancyT<Piece::BlackKing>().board() & m_state.whiteSquares().board();
}

void Engine::findWhiteSquares() noexcept //TODO: maybe move to MoveGen?
{
	std::uint64_t squares{};

	BitBoard pawns{ m_state.pieceOccupancyT<Piece::WhitePawn>() };
	BitBoard knights{ m_state.pieceOccupancyT<Piece::WhiteKnight>() };
	BitBoard bishops{ m_state.pieceOccupancyT<Piece::WhiteBishop>() };
	BitBoard rooks{ m_state.pieceOccupancyT<Piece::WhiteRook>() };
	BitBoard queens{ m_state.pieceOccupancyT<Piece::WhiteQueen>() };
	BitBoard kings{ m_state.pieceOccupancyT<Piece::WhiteKing>() }; //TODO: refactor?

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
		squares |= m_moveGen.getBishopMoves(index, m_state.occupancy()).board();
	}

	while (rooks.board())
	{
		const int index{ rooks.popLeastSignificantBit() };
		squares |= m_moveGen.getRookMoves(index, m_state.occupancy()).board();
	}

	while (queens.board())
	{
		const int index{ queens.popLeastSignificantBit() };
		squares |= m_moveGen.getBishopMoves(index, m_state.occupancy()).board() | m_moveGen.getRookMoves(index, m_state.occupancy()).board();
	}

	const int kingIndex{ kings.popLeastSignificantBit() };
	squares |= m_moveGen.getKingMoves(kingIndex).board();

	m_state.setWhiteSquares(squares);
}

void Engine::findBlackSquares() noexcept
{
	std::uint64_t squares{};

	BitBoard pawns{ m_state.pieceOccupancyT<Piece::BlackPawn>() };
	BitBoard knights{ m_state.pieceOccupancyT<Piece::BlackKnight>() };
	BitBoard bishops{ m_state.pieceOccupancyT<Piece::BlackBishop>() };
	BitBoard rooks{ m_state.pieceOccupancyT<Piece::BlackRook>() };
	BitBoard queens{ m_state.pieceOccupancyT<Piece::BlackQueen>() };
	BitBoard kings{ m_state.pieceOccupancyT<Piece::BlackKing>() };

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
		squares |= m_moveGen.getBishopMoves(index, m_state.occupancy()).board();
	}

	while (rooks.board())
	{
		const int index{ rooks.popLeastSignificantBit() };
		squares |= m_moveGen.getRookMoves(index, m_state.occupancy()).board();
	}

	while (queens.board())
	{
		const int index{ queens.popLeastSignificantBit() };
		squares |= m_moveGen.getBishopMoves(index, m_state.occupancy()).board() | m_moveGen.getRookMoves(index, m_state.occupancy()).board();
	}

	const int kingIndex{ kings.popLeastSignificantBit() };
	squares |= m_moveGen.getKingMoves(kingIndex).board();

	m_state.setBlackSquares(squares);
}

bool Engine::makeLegalMove(State& state, bool white, Move move) noexcept
{
	state.makeMove(white, move);
	
	// Always update both sides //TODO: maybe go back to testing only one side?
	findWhiteSquares();
	findBlackSquares();

	return white 
		? (state.pieceOccupancyT<Piece::BlackKing>().board() && !state.whiteKingInCheck()) 
		: (state.pieceOccupancyT<Piece::WhiteKing>().board() && !state.blackKingInCheck());
}