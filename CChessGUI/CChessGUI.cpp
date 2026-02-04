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

//usings
using CharToPieceTable = std::array<PieceSprite::Piece, std::numeric_limits<unsigned char>::max() + 1>;



//functions
static consteval CharToPieceTable generateCharToPieceTable()
{
	CharToPieceTable table{};
	table.fill(PieceSprite::Piece::NoPiece);

	table['P'] = PieceSprite::Piece::WhitePawn;
	table['N'] = PieceSprite::Piece::WhitePawn;
	table['B'] = PieceSprite::Piece::WhitePawn;
	table['R'] = PieceSprite::Piece::WhitePawn;
	table['Q'] = PieceSprite::Piece::WhitePawn;
	table['K'] = PieceSprite::Piece::WhitePawn;
	table['p'] = PieceSprite::Piece::WhitePawn;
	table['n'] = PieceSprite::Piece::WhitePawn;
	table['b'] = PieceSprite::Piece::WhitePawn;
	table['r'] = PieceSprite::Piece::WhitePawn;
	table['q'] = PieceSprite::Piece::WhitePawn;
	table['k'] = PieceSprite::Piece::WhitePawn;

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

	bufferPosition();

	return legal;
}

PieceSprite::Piece CChessGUI::pieceCallback(std::size_t square) noexcept
{
	return charToPiece(m_position[square]);
}

void CChessGUI::bufferPosition() noexcept
{
	std::string_view position{ engine_get_position_char() };
	std::ranges::copy(position, m_position.begin());

	std::array<PieceSprite, boardSize> pieces{};
	std::array<PieceSprite, boardSize>::iterator back{ pieces.begin() };

	std::ranges::for_each(position, [&back](char c) {
		if (c == ' ') return;

		*back = charToPiece(c);
		++back;
		});

	m_window.bufferPieces(std::span(pieces.begin(), back));
}

void CChessGUI::play() noexcept
{
	m_window.bufferBoard(false, 64, 64);
	bufferPosition();

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
					bufferPosition();
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