#pragma once

#include <cstdint>

enum class Castle : std::uint8_t //if you change type rememember to change operators
{
	WhiteKingSide = 0b0001,
	WhiteQueenSide = 0b0010,
	BlackKingSide = 0b0100,
	BlackQueenSide = 0b1000,
	WhiteBoth = 0b0011,
	BlackBoth = 0b1100,
	All = 0b1111,
	None = 0b0000
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