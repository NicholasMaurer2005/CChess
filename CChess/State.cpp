#include "State.h"



//constructors
State::State() noexcept
	: m_occupancy(), m_whiteOccupancy(), m_blackOccupancy(), m_pieceOccupancy(), m_castleRights(), m_kingInCheck() { }



//private methods
void State::moveOccupancy(bool white, std::size_t sourceIndex, std::size_t destinationIndex) noexcept
{
	m_occupancy.reset(sourceIndex);
	m_occupancy.set(destinationIndex);

	if (white)
	{
		m_whiteOccupancy.reset(sourceIndex);
		m_whiteOccupancy.set(destinationIndex);
	}
	else
	{
		m_blackOccupancy.reset(sourceIndex);
		m_blackOccupancy.set(destinationIndex);
	}
}

void State::movePiece(Piece piece, std::size_t sourceIndex, std::size_t destinationIndex) noexcept
{
	m_pieceOccupancy[static_cast<std::size_t>(piece)].reset(sourceIndex);
	m_pieceOccupancy[static_cast<std::size_t>(piece)].set(destinationIndex);
}

void State::moveQuiet(bool white, Piece sourcePiece, std::size_t sourceIndex, std::size_t destinationIndex) noexcept
{
	moveOccupancy(white, sourceIndex, destinationIndex);
	movePiece(sourcePiece, sourceIndex, destinationIndex);
}

void State::moveCapture(bool white, Piece sourcePiece, Piece capturePiece, std::size_t sourceIndex, std::size_t destinationIndex) noexcept
{
	moveOccupancy(white, sourceIndex, destinationIndex);
	movePiece(sourcePiece, sourceIndex, destinationIndex);

	m_pieceOccupancy[static_cast<std::size_t>(capturePiece)].reset(destinationIndex);
}

void State::moveEnpassant(bool white, Piece sourcePiece, Piece capturePiece, std::size_t sourceIndex, std::size_t destinationIndex, std::size_t enpassantIndex) noexcept
{
	moveOccupancy(white, sourceIndex, destinationIndex);
	movePiece(sourcePiece, sourceIndex, destinationIndex);

	m_pieceOccupancy[static_cast<std::size_t>(capturePiece)].reset(enpassantIndex);
}

void State::moveCastle(bool white, Castle castle) noexcept
{
	//TODO
}



//move
unmakeMoveInfo State::makeMove(bool white, Move move) noexcept
{
	unmakeMoveInfo info{ m_whiteEnpassantSquare, m_blackEnpassantSquare, m_castleRights, m_kingInCheck };

	if (move.castleFlag()) [[unlikely]]
	{
		//castle move
	}
	else if (move.enpassantFlag()) [[unlikely]]
	{
		//enpassant
		(white ? 24 : 32);
	}
	else
	{
		const Piece promotePiece{ move.promotePiece() };
		const Piece attackPiece{ move.attackPiece() };

		if (promotePiece == Piece::NoPiece) [[likely]]
		{
			if (attackPiece == Piece::NoPiece)
			{

				if (move.doublePawnFlag()) [[unlikely]]
				{
					//double pawn push
				}
				else
				{
					//normal quiet
				}
			}
			else
			{
				//normal capture
			}
		}
		else
		{
			if (attackPiece == Piece::NoPiece)
			{
				//quiet promote
			}
			else
			{
				//capture promote
			}
		}
	}
}

void State::unmakeMove(bool white, Move move, unmakeMoveInfo info)
{

}



//getters
BitBoard State::occupancy() const noexcept
{
	return m_occupancy;
}

BitBoard State::whiteOccupancy() const noexcept
{
	return m_whiteOccupancy;
}

BitBoard State::blackOccupancy() const noexcept
{
	return m_blackOccupancy;
}

BitBoard State::pieceOccupancy(Piece piece) const noexcept
{
	return m_pieceOccupancy[static_cast<std::size_t>(piece)];
}

BitBoard State::whiteEnpassantSquare() const noexcept
{
	return m_whiteEnpassantSquare;
}

BitBoard State::blackEnpassantSquare() const noexcept
{
	return m_blackEnpassantSquare;
}

bool State::castleWhiteKingSide() const noexcept
{
	return static_cast<bool>(static_cast<std::uint32_t>(m_castleRights) & static_cast<std::uint32_t>(Castle::WhiteKingSide));
}

bool State::castleWhiteQueenSide() const noexcept
{
	return static_cast<bool>(static_cast<std::uint32_t>(m_castleRights) & static_cast<std::uint32_t>(Castle::WhiteQueenSide));
}

bool State::castleBlackKingSide() const noexcept
{
	return static_cast<bool>(static_cast<std::uint32_t>(m_castleRights) & static_cast<std::uint32_t>(Castle::BlackKingSide));
}

bool State::castleBlackQueenSide() const noexcept
{
	return static_cast<bool>(static_cast<std::uint32_t>(m_castleRights) & static_cast<std::uint32_t>(Castle::BlackQueenSide));
}

bool State::kingInCheck() const noexcept
{
	return m_kingInCheck;
}