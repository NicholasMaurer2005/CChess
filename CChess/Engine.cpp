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

		m_state.makeMove(white, move);

		perftRun(depth - 1, !white);

		//unmake move
		m_state = stateCopy;
	}
}

void Engine::perft(int depth) noexcept
{
	for (int i{ 1 }; i < depth; i++)
	{
		perftRun(i, true);
		
		std::cout << "perft ply " << i << ": " << m_perftCount << '\n';
		m_perftCount = 0;
	}

	std::cout << "done" << std::endl;
}



//private methods
bool Engine::isWhiteKingInCheck() const noexcept
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

bool Engine::isBlackKingInCheck() const noexcept
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