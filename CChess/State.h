#pragma once

#include <array>
#include <cstdint>
#include <string_view>

#include "ChessConstants.hpp"
#include "BitBoard.h"
#include "StackString.hpp"
#include "Castle.hpp"
#include "Move.h"



class cachealign State
{
public:

	//	Public Definitions

	using FenPosition = StackString<128>;
	using CharPosition = StackString<boardSize + 1>;



private:

	//	Private Members

	BitBoard m_occupancy{};
	BitBoard m_whiteOccupancy{};
	BitBoard m_blackOccupancy{};
	BitBoard m_enpassantSquare{};
	BitBoard m_whiteSquares{};
	BitBoard m_blackSquares{};
	std::array<BitBoard, pieceCount> m_pieceOccupancy{};
	Castle m_castleRights{};



private:

	//	Private Methods

	//move
	void moveOccupancy(bool white, int sourceIndex, int destinationIndex) noexcept;

	void moveOccupancyCapture(bool white, int sourceIndex, int destinationIndex) noexcept;

	void movePiece(Piece piece, int sourceIndex, int destinationIndex) noexcept;

	void testCastleRights(bool white, Piece sourcePiece, int sourceIndex) noexcept;

	void testCastleCaptureRights(bool white, Piece sourcePiece, Piece attackPiece, int sourceIndex, int destinationIndex) noexcept;



	//normal moves
	void moveQuiet(bool white, Piece sourcePiece, int sourceIndex, int destinationIndex) noexcept;

	void moveCapture(bool white, Piece sourcePiece, Piece capturePiece, int sourceIndex, int destinationIndex) noexcept;



	//enpassant
	void moveOccupancyEnpassant(bool white, int sourceIndex, int destinationIndex, int enpassantIndex) noexcept;

	void moveEnpassant(bool white, Piece sourcePiece, Piece capturePiece, int sourceIndex, int destinationIndex, int enpassantIndex) noexcept;



	//castle
	void moveCastle(Castle castle) noexcept;

	void unmoveCastle(Castle castle) noexcept;



	//promote
	void moveQuietPromote(bool white, Piece sourcePiece, Piece promotePiece, int sourceIndex, int destinationIndex) noexcept;

	void moveCapturePromote(bool white, Piece sourcePiece, Piece attackPiece, Piece promotePiece, int sourceIndex, int destinationIndex) noexcept;



public:

	//	Public Methods

	//constructors
	State() noexcept = default;

	State(std::string_view fen, Castle castle);

	static State fromFen(std::string_view position);

	static State fromChar(std::string_view position);



	//getters
	FenPosition fenPosition() const noexcept;

	CharPosition charPosition() const noexcept;

	BitBoard occupancy() const noexcept;

	BitBoard whiteOccupancy() const noexcept;

	BitBoard blackOccupancy() const noexcept;

	BitBoard pieceOccupancy(Piece piece) const noexcept;

	BitBoard enpassantSquare() const noexcept;

	bool castleWhiteKingSide() const noexcept;

	bool castleWhiteQueenSide() const noexcept;

	bool castleBlackKingSide() const noexcept;

	bool castleBlackQueenSide() const noexcept;

	bool whiteKingInCheck() const noexcept;

	bool blackKingInCheck() const noexcept;

	BitBoard whiteSquares() const noexcept;

	BitBoard blackSquares() const noexcept;

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



	//setters
	void setWhiteSquares(BitBoard squares) noexcept;

	void setBlackSquares(BitBoard squares) noexcept;



	//move
	void makeMove(bool white, Move move) noexcept;



	//compare
	bool operator== (const State& other) const noexcept;



	//print
	void print() const noexcept;

	void dump() const noexcept;
};