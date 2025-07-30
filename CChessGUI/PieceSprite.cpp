#include "PieceSprite.h"

#include <type_traits>
#include <array>

static_assert(std::is_standard_layout_v<PieceSprite>);
static_assert(sizeof(PieceSprite) == 64);

static constexpr float squareOffset{ 0.125f };

static constexpr float textureSheetWidth{ 608.0f };
static constexpr float textureSheetHeight{ 546.0f };
static constexpr float pieceSize{ 150.0f };
static constexpr float pieceTextureWidth{ pieceSize / textureSheetWidth };
static constexpr float pieceTextureHeight{ pieceSize / textureSheetHeight };


static constexpr int whitePawnX{ 1 };
static constexpr int whitePawnY{ 153 };

static constexpr int whiteKnightX{ 305 };
static constexpr int whiteKnightY{ 305 };

static constexpr int whiteBishopX{ 1 };
static constexpr int whiteBishopY{ 305 };

static constexpr int whiteRookX{ 457 };
static constexpr int whiteRookY{ 305 };

static constexpr int whiteQueenX{ 547 };
static constexpr int whiteQueenY{ 153 };

static constexpr int whiteKingX{ 153 };
static constexpr int whiteKingY{ 305 };


static constexpr int blackPawnX{ 1 };
static constexpr int blackPawnY{ 153 };

static constexpr int blackKnightX{ 305 };
static constexpr int blackKnightY{ 1 };

static constexpr int blackBishopX{ 1 };
static constexpr int blackBishopY{ 1 };

static constexpr int blackRookX{ 305 };
static constexpr int blackRookY{ 153 };

static constexpr int blackQueenX{ 153 };
static constexpr int blackQueenY{ 153 };

static constexpr int blackKingX{ 153 };
static constexpr int blackKingY{ 1 };

struct alignas(32) TextureCoords
{
	float x1, y1, x2, y2, x3, y3, x4, y4;
};

static consteval TextureCoords generateTextureCoords(int x, int y)
{
	const float normalizedX{ x / textureSheetWidth };
	const float normalizedY{ y / textureSheetHeight };

	return {
		normalizedX, normalizedY,
		normalizedX + pieceTextureWidth, normalizedY,
		normalizedX + pieceTextureWidth, normalizedY + pieceTextureHeight,
		normalizedX, normalizedY + pieceTextureHeight
	};
}

static constexpr std::array<TextureCoords, 12> pieceTextureCoords{
	generateTextureCoords(whitePawnX, whitePawnY),
	generateTextureCoords(whiteKnightX, whiteKnightY),
	generateTextureCoords(whiteBishopX, whiteBishopY),
	generateTextureCoords(whiteRookX, whiteRookY),
	generateTextureCoords(whiteQueenX, whiteQueenY),
	generateTextureCoords(whiteKingX, whiteKingY),
	generateTextureCoords(blackPawnX, blackPawnY),
	generateTextureCoords(blackKnightX, blackKnightY),
	generateTextureCoords(blackBishopX, blackBishopY),
	generateTextureCoords(blackRookX, blackRookY),
	generateTextureCoords(blackQueenX, blackQueenY),
	generateTextureCoords(blackKingX, blackKingY)
};


void PieceSprite::initTexture(Piece piece) noexcept
{
	const TextureCoords texture{ pieceTextureCoords[static_cast<std::size_t>(piece)] };

	m_texX1 = texture.x1;
	m_texY1 = texture.y1;
	m_texX2 = texture.x2;
	m_texY2 = texture.y2;
	m_texX3 = texture.x3;
	m_texY3 = texture.y3;
	m_texX4 = texture.x4;
	m_texY4 = texture.y4;
}

PieceSprite::PieceSprite(int rank, int file, Piece piece) noexcept
	: m_posX1(), m_posY1(), m_texX1(), m_texY1(),
	  m_posX2(), m_posY2(), m_texX2(), m_texY2(),
	  m_posX3(), m_posY3(), m_texX3(), m_texY3(),
	  m_posX4(), m_posY4(), m_texX4(), m_texY4()
{
	initTexture(piece);
	move(rank, file);
}

void PieceSprite::move(int rank, int file) noexcept
{
	m_posX1 = file * squareOffset;
	m_posY1 = rank * squareOffset;

	m_posX2 = m_posX1 + squareOffset;
	m_posY2 = m_posY1;

	m_posX3 = m_posX1 + squareOffset;
	m_posY3 = m_posY1 + squareOffset;

	m_posX4 = m_posX1;
	m_posY4 = m_posY1 + squareOffset;
}