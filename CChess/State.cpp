#include "State.h"



//constructors
State::State() noexcept
	: m_occupancy(), m_whiteOccupancy(), m_blackOccupancy(), m_pieceOccupancy(), m_castleRights(), m_kingInCheck() { }



//private methods
void State::moveOccupancy(bool white, int sourceIndex, int destinationIndex) noexcept
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

void State::movePiece(Piece piece, int sourceIndex, int destinationIndex) noexcept
{
	m_pieceOccupancy[static_cast<std::size_t>(piece)].reset(sourceIndex);
	m_pieceOccupancy[static_cast<std::size_t>(piece)].set(destinationIndex);
}

void State::moveQuiet(bool white, Piece sourcePiece, int sourceIndex, int destinationIndex) noexcept
{
	moveOccupancy(white, sourceIndex, destinationIndex);
	movePiece(sourcePiece, sourceIndex, destinationIndex);
}

void State::moveCapture(bool white, Piece sourcePiece, Piece capturePiece, int sourceIndex, int destinationIndex) noexcept
{
	moveOccupancy(white, sourceIndex, destinationIndex);
	movePiece(sourcePiece, sourceIndex, destinationIndex);

	m_pieceOccupancy[static_cast<std::size_t>(capturePiece)].reset(destinationIndex);
}

void State::moveEnpassant(bool white, Piece sourcePiece, Piece capturePiece, int sourceIndex, int destinationIndex, int enpassantIndex) noexcept
{
	moveOccupancy(white, sourceIndex, destinationIndex);
	movePiece(sourcePiece, sourceIndex, destinationIndex);

	m_pieceOccupancy[static_cast<std::size_t>(capturePiece)].reset(enpassantIndex);
}

void State::moveCastle(Castle castle) noexcept
{
	switch (castle)
	{
	case Castle::WhiteKingSide:
		moveQuiet(true, Piece::WhiteKing, 4, 6);
		moveQuiet(true, Piece::WhiteRook, 7, 5);
		break;

	case Castle::WhiteQueenSide:
		moveQuiet(true, Piece::WhiteKing, 4, 2);
		moveQuiet(true, Piece::WhiteRook, 0, 3);
		break;

	case Castle::BlackKingSide:
		moveQuiet(false, Piece::BlackKing, 60, 62);
		moveQuiet(false, Piece::BlackRook, 63, 61);
		break;

	case Castle::BlackQueenSide:
		moveQuiet(false, Piece::BlackKing, 60, 58);
		moveQuiet(false, Piece::BlackRook, 56, 59);
		break;
	}
}

void State::moveQuietPromote(bool white, Piece sourcePiece, Piece promotePiece, int sourceIndex, int destinationIndex) noexcept
{
	moveOccupancy(white, sourceIndex, destinationIndex);

	m_pieceOccupancy[static_cast<std::size_t>(sourcePiece)].reset(sourceIndex);
	m_pieceOccupancy[static_cast<std::size_t>(promotePiece)].set(destinationIndex);
}

void State::moveCapturePromote(bool white, Piece sourcePiece, Piece attackPiece, Piece promotePiece, int sourceIndex, int destinationIndex) noexcept
{
	moveOccupancy(white, sourceIndex, destinationIndex);

	m_pieceOccupancy[static_cast<std::size_t>(sourcePiece)].reset(sourceIndex);
	m_pieceOccupancy[static_cast<std::size_t>(attackPiece)].reset(destinationIndex);
	m_pieceOccupancy[static_cast<std::size_t>(promotePiece)].set(destinationIndex);
}



//move
unmakeMoveInfo State::makeMove(bool white, Move move) noexcept
{
	unmakeMoveInfo info{ m_whiteEnpassantSquare, m_blackEnpassantSquare, m_castleRights, m_kingInCheck };

	if (move.castleFlag()) [[unlikely]]
	{
		//castle
		moveCastle(move.castleType());
	}
	else if (move.enpassantFlag()) [[unlikely]]
	{
		//enpassant
		const int enpassantIndex{ move.enpassantIndex() + (white ? 24 : 32) };
		moveEnpassant(white, move.sourcePiece(), move.attackPiece(), move.sourceIndex(), move.destinationIndex(), enpassantIndex);
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
					const int sourceIndex{ move.sourceIndex() };
					moveQuiet(white, move.sourcePiece(), sourceIndex, move.destinationIndex());
					
					if (white)
					{
						const int enpassantIndex{ sourceIndex + 8 };
						m_whiteEnpassantSquare = BitBoard(1ULL << enpassantIndex);
					}
					else
					{
						const int enpassantIndex{ sourceIndex - 8 };
						m_blackEnpassantSquare = BitBoard(1ULL << enpassantIndex);
					}
				}
				else
				{
					//normal quiet
					moveQuiet(white, move.sourcePiece(), move.sourceIndex(), move.destinationIndex());
				}
			}
			else
			{
				//normal capture
				moveCapture(white, move.sourcePiece(), move.attackPiece(), move.sourceIndex(), move.destinationIndex());
			}
		}
		else
		{
			if (attackPiece == Piece::NoPiece)
			{
				//quiet promote
				moveQuietPromote(white, move.sourcePiece(), promotePiece, move.sourceIndex(), move.destinationIndex());
			}
			else
			{
				//capture promote
				moveCapturePromote(white, move.sourcePiece(), attackPiece, promotePiece, move.sourceIndex(), move.destinationIndex());
			}
		}
	}

	return info;
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