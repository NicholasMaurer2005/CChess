#include "PieceSprite.h"

#include <type_traits>
#include <array>

static_assert(std::is_standard_layout_v<PieceSprite>);
static_assert(sizeof(PieceSprite) == 64);

static constexpr float squareOffset{ 0.125f };

static constexpr int textureSheetWidth{ 600 };
static constexpr int textureSheetHeight{ 450 };
static constexpr int pieceSize{ 150 };
static constexpr float pieceTextureWidth{ static_cast<float>(pieceSize) / textureSheetWidth };
static constexpr float pieceTextureHeight{ static_cast<float>(pieceSize) / textureSheetHeight };


static constexpr int whitePawnX{ 450 };
static constexpr int whitePawnY{ 0 };

static constexpr int whiteKnightX{ 300 };
static constexpr int whiteKnightY{ 300 };

static constexpr int whiteBishopX{ 0 };
static constexpr int whiteBishopY{ 300 };

static constexpr int whiteRookX{ 450 };
static constexpr int whiteRookY{ 300 };

static constexpr int whiteQueenX{ 450 };
static constexpr int whiteQueenY{ 150 };

static constexpr int whiteKingX{ 150 };
static constexpr int whiteKingY{ 300 };


static constexpr int blackPawnX{ 0 };
static constexpr int blackPawnY{ 150 };

static constexpr int blackKnightX{ 300 };
static constexpr int blackKnightY{ 0 };

static constexpr int blackBishopX{ 0 };
static constexpr int blackBishopY{ 0 };

static constexpr int blackRookX{ 300 };
static constexpr int blackRookY{ 150 };

static constexpr int blackQueenX{ 150 };
static constexpr int blackQueenY{ 150 };

static constexpr int blackKingX{ 150 };
static constexpr int blackKingY{ 0 };

struct alignas(32) TextureCoords
{
	float x1, y1, x2, y2, x3, y3, x4, y4;
};

static consteval TextureCoords generateTextureCoords(int x, int y)
{
	const float normalizedX{ static_cast<float>(x) / textureSheetWidth };
	const float normalizedY{ static_cast<float>(y) / textureSheetHeight };

	return {
		//bottem right
		normalizedX, normalizedY + pieceTextureHeight,
		//top right
		normalizedX, normalizedY,
		//top left
		normalizedX + pieceTextureWidth, normalizedY,
		//bottem left
		normalizedX + pieceTextureWidth, normalizedY + pieceTextureHeight
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
	const float topLeftX{ file * squareOffset };
	const float topLeftY{ rank * squareOffset };

	//top left
	m_posX1 = 2.0f * topLeftX - 1.0f;
	m_posY1 = 2.0f * topLeftY - 1.0f;

	//bottem left
	m_posX2 = 2.0f * topLeftX - 1.0f;
	m_posY2 = 2.0f * (topLeftY + squareOffset) - 1.0f;

	//bottem right
	m_posX3 = 2.0f * (topLeftX + squareOffset) - 1.0f;
	m_posY3 = 2.0f * (topLeftY + squareOffset) - 1.0f;

	//top right
	m_posX4 = 2.0f * (topLeftX + squareOffset) - 1.0f;
	m_posY4 = 2.0f * topLeftY - 1.0f;
}