#include "CChessGUI.h"

#include <CChess.h>
#include <array>
#include <limits>
#include <algorithm>
#include <string_view>
#include <span>
#include <iostream>
#include <format>

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

bool CChessGUI::moveCallback(int source, int destination) noexcept
{
	const bool legal{ engine_move(source, destination) == CCHESS_TRUE };
	
	if (legal)
	{
		m_whiteToMove = false;
	}

	bufferNewPosition();

	return legal;
}

PieceSprite::Piece CChessGUI::pieceCallback(std::size_t square) noexcept
{
	const PieceSprite::Piece piece{ charToPiece(m_position[square]) };
	m_position[square] = '.';
	bufferCurrentPosition();
	
	return piece;
}

void CChessGUI::bufferPosition(std::span<const char> position) noexcept
{
	std::array<PieceSprite, boardSize> pieces{};
	std::array<PieceSprite, boardSize>::iterator back{ pieces.begin() };

	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			const PieceSprite::Piece piece{ charToPiece(m_position[static_cast<std::size_t>(rank) * fileSize + file]) };
			if (piece == PieceSprite::Piece::NoPiece) continue;

			*back = PieceSprite(rank, file, piece);
			++back;
		}
	}

	m_window.bufferPieces(std::span(pieces.begin(), back));
}

void CChessGUI::bufferNewPosition() noexcept
{
	std::string_view position{ engine_get_position_char() };
	std::ranges::copy(position, m_position.begin());

	bufferPosition(position);
}

void CChessGUI::bufferCurrentPosition() noexcept
{
	bufferPosition(m_position);
}

void CChessGUI::play() noexcept
{
	m_window.bufferBoard(false, 64, 64);
	bufferNewPosition();

	while (m_window.open())
	{
		if (!m_whiteToMove)
		{
			if (!m_searching)
			{
				m_searching = true;
				engine_start_search();
			}
			else 
			{
				int done{};
				int evaluation{};
				int depth{};
				float nodesPerSecond{};
				float timeRemaining{};
				const char* pv{ "no pv" };
				if (engine_search_info(&done, &evaluation, &depth, &nodesPerSecond, &timeRemaining, &pv))
				{
					std::cout << std::format("Depth: {} ply  |  Evaluation: {}  |  Time Remaining: {:.3f} ms  |  {:.5f} kNodes/s\nPV: {}\n\n", depth, evaluation, timeRemaining, nodesPerSecond * 0.001f, pv);
				}

				int source{};
				int destination{};

				if (engine_best_move(&source, &destination))
				{
					m_searching = false;
					m_whiteToMove = true;
					engine_move_unchecked(source, destination);
					m_window.bufferBoard(false, source, destination);
					bufferNewPosition();
				}
			}
		}

		m_window.draw();
	}
}



//	Public Methods

//constructor
CChessGUI::CChessGUI()
{
	if (engine_create() == CCHESS_FALSE) throw std::runtime_error("unable to create engine");

	play();
}

CChessGUI::~CChessGUI() noexcept
{
	engine_destroy();
}