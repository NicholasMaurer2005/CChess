#include "KillerMoveHistory.h"

#include "Move.h"


KillerMoveHistory::KillerMoveHistory() noexcept
	: m_moves()
{
	m_moves.fill({ 0, { 0, 0 } });
}

KillerMoves KillerMoveHistory::killerMoves(int depth) const noexcept
{
	const std::size_t index{ static_cast<std::size_t>(depth) };

	return { m_moves[index].moves[0], m_moves[index].moves[1] };
}

void KillerMoveHistory::push(int depth, Move move) noexcept
{
	const std::size_t plyIndex{ static_cast<std::size_t>(depth) };
	const std::size_t moveIndex{ m_moves[plyIndex].position & 1 };
	const Move storedMove{ m_moves[plyIndex].moves[moveIndex] };

	m_moves[plyIndex].moves[moveIndex] = move.attackPiece() == Piece::NoPiece ? move : storedMove;
	++m_moves[plyIndex].position;
}