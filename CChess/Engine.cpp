#include "Engine.h"

#include <iostream>

#include "MoveList.h"
#include "Move.h"



//constructor
Engine::Engine(std::string_view fen) noexcept
	: m_moveGen(), m_state(fen), m_perftCount() { }



//perft
void Engine::perftRun(int depth, bool white) noexcept
{
	if (depth == 0)
	{
		++m_perftCount;
		return;
	}

	const MoveList moves{ m_moveGen.generateMoves(white, m_state) };

	for (Move move : moves)
	{
		State stateCopy{ m_state };

		if (makeLegalMove(white, move))
		{
			/*move.print();
			stateCopy.print();
			m_state.print();
			std::cout << "\n\n";*/

			perftRun(depth - 1, !white);
		}

		//unmake move
		m_state = stateCopy;
	}
}

void Engine::perft(int depth) noexcept
{
	for (int i{ 1 }; i <= depth; i++)
	{
		perftRun(i, true);
		
		std::cout << "perft ply " << i << ": " << m_perftCount << '\n';
		m_perftCount = 0;
	}

	std::cout << "done" << std::endl;
}

void Engine::printMoves(bool white) noexcept
{
	const MoveList moves{ m_moveGen.generateMoves(white, m_state) };

	for (Move move : moves)
	{
		State stateCopy{ m_state };

		if (makeLegalMove(white, move))
		{
			move.print();
		}

		//unmake move
		m_state = stateCopy;
	}
}


//private methods
bool Engine::whiteKingInCheck() const noexcept
{
	const std::size_t kingSquare{ static_cast<std::size_t>(m_state.pieceOccupancyT<Piece::WhiteKing>().leastSignificantBit()) };

	const BitBoard attackers{
		m_moveGen.getWhitePawnMoves(kingSquare).board() & m_state.pieceOccupancyT<Piece::BlackPawn>().board()
		| m_moveGen.getKnightMoves(kingSquare).board() & m_state.pieceOccupancyT<Piece::BlackKnight>().board()
		| m_moveGen.getBishopMoves(kingSquare, m_state.occupancy()).board() & (m_state.pieceOccupancyT<Piece::BlackBishop>().board() | m_state.pieceOccupancyT<Piece::BlackQueen>().board())
		| m_moveGen.getRookMoves(kingSquare, m_state.occupancy()).board() & (m_state.pieceOccupancyT<Piece::BlackRook>().board() | m_state.pieceOccupancyT<Piece::BlackQueen>().board())
	};

	return attackers.board();
}

bool Engine::blackKingInCheck() const noexcept
{
	const std::size_t kingSquare{ static_cast<std::size_t>(m_state.pieceOccupancyT<Piece::BlackKing>().leastSignificantBit()) };

	const BitBoard attackers{
		m_moveGen.getBlackPawnMoves(kingSquare).board() & m_state.pieceOccupancyT<Piece::WhitePawn>().board()
		| m_moveGen.getKnightMoves(kingSquare).board() & m_state.pieceOccupancyT<Piece::WhiteKnight>().board()
		| m_moveGen.getBishopMoves(kingSquare, m_state.occupancy()).board() & (m_state.pieceOccupancyT<Piece::WhiteBishop>().board() | m_state.pieceOccupancyT<Piece::WhiteQueen>().board())
		| m_moveGen.getRookMoves(kingSquare, m_state.occupancy()).board() & (m_state.pieceOccupancyT<Piece::WhiteRook>().board() | m_state.pieceOccupancyT<Piece::WhiteQueen>().board())
		| m_moveGen.getKingMoves(kingSquare).board() & m_state.pieceOccupancyT<Piece::WhiteKing>().board() // optional
	};

	return attackers.board();
}

void Engine::findWhiteSquares() noexcept //TODO: maybe move to MoveGen?
{
	std::uint64_t squares{};

	BitBoard pawns{ m_state.pieceOccupancyT<Piece::WhitePawn>() };
	BitBoard knights{ m_state.pieceOccupancyT<Piece::WhiteKnight>() };
	BitBoard bishops{ m_state.pieceOccupancyT<Piece::WhiteBishop>() };
	BitBoard rooks{ m_state.pieceOccupancyT<Piece::WhiteRook>() };
	BitBoard queens{ m_state.pieceOccupancyT<Piece::WhiteQueen>() };
	BitBoard kings{ m_state.pieceOccupancyT<Piece::WhiteKing>() };

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

	m_state.setBlackSquares(squares);
}

bool Engine::makeLegalMove(bool white, Move move) noexcept
{
	m_state.makeMove(white, move);
	
	if (white)
	{
		findBlackSquares();
		return !m_state.whiteKingInCheck();
	}
	else
	{
		findWhiteSquares();
		return !m_state.blackKingInCheck();
	}
}