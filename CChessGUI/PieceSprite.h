#pragma once

#include <cstddef>

class alignas(64) PieceSprite
{
public:

	enum class Piece : std::size_t
	{
		WhitePawn = 0,
		WhiteKnight = 1,
		WhiteBishop = 2,
		WhiteRook = 3,
		WhiteQueen = 4,
		WhiteKing = 5,
		BlackPawn = 6,
		BlackKnight = 7,
		BlackBishop = 8,
		BlackRook = 9,
		BlackQueen = 10,
		BlackKing = 11,
		NoPiece = 12
	};

private:

	float m_x1{};
	float m_y1{};
	float m_u1{};
	float m_v1{};

	float m_x2{};
	float m_y2{};
	float m_u2{};
	float m_v2{};

	float m_x3{};
	float m_y3{};
	float m_u3{};
	float m_v3{};

	float m_x4{};
	float m_y4{};
	float m_u4{};
	float m_v4{};



private:

	void initTexture(Piece piece) noexcept;
	


public:

	PieceSprite() noexcept {};

	PieceSprite(int rank, int file, Piece piece) noexcept;

	PieceSprite(Piece piece) noexcept;

	void move(int rank, int file) noexcept;

	void dump() const noexcept;
};


