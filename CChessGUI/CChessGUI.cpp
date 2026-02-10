#include "CChessGUI.h"

#include <CChess.h>
#include <string_view>
#include <algorithm>
#include <array>
#include <limits>

#include "PieceSprite.h"



//	Static Helpers

//constants
static constexpr int rankSize{ 8 };
static constexpr int fileSize{ 8 };

//usings
using CharToPieceTable = std::array<PieceSprite::Piece, std::numeric_limits<unsigned char>::max() + 1>;



//functions
static consteval CharToPieceTable generateCharToPieceTable()
{
	CharToPieceTable table{};
	table.fill(PieceSprite::Piece::NoPiece);

	table['P'] = PieceSprite::Piece::WhitePawn;
	table['N'] = PieceSprite::Piece::WhiteKnight;
	table['B'] = PieceSprite::Piece::WhiteBishop;
	table['R'] = PieceSprite::Piece::WhiteRook;
	table['Q'] = PieceSprite::Piece::WhiteQueen;
	table['K'] = PieceSprite::Piece::WhiteKing;
	table['p'] = PieceSprite::Piece::BlackPawn;
	table['n'] = PieceSprite::Piece::BlackKnight;
	table['b'] = PieceSprite::Piece::BlackBishop;
	table['r'] = PieceSprite::Piece::BlackRook;
	table['q'] = PieceSprite::Piece::BlackQueen;
	table['k'] = PieceSprite::Piece::BlackKing;

	return table;
}

static PieceSprite::Piece charToPiece(char c) noexcept
{
	static constexpr CharToPieceTable charToPieceTable{ generateCharToPieceTable() };

	return charToPieceTable[c];
}



//	Private Methods

void CChessGUI::bufferPosition() noexcept
{
	std::array<PieceSprite, boardSize> pieces{};
	std::array<PieceSprite, boardSize>::iterator back{ pieces.begin() };

	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			const std::size_t index{ static_cast<std::size_t>(rank) * fileSize + file };
			const PieceSprite::Piece piece{ charToPiece(m_position[*m_menuManager.flippedPtr() ? 63 - index : index])};
			if (piece == PieceSprite::Piece::NoPiece) continue;

			*back = PieceSprite(rank, file, piece);
			++back;
		}
	}

	m_window.bufferPieces(std::span(pieces.begin(), back));

	if (m_moveSource != m_moveDestination)
	{
		m_window.bufferBoard(m_moveSource, m_moveDestination);
	}
	else
	{
		m_window.bufferBoard();
	}
}



//callbacks
void CChessGUI::moveCallback(int source, int destination) noexcept
{
	if (*m_menuManager.flippedPtr())
	{
		source = 63 - source;
		destination = 63 - destination;
	}

	makeMove(source, destination);
}

PieceSprite::Piece CChessGUI::pieceCallback(int square) noexcept
{
	square = *m_menuManager.flippedPtr() ? 63 - square : square;

	const PieceSprite::Piece piece{ charToPiece(m_position[square]) };
	m_position[square] = '.';
	bufferPosition();

	return piece;
}

void CChessGUI::updatePosition() noexcept
{
	std::string_view position{ engine_get_position_char() };
	std::ranges::copy(position, m_position.begin());
	
	m_moveSource = 0;
	m_moveDestination = 0;
	engine_last_move(&m_moveSource, &m_moveDestination);
}

void CChessGUI::makeMove(int source, int destination) noexcept
{
	engine_move(*m_menuManager.flippedPtr(), source, destination);
	updatePosition();
	bufferPosition();
}


//	Private Members

void CChessGUI::play() noexcept
{
	while (m_window.open())
	{
		if (m_menuManager.engineShouldRedraw())
		{
			updatePosition();
			bufferPosition();
			m_menuManager.engineJustRedrew();
		}

		if (m_menuManager.engineShouldMove())
		{
			if (m_menuManager.searching())
			{
				
			}
			else
			{
				engine_start_search(m_menuManager.whiteToMove());
				m_menuManager.setSearching(true);
			}
		}

		m_window.draw();
	}
}

CChessGUI::CChessGUI()
{
	engine_create();

	updatePosition();
	bufferPosition();

	play();
}

CChessGUI::~CChessGUI()
{
	engine_destroy();
}