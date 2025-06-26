#include "MoveList.h"

MoveList::MoveList() noexcept
	: m_back() { }

void MoveList::sort() noexcept
{

}

std::array<Move, maxLegalMoves>::const_iterator MoveList::begin() const noexcept
{
	return m_moves.begin() + whitePieceOffset;
}

std::array<Move, maxLegalMoves>::const_iterator MoveList::end() const noexcept
{
	return m_moves.begin() + m_back;
}