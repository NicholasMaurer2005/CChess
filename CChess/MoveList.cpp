#include "MoveList.h"

MoveList::MoveList() noexcept
	: m_back() { }


std::size_t MoveList::size() const noexcept
{
	return m_back;
}

void MoveList::sort() noexcept
{

}

std::array<Move, maxLegalMoves>::const_iterator MoveList::begin() const noexcept
{
	return m_moves.begin();
}

std::array<Move, maxLegalMoves>::const_iterator MoveList::end() const noexcept
{
	return m_moves.begin() + m_back;
}