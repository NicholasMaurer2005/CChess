#include "CChessGUI.h"

#include <CChess.h>
#include <string_view>
#include <algorithm>
#include <array>
#include <limits>

#include "PieceSprite.h"
#include <span>



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
			const PieceSprite::Piece piece{ charToPiece(m_position[*m_menuManager.flippedPtr() ? 63 - index : index]) };

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
void CChessGUI::drawPosition() noexcept
{
	std::string_view position{ engine_get_position_char() };
	std::ranges::copy(position, m_position.begin());
	
	m_moveSource = 0;
	m_moveDestination = 0;
	engine_last_move(&m_moveSource, &m_moveDestination);
}

void CChessGUI::makeMove(int source, int destination) noexcept
{
	engine_move(m_menuManager.whiteToMove(), source, destination);
	drawPosition();
	bufferPosition();
}



//	Private Members

void CChessGUI::play() noexcept
{
	while (m_window.open())
	{
		if (m_menuManager.searching())
		{
			CCHESS_BOOL done{};
			int evaluation{}, depth{};
			float nodesPerSecond{}, timeRemaining{};
			const char* principalVariation{};

			if (engine_search_info(&done, &evaluation, &depth, &nodesPerSecond, &timeRemaining, &principalVariation))
			{
				if (done)
				{
					int source{}, destination{};
					engine_best_move(&source, &destination);

					makeMove(source, destination);

					drawPosition();
				}
			}
		}

		if (m_menuManager.engineShouldMove())
		{
			m_menuManager.setSearching(true);
			engine_start_search(m_menuManager.whiteToMove())
		}

		if (m_menuManager.engineShouldParsePlayerMove())
		{

		}

		m_window.draw();
	}
}

CChessGUI::CChessGUI()
{
	engine_create();

	drawPosition();
	bufferPosition();

	play();
}

CChessGUI::~CChessGUI()
{
	engine_destroy();
}