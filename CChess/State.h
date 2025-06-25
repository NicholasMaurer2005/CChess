#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <algorithm>

#include "ChessConstants.hpp"
#include "BitBoard.h"
#include "MoveList.h"



class cachealign State
{
private:

	BitBoard m_occupancy;
	BitBoard m_whiteOccupancy;
	BitBoard m_blackOccupancy;
	BitBoard m_whiteEnpassantSquare;
	BitBoard m_blackEnpassantSquare;

	std::array<BitBoard, pieceCount> m_pieceOccupancy;

	Castle m_castleRights;

	bool m_kingInCheck;



public:

	//construcors
	State() noexcept;



	//getters
	BitBoard occupancy() const noexcept;

	BitBoard whiteOccupancy() const noexcept;

	BitBoard blackOccupancy() const noexcept;

	BitBoard pieceOccupancy(Piece piece) const noexcept;

	BitBoard whiteEnpassantSquare() const noexcept;

	BitBoard blackEnpassantSquare() const noexcept;

	bool castleWhiteKingSide() const noexcept;

	bool castleWhiteQueenSide() const noexcept;

	bool castleBlackKingSide() const noexcept;

	bool castleBlackQueenSide() const noexcept;

	bool kingInCheck() const noexcept;

	template<Piece piece>
	BitBoard pieceOccupancyT() const noexcept
	{
		return m_pieceOccupancy[static_cast<std::size_t>(piece)];
	}

	template<bool white>
	Piece findPiece(int index) const noexcept
	{
		constexpr int begin{ white ? whitePieceOffset : blackPieceOffset };
		constexpr int end{ white ? blackPieceOffset : pieceCount };
		
		for (std::size_t i{ begin }; i < end; ++i)
		{
			if (m_pieceOccupancy[i].test(index))
			{
				return static_cast<Piece>(i);
			}
		}

		//this should never execute, test for occupancy before calling State::findPiece()
		return Piece::NoPiece;
	}
};

