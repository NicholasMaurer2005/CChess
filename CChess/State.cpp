#include "State.h"

#include <cctype>
#include <iostream>
#include <array>
#include <span>
#include <stdexcept>
#include <algorithm>

#include "Move.h"
#include "ChessConstants.hpp"
#include "BitBoard.h"



// Static Helpers

static consteval std::array<Piece, 255>  generateCharToPiece()
{
	std::array<Piece, 255> table{};

	table['P'] = Piece::WhitePawn;
	table['N'] = Piece::WhiteKnight;
	table['B'] = Piece::WhiteBishop;
	table['R'] = Piece::WhiteRook;
	table['Q'] = Piece::WhiteQueen;
	table['K'] = Piece::WhiteKing;
	table['p'] = Piece::BlackPawn;
	table['n'] = Piece::BlackKnight;
	table['b'] = Piece::BlackBishop;
	table['r'] = Piece::BlackRook;
	table['q'] = Piece::BlackQueen;
	table['k'] = Piece::BlackKing;

	return table;
}

static char pieceToChar(Piece piece)
{
	static constexpr std::array<char, pieceCount> table{ '.', 'P','N','B','R','Q','K','p','n','b','r','q','k' };

	return table[static_cast<std::size_t>(piece)];
}

static constexpr std::array<Piece, 255> charToPiece{ generateCharToPiece() };


//	Private Methods

//move
void State::moveOccupancy(bool white, int sourceIndex, int destinationIndex) noexcept
{
	m_occupancy.reset(sourceIndex);
	m_occupancy.set(destinationIndex);

	if (white)
	{
		m_whiteOccupancy.reset(sourceIndex);
		m_whiteOccupancy.set(destinationIndex);
	}
	else
	{
		m_blackOccupancy.reset(sourceIndex);
		m_blackOccupancy.set(destinationIndex);
	}
}

void State::moveOccupancyCapture(bool white, int sourceIndex, int destinationIndex) noexcept
{
	m_occupancy.reset(sourceIndex);

	if (white)
	{
		m_whiteOccupancy.reset(sourceIndex);
		m_whiteOccupancy.set(destinationIndex);
		m_blackOccupancy.reset(destinationIndex);
	}
	else
	{
		m_blackOccupancy.reset(sourceIndex);
		m_blackOccupancy.set(destinationIndex);
		m_whiteOccupancy.reset(destinationIndex);
	}
}

void State::movePiece(Piece piece, int sourceIndex, int destinationIndex) noexcept
{
	m_pieceOccupancy[static_cast<std::size_t>(piece)].reset(sourceIndex);
	m_pieceOccupancy[static_cast<std::size_t>(piece)].set(destinationIndex);
}

void State::testCastleRights(bool white, Piece sourcePiece, int sourceIndex) noexcept
{
	if (white)
	{
		if (!(static_cast<bool>(m_castleRights & Castle::WhiteBoth))) return;

		if (sourcePiece == Piece::WhiteRook)
		{
			if (sourceIndex == 7)
			{
				m_castleRights &= ~Castle::WhiteKingSide;
			}
			else if (sourceIndex == 0)
			{
				m_castleRights &= ~Castle::WhiteQueenSide;
			}
		}
		else if (sourcePiece == Piece::WhiteKing)
		{
			m_castleRights &= ~Castle::WhiteBoth;
		}
	}
	else
	{
		if (!(static_cast<bool>(m_castleRights & Castle::BlackBoth))) return;

		if (sourcePiece == Piece::BlackRook)
		{
			if (sourceIndex == 63)
			{
				m_castleRights &= ~Castle::BlackKingSide;
			}
			else if (sourceIndex == 56)
			{
				m_castleRights &= ~Castle::BlackQueenSide;
			}
		}
		else if (sourcePiece == Piece::BlackKing)
		{
			m_castleRights &= ~Castle::BlackBoth;
		}
	}
}

void State::testCastleCaptureRights(bool white, Piece sourcePiece, Piece attackPiece, int sourceIndex, int destinationIndex) noexcept
{
	if (white) //TODO: maybe find faster way?
	{
		if (sourcePiece == Piece::WhiteRook)
		{
			if (sourceIndex == h1)
			{
				m_castleRights &= ~Castle::WhiteKingSide;
			}
			else if (sourceIndex == a1)
			{
				m_castleRights &= ~Castle::WhiteQueenSide;
			}
		}
		else if (sourcePiece == Piece::WhiteKing)
		{
			m_castleRights &= ~Castle::WhiteBoth;
		}

		if (attackPiece == Piece::BlackRook)
		{
			if (destinationIndex == h8)
			{
				m_castleRights &= ~Castle::BlackKingSide;
			}
			else if (destinationIndex == a8)
			{
				m_castleRights &= ~Castle::BlackQueenSide;
			}
		}
	}
	else
	{
		if (sourcePiece == Piece::BlackRook)
		{
			if (sourceIndex == h8)
			{
				m_castleRights &= ~Castle::BlackKingSide;
			}
			else if (sourceIndex == a8)
			{
				m_castleRights &= ~Castle::BlackQueenSide;
			}
		}
		else if (sourcePiece == Piece::BlackKing)
		{
			m_castleRights &= ~Castle::BlackBoth;
		}

		if (attackPiece == Piece::WhiteRook)
		{
			if (destinationIndex == h1)
			{
				m_castleRights &= ~Castle::WhiteKingSide;
			}
			else if (destinationIndex == a1)
			{
				m_castleRights &= ~Castle::WhiteQueenSide;
			}
		}
	}
}



//normal moves
void State::moveQuiet(bool white, Piece sourcePiece, int sourceIndex, int destinationIndex) noexcept
{
	moveOccupancy(white, sourceIndex, destinationIndex);
	movePiece(sourcePiece, sourceIndex, destinationIndex);
}

void State::moveCapture(bool white, Piece sourcePiece, Piece capturePiece, int sourceIndex, int destinationIndex) noexcept
{
	moveOccupancyCapture(white, sourceIndex, destinationIndex);
	movePiece(sourcePiece, sourceIndex, destinationIndex);
	m_pieceOccupancy[static_cast<std::size_t>(capturePiece)].reset(destinationIndex);
}



//enpassant
void State::moveOccupancyEnpassant(bool white, int sourceIndex, int destinationIndex, int enpassantIndex) noexcept
{
	m_occupancy.reset(sourceIndex);
	m_occupancy.set(destinationIndex);
	m_occupancy.reset(enpassantIndex);

	if (white)
	{
		m_whiteOccupancy.reset(sourceIndex);
		m_whiteOccupancy.set(destinationIndex);
		m_blackOccupancy.reset(enpassantIndex);
	}
	else
	{
		m_blackOccupancy.reset(sourceIndex);
		m_blackOccupancy.set(destinationIndex);
		m_whiteOccupancy.reset(enpassantIndex);
	}
}

void State::moveEnpassant(bool white, Piece sourcePiece, Piece capturePiece, int sourceIndex, int destinationIndex, int enpassantIndex) noexcept
{
	moveOccupancyEnpassant(white, sourceIndex, destinationIndex, enpassantIndex);
	movePiece(sourcePiece, sourceIndex, destinationIndex);

	m_pieceOccupancy[static_cast<std::size_t>(capturePiece)].reset(enpassantIndex);
}



//castle
void State::moveCastle(Castle castle) noexcept
{
	switch (castle)
	{
	case Castle::WhiteKingSide:
		moveQuiet(true, Piece::WhiteKing, e1, g1);
		moveQuiet(true, Piece::WhiteRook, h1, f1);
		m_castleRights &= ~Castle::WhiteBoth;
		break;

	case Castle::WhiteQueenSide:
		moveQuiet(true, Piece::WhiteKing, e1, c1);
		moveQuiet(true, Piece::WhiteRook, a1, d1);
		m_castleRights &= ~Castle::WhiteBoth;
		break;

	case Castle::BlackKingSide:
		moveQuiet(false, Piece::BlackKing, e8, g8);
		moveQuiet(false, Piece::BlackRook, h8, f8);
		m_castleRights &= ~Castle::BlackBoth;
		break;

	case Castle::BlackQueenSide:
		moveQuiet(false, Piece::BlackKing, e8, c8);
		moveQuiet(false, Piece::BlackRook, a8, d8);
		m_castleRights &= ~Castle::BlackBoth;
		break;
	}
}

void State::unmoveCastle(Castle castle) noexcept
{
	switch (castle)
	{
	case Castle::WhiteKingSide:
		moveQuiet(true, Piece::WhiteKing, g1, e1);
		moveQuiet(true, Piece::WhiteRook, f1, h1);
		break;

	case Castle::WhiteQueenSide:
		moveQuiet(true, Piece::WhiteKing, c1, e1);
		moveQuiet(true, Piece::WhiteRook, d1, a1);
		break;

	case Castle::BlackKingSide:
		moveQuiet(false, Piece::BlackKing, g8, e8);
		moveQuiet(false, Piece::BlackRook, f8, h8);
		break;

	case Castle::BlackQueenSide:
		moveQuiet(false, Piece::BlackKing, c8, e8);
		moveQuiet(false, Piece::BlackRook, d8, a8);
		break;
	}
}



//promote
void State::moveQuietPromote(bool white, Piece sourcePiece, Piece promotePiece, int sourceIndex, int destinationIndex) noexcept
{
	moveOccupancy(white, sourceIndex, destinationIndex);

	m_pieceOccupancy[static_cast<std::size_t>(sourcePiece)].reset(sourceIndex);
	m_pieceOccupancy[static_cast<std::size_t>(promotePiece)].set(destinationIndex);
}

void State::moveCapturePromote(bool white, Piece sourcePiece, Piece attackPiece, Piece promotePiece, int sourceIndex, int destinationIndex) noexcept
{
	testCastleCaptureRights(white, sourcePiece, attackPiece, sourceIndex, destinationIndex);
	moveOccupancyCapture(white, sourceIndex, destinationIndex);

	m_pieceOccupancy[static_cast<std::size_t>(sourcePiece)].reset(sourceIndex);
	m_pieceOccupancy[static_cast<std::size_t>(attackPiece)].reset(destinationIndex);
	m_pieceOccupancy[static_cast<std::size_t>(promotePiece)].set(destinationIndex);
}



//	Public Methods

//constructors
State::State(std::string_view fen, Castle castle)
	: m_castleRights(castle)
{ 
	std::size_t coreEndIndex{ fen.find(' ') };
	std::string_view coreFen{ fen.substr(0, coreEndIndex) };
	
	const bool whiteToMove{ fen[coreEndIndex + 1] == 'w' };


	for (std::size_t i{}; i < rankSize; ++i)
	{
		const std::size_t slashIndex{ coreFen.find('/') };
		const std::string_view rank{ coreFen.substr(0, slashIndex) };
		
		std::size_t boardIndex = (7 - i) * 8;

		for (char c : rank)
		{
			if (std::isdigit(c))
			{
				std::size_t digit{ static_cast<std::size_t>(c - '0') };
				boardIndex += digit;
			}
			else
			{
				const Piece piece{ charToPiece[c] };
				const bool white = static_cast<std::uint32_t>(piece) < static_cast<std::uint32_t>(blackPieceOffset);

				m_occupancy.set(static_cast<int>(boardIndex));
				m_pieceOccupancy[static_cast<std::size_t>(piece)].set(static_cast<int>(boardIndex));

				if (white)
				{
					m_whiteOccupancy.set(static_cast<int>(boardIndex));
				}
				else
				{
					m_blackOccupancy.set(static_cast<int>(boardIndex));
				}

				++boardIndex;
			}
		}

		coreFen = coreFen.substr(slashIndex + 1);
	}
}

State State::fromFen(std::string_view position)
{
	throw std::logic_error("Method 'State::fromFen' is not implimented!");
}

State State::fromChar(std::string_view position)
{
	throw std::logic_error("Method 'State::fromChar' is not implimented!");
}



//getters
State::FenPosition State::fenPosition() const noexcept
{
	throw std::logic_error("Method 'State::fenPosition()' is not implimented!");
}

State::CharPosition State::charPosition() const noexcept
{	
	CharPosition position;
	position.resize();

	for (int i{}; i < boardSize; ++i)
	{
		const Piece whitePiece{ findPiece<true>(i) };
		const Piece blackPiece{ findPiece<false>(i) };
		const Piece piece{ whitePiece != Piece::NoPiece ? whitePiece : blackPiece };

		position[i] = pieceToChar(piece);
	}

	return position;
}

BitBoard State::occupancy() const noexcept
{
	return m_occupancy;
}

BitBoard State::whiteOccupancy() const noexcept
{
	return m_whiteOccupancy;
}

BitBoard State::blackOccupancy() const noexcept
{
	return m_blackOccupancy;
}

BitBoard State::pieceOccupancy(Piece piece) const noexcept
{
	return m_pieceOccupancy[static_cast<std::size_t>(piece)];
}

BitBoard State::enpassantSquare() const noexcept
{
	return m_enpassantSquare;
}

bool State::castleWhiteKingSide() const noexcept
{
	return static_cast<bool>(m_castleRights & Castle::WhiteKingSide);
}

bool State::castleWhiteQueenSide() const noexcept
{
	return static_cast<bool>(m_castleRights & Castle::WhiteQueenSide);
}

bool State::castleBlackKingSide() const noexcept
{
	return static_cast<bool>(m_castleRights & Castle::BlackKingSide);
}

bool State::castleBlackQueenSide() const noexcept
{
	return static_cast<bool>(m_castleRights & Castle::BlackQueenSide);
}

bool State::whiteKingInCheck() const noexcept
{
	return m_blackSquares.board() & pieceOccupancyT<Piece::WhiteKing>().board();
}

bool State::blackKingInCheck() const noexcept
{
	return m_whiteSquares.board() & pieceOccupancyT<Piece::BlackKing>().board();
}

BitBoard State::whiteSquares() const noexcept
{
	return m_whiteSquares;
}

BitBoard State::blackSquares() const noexcept
{
	return m_blackSquares;
}



//setters
void State::setWhiteSquares(BitBoard squares) noexcept
{
	m_whiteSquares = squares;
}

void State::setBlackSquares(BitBoard squares) noexcept
{
	m_blackSquares = squares;
}



//move 
void State::makeMove(bool white, Move move) noexcept //TODO: could I make this a switch? GGGGRRRRAAATATATATATATA
{
	const int sourceIndex{ move.sourceIndex() };
	const int destinationIndex{ move.destinationIndex() };
	const Piece sourcePiece{ move.sourcePiece() };

	m_enpassantSquare = BitBoard();

	if (move.castleFlag()) [[unlikely]]
	{
		//castle
		const Castle castleType{ move.castleType() };
		moveCastle(castleType);
	}
	else if (move.enpassantFlag()) [[unlikely]]
	{
		//enpassant
		const int enpassantIndex{ move.enpassantIndex() + (white ? 32 : 24) };
		moveEnpassant(white, sourcePiece, move.attackPiece(), sourceIndex, destinationIndex, enpassantIndex);
	}
	else
	{
		const Piece promotePiece{ move.promotePiece() };
		const Piece capturePiece{ move.attackPiece() };

		if (promotePiece == Piece::NoPiece) [[likely]]
		{
			if (capturePiece == Piece::NoPiece)
			{
				if (move.doublePawnFlag()) [[unlikely]]
				{
					//double pawn push
					const int enpassantIndex{ sourceIndex + (white ? 8 : -8) };
					m_enpassantSquare = BitBoard(1ULL << enpassantIndex);

					moveQuiet(white, sourcePiece, sourceIndex, destinationIndex);
					testCastleRights(white, sourcePiece, sourceIndex); //TODO: refactor so only one branch happens
				}
				else
				{
					//normal quiet
					moveQuiet(white, sourcePiece, sourceIndex, destinationIndex);
					testCastleRights(white, sourcePiece, sourceIndex); //TODO: refactor so only one branch happens
				}
			}
			else
			{
				//normal capture
				moveCapture(white, sourcePiece, capturePiece, sourceIndex, destinationIndex);
				testCastleCaptureRights(white, sourcePiece, capturePiece, sourceIndex, destinationIndex);
			}
		}
		else
		{
			if (capturePiece == Piece::NoPiece)
			{
				//quiet promote
				moveQuietPromote(white, move.sourcePiece(), promotePiece, move.sourceIndex(), move.destinationIndex());
			}
			else
			{
				//capture promote
				moveCapturePromote(white, move.sourcePiece(), capturePiece, promotePiece, move.sourceIndex(), move.destinationIndex());
			}
		}
	}
}



//compare
bool State::operator==(const State& other) const noexcept
{
	return m_occupancy == other.m_occupancy &&
		m_whiteOccupancy == other.m_whiteOccupancy &&
		m_blackOccupancy == other.m_blackOccupancy &&
		m_enpassantSquare == other.m_enpassantSquare &&
		m_whiteSquares == other.m_whiteSquares &&
		m_blackSquares == other.m_blackSquares &&
		m_pieceOccupancy == other.m_pieceOccupancy &&
		m_castleRights == other.m_castleRights;
}



//print
void State::print() const noexcept
{
	constexpr std::array<char, pieceCount> pieceToChar{ '.', 'P', 'N', 'B', 'R', 'Q', 'K', 'X', 'n', 'b', 'r', 'q', 'k' };

	std::array<Piece, boardSize> board{};

	for (std::uint32_t i{ whitePieceOffset }; i < pieceCount; ++i)
	{
		BitBoard occupancy{ m_pieceOccupancy[i] };

		while (occupancy.board())
		{
			const int index{ occupancy.popLeastSignificantBit() };
			board[index] = static_cast<Piece>(i);
		}
	}

	const int enpassantIndex = m_enpassantSquare.board() ? m_enpassantSquare.leastSignificantBit() : 64;

	for (int rank{ rankSize - 1 }; rank >= 0; --rank)
	{
		std::cout << (rank + 1) << "  ";

		for (int file{}; file < fileSize; ++file)
		{
			const std::size_t index{ static_cast<std::size_t>(rank * fileSize + file) };

			if (index == enpassantIndex)
			{
				std::cout << "e ";
			}
			else
			{
				std::cout << pieceToChar[static_cast<std::size_t>(board[index])] << ' ';
			}
		}

		std::cout << "\n";
	}

	std::cout << "\n   A B C D E F G H\n";
}

void State::dump() const noexcept
{
	std::cout << "occupancy\n";
	m_occupancy.print();

	std::cout << "\n\nwhite occupancy\n";
	m_whiteOccupancy.print();

	std::cout << "\n\nblack occupancy\n";
	m_blackOccupancy.print();

	std::cout << "\n\nenpassant square\n";
	m_enpassantSquare.print();

	std::cout << "\n\nwhite squares\n";
	m_whiteSquares.print();

	std::cout << "\n\nblack squares\n";
	m_blackSquares.print();

	std::cout << "\n\ncastleRights\n";
	std::cout << static_cast<int>(m_castleRights);

	std::cout << "\n\nwhite pawn occupancy\n";
	pieceOccupancyT<Piece::WhitePawn>().print();

	std::cout << "\n\nwhite knight occupancy\n";
	pieceOccupancyT<Piece::WhiteKnight>().print();

	std::cout << "\n\nwhite bishop occupancy\n";
	pieceOccupancyT<Piece::WhiteBishop>().print();

	std::cout << "\n\nwhite rook occupancy\n";
	pieceOccupancyT<Piece::WhiteRook>().print();

	std::cout << "\n\nwhite queen occupancy\n";
	pieceOccupancyT<Piece::WhiteQueen>().print();

	std::cout << "\n\nwhite king occupancy\n";
	pieceOccupancyT<Piece::WhiteKing>().print();

	std::cout << "\n\nblack pawn occupancy\n";
	pieceOccupancyT<Piece::BlackPawn>().print();

	std::cout << "\n\nblack knight occupancy\n";
	pieceOccupancyT<Piece::BlackKnight>().print();

	std::cout << "\n\nblack bishop occupancy\n";
	pieceOccupancyT<Piece::BlackBishop>().print();

	std::cout << "\n\nblack rook occupancy\n";
	pieceOccupancyT<Piece::BlackRook>().print();

	std::cout << "\n\nblack queen occupancy\n";
	pieceOccupancyT<Piece::BlackQueen>().print();

	std::cout << "\n\nblack king occupancy\n";
	pieceOccupancyT<Piece::BlackKing>().print();
	std::cout << '\n';
}