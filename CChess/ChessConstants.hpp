#pragma once

#include <cstdint>
#include <new>

constexpr int boardSize{ 64 };
constexpr int fileSize{ 8 };
constexpr int rankSize{ 8 };

#define cachealign alignas(std::hardware_destructive_interference_size)

//TODO: maybe add board square enum?


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
enum class Castle : std::uint32_t //if you change type rememember to change operators
{
	WhiteKingSide	= 0b0001,
	WhiteQueenSide	= 0b0010,
	BlackKingSide	= 0b0100,
	BlackQueenSide	= 0b1000,
	WhiteBoth		= 0b0011,
	BlackBoth		= 0b1100,
	All				= 0b1111
};

constexpr Castle operator& (Castle lhs, Castle rhs) noexcept
{
	return static_cast<Castle>(static_cast<std::uint32_t>(lhs) & static_cast<std::uint32_t>(rhs));
}

constexpr Castle operator^ (Castle lhs, Castle rhs) noexcept
{
	return static_cast<Castle>(static_cast<std::uint32_t>(lhs) ^ static_cast<std::uint32_t>(rhs));
}

constexpr Castle operator~ (Castle c) noexcept
{
	return static_cast<Castle>(~static_cast<std::uint32_t>(c));
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
