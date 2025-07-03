#include "State.h"

#include <cctype>
#include <iostream>


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



//constructors
State::State() noexcept
	//occupancy
	: m_occupancy(), m_whiteOccupancy(), m_blackOccupancy(), m_pieceOccupancy(),

	//enpassant
	m_enpassantSquare(),

	//squares
	m_whiteSquares(), m_blackSquares(),

	m_castleRights(), m_whiteKingInCheck(), m_blackKingInCheck() { }


State::State(std::string_view fen, Castle castle) //TODO: check for valid fens/make sure it never "fails"
	//occupancy
	: m_occupancy(), m_whiteOccupancy(), m_blackOccupancy(), m_pieceOccupancy(),

	//enpassant
	m_enpassantSquare(),

	//squares
	m_whiteSquares(), m_blackSquares(),

	m_castleRights(castle), m_whiteKingInCheck(), m_blackKingInCheck()
{ 
	constexpr std::array<Piece, 255> charToPiece{ generateCharToPiece() };



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



//private methods //TODO: maybe refactor? | consistant nameing PLEASE
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

void State::movePiece(Piece piece, int sourceIndex, int destinationIndex) noexcept
{
	m_pieceOccupancy[static_cast<std::size_t>(piece)].reset(sourceIndex);
	m_pieceOccupancy[static_cast<std::size_t>(piece)].set(destinationIndex);
}

void State::moveQuiet(bool white, Piece sourcePiece, int sourceIndex, int destinationIndex) noexcept
{
	testCastleRights(white, sourcePiece, sourceIndex); //TODO: refactor so only one branch happens

	moveOccupancy(white, sourceIndex, destinationIndex);
	movePiece(sourcePiece, sourceIndex, destinationIndex);
}

void State::moveCapture(bool white, Piece sourcePiece, Piece capturePiece, int sourceIndex, int destinationIndex) noexcept
{
	testCastleCaptureRights(white, sourcePiece, capturePiece, sourceIndex, destinationIndex);

	moveOccupancyCapture(white, sourceIndex, destinationIndex);
	movePiece(sourcePiece, sourceIndex, destinationIndex);
	m_pieceOccupancy[static_cast<std::size_t>(capturePiece)].reset(destinationIndex);
}

void State::moveEnpassant(bool white, Piece sourcePiece, Piece capturePiece, int sourceIndex, int destinationIndex, int enpassantIndex) noexcept
{
	moveOccupancyEnpassant(white, sourceIndex, destinationIndex, enpassantIndex);
	movePiece(sourcePiece, sourceIndex, destinationIndex);

	m_pieceOccupancy[static_cast<std::size_t>(capturePiece)].reset(enpassantIndex);
}

void State::moveCastle(Castle castle) noexcept
{
	switch (castle)
	{
	case Castle::WhiteKingSide:
		moveQuiet(true, Piece::WhiteKing, 4, 6);
		moveQuiet(true, Piece::WhiteRook, 7, 5);
		m_castleRights &= ~Castle::WhiteBoth;
		break;

	case Castle::WhiteQueenSide:
		moveQuiet(true, Piece::WhiteKing, 4, 2);
		moveQuiet(true, Piece::WhiteRook, 0, 3);
		m_castleRights &= ~Castle::WhiteBoth;
		break;

	case Castle::BlackKingSide:
		moveQuiet(false, Piece::BlackKing, 60, 62);
		moveQuiet(false, Piece::BlackRook, 63, 61);
		m_castleRights &= ~Castle::BlackBoth;
		break;

	case Castle::BlackQueenSide:
		moveQuiet(false, Piece::BlackKing, 60, 58);
		moveQuiet(false, Piece::BlackRook, 56, 59);
		m_castleRights &= ~Castle::BlackBoth;
		break;
	}
}

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
unmakeMoveInfo State::makeMove(bool white, Move move) noexcept
{
	unmakeMoveInfo info{ m_enpassantSquare, m_castleRights, m_whiteKingInCheck, m_blackKingInCheck };

	m_enpassantSquare = BitBoard();

	if (move.castleFlag()) [[unlikely]]
	{
		//castle
		moveCastle(move.castleType());
	}
	else if (move.enpassantFlag()) [[unlikely]]
	{
		//enpassant
		const int enpassantIndex{ move.enpassantIndex() + (white ? 32 : 24) };
		moveEnpassant(white, move.sourcePiece(), move.attackPiece(), move.sourceIndex(), move.destinationIndex(), enpassantIndex);
	}
	else
	{
		const Piece promotePiece{ move.promotePiece() };
		const Piece attackPiece{ move.attackPiece() };

		if (promotePiece == Piece::NoPiece) [[likely]]
		{
			if (attackPiece == Piece::NoPiece)
			{
				if (move.doublePawnFlag()) [[unlikely]]
				{
					//double pawn push
					const int sourceIndex{ move.sourceIndex() };
					const int enpassantIndex{ sourceIndex + (white ? 8 : -8) };

					m_enpassantSquare = BitBoard(1ULL << enpassantIndex);
					moveQuiet(white, move.sourcePiece(), sourceIndex, move.destinationIndex());
				}
				else
				{
					//normal quiet
					moveQuiet(white, move.sourcePiece(), move.sourceIndex(), move.destinationIndex());
				}
			}
			else
			{
				//normal capture
				moveCapture(white, move.sourcePiece(), move.attackPiece(), move.sourceIndex(), move.destinationIndex());
			}
		}
		else
		{
			if (attackPiece == Piece::NoPiece)
			{
				//quiet promote
				moveQuietPromote(white, move.sourcePiece(), promotePiece, move.sourceIndex(), move.destinationIndex());
			}
			else
			{
				//capture promote
				moveCapturePromote(white, move.sourcePiece(), attackPiece, promotePiece, move.sourceIndex(), move.destinationIndex());
			}
		}
	}

	return info;
}

void State::unmakeMove(bool white, Move move, unmakeMoveInfo info) noexcept
{

}



//getters
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

bool State::whiteKingInCheck() noexcept
{
	m_whiteKingInCheck = m_blackSquares.board() & pieceOccupancyT<Piece::WhiteKing>().board();
	return m_whiteKingInCheck;
}

bool State::blackKingInCheck() noexcept
{
	m_blackKingInCheck = m_whiteSquares.board() & pieceOccupancyT<Piece::BlackKing>().board();
	return m_blackKingInCheck;
}

BitBoard State::whiteSquares() const noexcept
{
	return m_whiteSquares;
}

BitBoard State::blackSquares() const noexcept
{
	return m_blackSquares;
}