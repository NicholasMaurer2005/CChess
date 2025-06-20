#pragma once

#include <cstddef>
#include <cstdint>
#include <new>

constexpr int boardSize{ 64 };
constexpr int fileSize{ 8 };
constexpr int rankSize{ 8 };

#define cachealign alignas(std::hardware_destructive_interference_size)

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
};

constexpr int pieceCount{ 12 };