#include "State.h"



//constructors
State::State() noexcept
	: m_occupancy(), m_whiteOccupancy(), m_blackOccupancy(), m_pieceOccupancy(), m_castleRights(), m_kingInCheck() { }



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