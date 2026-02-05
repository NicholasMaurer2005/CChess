#include "PieceSprite.h"

#include <type_traits>
#include <array>
#include <iostream>
#include <format>



static_assert(std::is_standard_layout_v<PieceSprite>);
static_assert(sizeof(PieceSprite) == 64);



template<typename T>
struct Vec2
{
	T x, y;
};



static constexpr float squareOffset{ 0.125f };
static constexpr Vec2 textureSheetSize{ 600, 450 };
static constexpr Vec2 pieceSize{ 150, 150 };
static constexpr Vec2 pieceTextureSize{ static_cast<float>(pieceSize.x) / textureSheetSize.x, static_cast<float>(pieceSize.y) / textureSheetSize.y };

static constexpr Vec2 whitePawnLocation{ 450, 0 };
static constexpr Vec2 whiteKnightLocation{ 300, 300 };
static constexpr Vec2 whiteBishopLocation{ 0, 300 };
static constexpr Vec2 whiteRookLocation{ 450, 300 };
static constexpr Vec2 whiteQueenLocation{ 450, 150 };
static constexpr Vec2 whiteKingLocation{ 150, 300 };
static constexpr Vec2 blackPawnLocation{ 0, 150 };
static constexpr Vec2 blackKnightLocation{ 300, 0 };
static constexpr Vec2 blackBishopLocation{ 0, 0 };
static constexpr Vec2 blackRookLocation{ 300, 150 };
static constexpr Vec2 blackQueenLocation{ 150, 150 };
static constexpr Vec2 blackKingLocation{ 150, 0 };


//helpers
struct alignas(32) TextureCoords
{
	float u1, v1, u2, v2, u3, v3, u4, v4;
};

static consteval TextureCoords generateTextureCoords(int x, int y)
{
	const Vec2 normalized{ static_cast<float>(x) / textureSheetSize.x, static_cast<float>(y) / textureSheetSize.y };

	return {
		//bottem left
		normalized.x, normalized.y,
		//bottem right
		normalized.x + pieceTextureSize.x, normalized.y,
		//top right
		normalized.x + pieceTextureSize.x, normalized.y + pieceTextureSize.y,
		//top left
		normalized.x, normalized.y + pieceTextureSize.y
	};
}

static constexpr std::array<TextureCoords, 12> pieceTextureCoords{
	generateTextureCoords(whitePawnLocation.x, whitePawnLocation.y),
	generateTextureCoords(whiteKnightLocation.x, whiteKnightLocation.y),
	generateTextureCoords(whiteBishopLocation.x, whiteBishopLocation.y),
	generateTextureCoords(whiteRookLocation.x, whiteRookLocation.y),
	generateTextureCoords(whiteQueenLocation.x, whiteQueenLocation.y),
	generateTextureCoords(whiteKingLocation.x, whiteKingLocation.y),
	generateTextureCoords(blackPawnLocation.x, blackPawnLocation.y),
	generateTextureCoords(blackKnightLocation.x, blackKnightLocation.y),
	generateTextureCoords(blackBishopLocation.x, blackBishopLocation.y),
	generateTextureCoords(blackRookLocation.x, blackRookLocation.y),
	generateTextureCoords(blackQueenLocation.x, blackQueenLocation.y),
	generateTextureCoords(blackKingLocation.x, blackKingLocation.y)
};



//private methods
void PieceSprite::initTexture(Piece piece) noexcept
{
	const TextureCoords texture{ pieceTextureCoords[static_cast<std::size_t>(piece)] };

	m_u1 = texture.u1;
	m_v1 = texture.v1;
	m_u2 = texture.u2;
	m_v2 = texture.v2;
	m_u3 = texture.u3;
	m_v3 = texture.v3;
	m_u4 = texture.u4;
	m_v4 = texture.v4;
}



//constructors
PieceSprite::PieceSprite(int rank, int file, Piece piece) noexcept
{
	initTexture(piece);
	move(rank, file);
}

PieceSprite::PieceSprite(Piece piece) noexcept
{
	initTexture(piece);

	//bottem left
	m_x1 = -squareOffset;
	m_y1 = squareOffset;

	//bottem right
	m_x2 = squareOffset;
	m_y2 = squareOffset;

	//top right
	m_x3 = squareOffset;
	m_y3 = -squareOffset;

	//top left
	m_x4 = -squareOffset;
	m_y4 = -squareOffset;
}



//public methods
void PieceSprite::move(int rank, int file) noexcept
{
	const float topLeftX{ file * squareOffset };
	const float topLeftY{ rank * squareOffset };

	//bottem left
	m_x1 = 2.0f * topLeftX - 1.0f;
	m_y1 = 2.0f * (topLeftY + squareOffset) - 1.0f;

	//bottem right
	m_x2 = 2.0f * (topLeftX + squareOffset) - 1.0f;
	m_y2 = 2.0f * (topLeftY + squareOffset) - 1.0f;

	//top right
	m_x3 = 2.0f * (topLeftX + squareOffset) - 1.0f;
	m_y3 = 2.0f * topLeftY - 1.0f;

	//top left
	m_x4 = 2.0f * topLeftX - 1.0f;
	m_y4 = 2.0f * topLeftY - 1.0f;
}

void PieceSprite::dump() const noexcept
{
	std::cout << std::format(
		"pos1: {:.3f}, {:.3f} | tex1: {:.3f}, {:.3f}\npos2: {:.3f}, {:.3f} | tex2: {:.3f}, {:.3f}\npos3: {:.3f}, {:.3f} | tex3: {:.3f}, {:.3f}\npos4: {:.3f}, {:.3f} | tex4: {:.3f}, {:.3f}\n\n", 
		m_x1, m_y1, m_u1, m_v1, 
		m_x2, m_y2, m_u2, m_v2, 
		m_x3, m_y3, m_u3, m_v3, 
		m_x4, m_y4, m_u4, m_v4
	);
}