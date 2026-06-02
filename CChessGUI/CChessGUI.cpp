#include "CChessGUI.h"

#include <CChess.h>
#include <string_view>
#include <algorithm>
#include <array>
#include <limits>
#include <format>

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
	using PieceArray = std::array<PieceSprite, boardSize>;

	PieceArray pieces{};
	PieceArray::iterator back{ pieces.begin() };

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

void CChessGUI::updatePosition() noexcept
{
	std::string_view position{ engine_get_position_char() };
	std::ranges::copy(position, m_position.begin());
	
	m_moveSource = 0;
	m_moveDestination = 0;
	engine_last_move(&m_moveSource, &m_moveDestination);
}

PieceSprite::Piece CChessGUI::pieceCallback(int square) noexcept
{
	square = *m_menuManager.flippedPtr() ? 63 - square : square;

	const PieceSprite::Piece piece{ charToPiece(m_position[square]) };
	m_position[square] = '.';
	bufferPosition();

	return piece;
}

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
				m_menuManager.setEvaluationString(std::format("{} - {}ply", evaluation, depth));
				m_menuManager.setPrincipalVariation(principalVariation);
			}

			int source{}, destination{};
			if (engine_best_move(&source, &destination))
			{
				engine_move(m_menuManager.whiteToMove(), source, destination);
				m_menuManager.setSearching(false);
				m_menuManager.flipColorToMove();
				m_menuManager.setEngineShouldRedraw();
			}
		}

		if (m_menuManager.engineShouldMove())
		{
			engine_start_search(m_menuManager.whiteToMove());
			m_menuManager.setSearching(true);
		}

		if (m_menuManager.engineShouldParsePlayerMove())
		{
			auto [source, destination] = m_menuManager.lastPlayerMove();

			if (engine_move(m_menuManager.whiteToMove(), source, destination)) m_menuManager.flipColorToMove();

			m_menuManager.setEngineShouldRedraw();
		}

		if (m_menuManager.engineShouldReset())
		{
			engine_set_position_start();

			m_menuManager.setEngineShouldRedraw();
		}

		if (m_menuManager.engineShouldMoveForward() && engine_move_forward())
		{
			m_menuManager.setEngineShouldRedraw();

			m_menuManager.flipColorToMove();
		}

		if (m_menuManager.engineShouldMoveBack() && engine_move_back())
		{
			m_menuManager.setEngineShouldRedraw();

			m_menuManager.flipColorToMove();
		}

		if (m_menuManager.engineShouldRedraw())
		{
			updatePosition();
			bufferPosition();
		}

		m_window.draw();
	}
}



//	Public Methods

//constructors
CChessGUI::CChessGUI()
	: m_menuManager([this](int square) { return pieceCallback(square); })
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