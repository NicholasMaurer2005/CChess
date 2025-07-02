#include "Engine.h"

#include <iostream>

#include "MoveList.h"
#include "Move.h"



//constructor
Engine::Engine(std::string_view fen, Castle castle) noexcept
	: m_moveGen(), m_state(fen, castle), m_perftCount()
{
	findWhiteSquares();
	findBlackSquares();
}


//perft

static int mates{};

void Engine::perftRun(int depth, bool white) noexcept
{
	if (depth == 0)
	{
		++m_perftCount; //TODO: maybe make recursive sum?
		return;
	}

	const MoveList moves{ m_moveGen.generateMoves(white, m_state) };

	int legalMovesCount{};

	for (Move move : moves)
	{
		State stateCopy{ m_state };

		if (makeLegalMove(white, move))
		{
			++legalMovesCount;
			perftRun(depth - 1, !white);
		}

		//unmake move
		m_state = stateCopy;
	}

	if (legalMovesCount == 0 && (white ? whiteKingInCheck() : blackKingInCheck()))
	{
		++mates;
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
	std::cout << mates << std::endl;
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

void Engine::printMoves(bool white, int depth) noexcept
{
	const MoveList moves{ m_moveGen.generateMoves(white, m_state) };

	if (moves.size() == 0 && (white ? whiteKingInCheck() : blackKingInCheck()))
	{
		mates++;
		return;
	}

	for (Move move : moves)
	{
		State stateCopy{ m_state };

		if (makeLegalMove(white, move))
		{
			if (move.sourceIndex() == g2 && move.destinationIndex() == g4)
			{
				stateCopy.whiteSquares().print();
				m_state.whiteSquares().print();
				std::cout << "\n\n\n";

				stateCopy.blackSquares().print();
				m_state.blackSquares().print();
				std::cout << "\n\n\n";

				stateCopy.occupancy().print();
				m_state.occupancy().print();
				std::cout << "\n\n\n";

				stateCopy.whiteOccupancy().print();
				m_state.whiteOccupancy().print();
				std::cout << "\n\n\n";

				stateCopy.blackOccupancy().print();
				m_state.blackOccupancy().print();
				std::cout << "\n\n\n";

				for (std::uint32_t i{ whitePieceOffset }; i < pieceCount; ++i)
				{
					const Piece piece{ static_cast<Piece>(i) };
					stateCopy.pieceOccupancy(piece).print();
					m_state.pieceOccupancy(piece).print();
					std::cout << "\n\n\n";
				}
			}

			std::cout << move.string();

			perftRun(1, !white);

			std::cout << ": " << m_perftCount << '\n';
			m_perftCount = 0;
		}

		//unmake move
		m_state = stateCopy;
	}

	std::cout << "mates " << mates << std::endl;
}



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
	if (move.attackPiece() == Piece::WhiteKing || move.attackPiece() == Piece::BlackKing)
	{
		throw;
	}

	m_state.makeMove(white, move);
	

	// Always update both sides //TODO: maybe go back to testing only one side?
	findWhiteSquares();
	findBlackSquares();

	

	return white 
		? (m_state.pieceOccupancyT<Piece::BlackKing>().board() && !m_state.whiteKingInCheck()) 
		: (m_state.pieceOccupancyT<Piece::WhiteKing>().board() && !m_state.blackKingInCheck());
}