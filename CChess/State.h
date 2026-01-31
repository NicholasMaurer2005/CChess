#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <string_view>

#include "ChessConstants.hpp"
#include "BitBoard.h"
#include "MoveList.hpp"

enum class MoveType : std::uint8_t
{
	WhiteQuiet,
	WhiteCapture,
	WhiteCastle,
	WhitePromote,
	WhitePromoteCapture,
	WhiteEnpassant,
	BlackQuiet,
	BlackCapture,
	BlackCastle,
	BlackPromote,
	BlackPromoteCapture,
	BlackEnpassant
};

enum class SmallPiece : std::uint8_t
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

struct unmakeMoveInfo
{
	BitBoard whiteSquares;
	BitBoard blackSquares;
	std::uint8_t sourceIndex;
	std::uint8_t destinationIndex;
	SmallPiece sourcePiece;
	Castle castleRights;
	std::uint8_t enpassantIndex;
	MoveType type;
	SmallPiece capturePiece;
	std::uint8_t promoteOrCastle;
};



class cachealign State
{
private:

	BitBoard m_occupancy;
	BitBoard m_whiteOccupancy;
	BitBoard m_blackOccupancy;
	BitBoard m_enpassantSquare;
	BitBoard m_whiteSquares; 
	BitBoard m_blackSquares;

	std::array<BitBoard, pieceCount> m_pieceOccupancy;

	Castle m_castleRights;



	//private methods
	void moveOccupancy(bool white, int sourceIndex, int destinationIndex) noexcept;

	void moveOccupancyCapture(bool white, int sourceIndex, int destinationIndex) noexcept;

	void movePiece(Piece piece, int sourceIndex, int destinationIndex) noexcept;



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



	void testCastleRights(bool white, Piece sourcePiece, int sourceIndex) noexcept;

	void testCastleCaptureRights(bool white, Piece sourcePiece, Piece attackPiece, int sourceIndex, int destinationIndex) noexcept;



public:

	//constructors
	State() noexcept;

	State(std::string_view fen, Castle castle);

	State fromChar(std::string_view position);

	State fromFen(std::string_view position);



	//compare
	bool operator== (const State& other) const noexcept;


	//print
	void print() const noexcept;

	void dump() const noexcept;



	//move
	void makeMove(bool white, Move move) noexcept;

	// :(
	void unmakeMove(unmakeMoveInfo& info) noexcept;


	//setters
	void setWhiteSquares(BitBoard squares) noexcept;

	void setBlackSquares(BitBoard squares) noexcept;



	//getters
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
};

