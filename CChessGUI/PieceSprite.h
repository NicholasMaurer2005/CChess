#pragma once

#include <cstddef>



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



class alignas(64) PieceSprite
{
private:

	float m_posX1;
	float m_posY1;
	float m_texX1;
	float m_texY1;

	float m_posX2;
	float m_posY2;
	float m_texX2;
	float m_texY2;

	float m_posX3;
	float m_posY3;
	float m_texX3;
	float m_texY3;

	float m_posX4;
	float m_posY4;
	float m_texX4;
	float m_texY4;



private:

	void initTexture(Piece piece) noexcept;
	


public:

	PieceSprite(int rank, int file, Piece piece) noexcept;

	PieceSprite(Piece piece) noexcept;

	void move(int rank, int file) noexcept;
};


