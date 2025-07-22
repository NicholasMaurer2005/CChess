#pragma once

#include <cstdint>
#include <new>

constexpr int boardSize{ 64 };
constexpr int fileSize{ 8 };
constexpr int rankSize{ 8 };

#define cachealign alignas(std::hardware_destructive_interference_size)



// The reason NoPiece is 0 is because the Move class tests if a move is an attack or quite based on whether attackPiece() is 
// equal to NoPiece. There are almost always more quiet moves than attack moves so to create a quiet move NoPiece does not 
// need any initialization, i.e. there is no bitwize and shift needed. Does that make sense at all?
enum class Piece : std::uint32_t
{
	NoPiece = 0,
	WhitePawn = 1,
	WhiteKnight = 2,
	WhiteBishop = 3,
	WhiteRook = 4,
	WhiteQueen = 5,
	WhiteKing = 6,
	BlackPawn = 7,
	BlackKnight = 8,
	BlackBishop = 9,
	BlackRook = 10,
	BlackQueen = 11,
	BlackKing = 12
};

constexpr int pieceCount{ 13 };
constexpr int whitePieceOffset{ 1 };
constexpr int blackPieceOffset{ 7 };

//TODO: maybe move to its own header file?
enum class Castle : std::uint8_t //if you change type rememember to change operators
{
	WhiteKingSide	= 0b0001,
	WhiteQueenSide	= 0b0010,
	BlackKingSide	= 0b0100,
	BlackQueenSide	= 0b1000,
	WhiteBoth		= 0b0011,
	BlackBoth		= 0b1100,
	All				= 0b1111,
	None			= 0b0000
};

constexpr Castle operator& (Castle lhs, Castle rhs) noexcept
{
	return static_cast<Castle>(static_cast<std::uint8_t>(lhs) & static_cast<std::uint8_t>(rhs));
}

constexpr Castle operator^ (Castle lhs, Castle rhs) noexcept
{
	return static_cast<Castle>(static_cast<std::uint8_t>(lhs) ^ static_cast<std::uint8_t>(rhs));
}

constexpr Castle operator~ (Castle c) noexcept
{
	return static_cast<Castle>(~static_cast<std::uint8_t>(c));
}

constexpr Castle& operator&= (Castle& lhs, Castle rhs) noexcept
{
	lhs = lhs & rhs;
	return lhs;
}

constexpr Castle& operator^= (Castle& lhs, Castle rhs) noexcept
{
	lhs = lhs ^ rhs;
	return lhs;
}

enum Square
{
	a1, b1, c1, d1, e1, f1, g1, h1,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a8, b8, c8, d8, e8, f8, g8, h8
};
