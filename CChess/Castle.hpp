#pragma once

#include <cstdint>



enum class Castle : std::uint8_t 
{
	WhiteKingSide	= 0b00000001,
	WhiteQueenSide	= 0b00000010,
	BlackKingSide	= 0b00000100,
	BlackQueenSide	= 0b00001000,
	WhiteBoth		= 0b00000011,
	BlackBoth		= 0b00001100,
	All				= 0b00001111,
	None			= 0b00000000
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