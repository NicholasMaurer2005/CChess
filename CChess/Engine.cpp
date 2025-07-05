#include "Engine.h"

#include <iostream>
#include <limits>
#include <cstdlib>

#include "MoveList.h"
#include "Move.h"
#include "Evaluate.h"



//constructor
Engine::Engine(std::string_view fen, Castle castle) noexcept
	: m_moveGen(), m_state(fen, castle), m_searching(), m_whiteToMove(true), m_gameOver()
{
	findWhiteSquares();
	findBlackSquares();
}



//search
int Engine::searchRun(int depth, bool white) noexcept
{
	if (depth == 0) return evaluate(m_state);

	int bestScore{ std::numeric_limits<int>::min() };

	MoveList moves{ m_moveGen.generateMoves(white, m_state) };
	int legalMoves{};

	for (Move move : moves)
	{
		const State stateCopy{ m_state };

		if (makeLegalMove(white, move))
		{
			++legalMoves;
			bestScore = std::max(bestScore, -searchRun(depth - 1, !white));
		}

		//unmake move
		m_state = stateCopy;
	}

	//check for mate
	if (legalMoves == 0)
	{
		const int checkMateScore{ white ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max() };
		const bool kingInCheck{ white ? whiteKingInCheck() : blackKingInCheck() };
		return kingInCheck ? checkMateScore : 0;
	}
	else
	{
		return bestScore;
	}
}
	
Move Engine::search() noexcept
{
	int bestScore{ std::numeric_limits<int>::min() };
	Move bestMove{ 0 };

	MoveList moves{ m_moveGen.generateMoves(m_whiteToMove, m_state) };
	int legalMoves{};

	for (Move move : moves)
	{
		const State stateCopy{ m_state };

		if (makeLegalMove(m_whiteToMove, move))
		{
			++legalMoves;

			const int score{ -searchRun(5, !m_whiteToMove) };

			if (score > bestScore)
			{
				bestScore = score;
				bestMove = move;
			}
		}

		//unmake move
		m_state = stateCopy;
	}

	//check for mate
	if (legalMoves == 0)
	{
		m_gameOver = true;
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

		const Move bestMove{ search() };
		
		if (m_gameOver)
		{
			return;
		}
		else
		{
			m_state.makeMove(m_whiteToMove, bestMove);
			m_whiteToMove = !m_whiteToMove;
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

		if (makeLegalMove(white, move))
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

bool Engine::makeLegalMove(bool white, Move move) noexcept
{
	m_state.makeMove(white, move);
	
	// Always update both sides //TODO: maybe go back to testing only one side?
	findWhiteSquares();
	findBlackSquares();

	return white 
		? (m_state.pieceOccupancyT<Piece::BlackKing>().board() && !m_state.whiteKingInCheck()) 
		: (m_state.pieceOccupancyT<Piece::WhiteKing>().board() && !m_state.blackKingInCheck());
}