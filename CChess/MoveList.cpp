#include "MoveList.h"

MoveList::MoveList() noexcept
	: m_back() { }

void MoveList::sort() noexcept
{

}

void MoveList::push(Move move) noexcept
{
	m_moves[m_back] = move;
	++m_back;
}

std::array<Move, maxLegalMoves>::const_iterator MoveList::begin() const noexcept
{
	return m_moves.begin();
}

std::array<Move, maxLegalMoves>::const_iterator MoveList::end() const noexcept
{
	return m_moves.begin() + m_back;
}