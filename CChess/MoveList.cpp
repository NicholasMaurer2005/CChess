#include "MoveList.h"

MoveList::MoveList() 
	: m_back() { }

void MoveList::sort()
{

}

void MoveList::push(Move move)
{
	m_moves[m_back] = move;
	++m_back;
}

std::array<Move, maxLegalMoves>::const_iterator MoveList::begin() const
{
	return m_moves.begin();
}

std::array<Move, maxLegalMoves>::const_iterator MoveList::end() const
{
	return m_moves.begin() + m_back;
}