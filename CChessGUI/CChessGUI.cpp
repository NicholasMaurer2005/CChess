#include "CChessGUI.h"

#include <CChess.h>
#include <array>
#include <limits>
#include <algorithm>
#include <string_view>
#include <span>
#include <iostream>
#include <format>
#include <stdexcept>

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

void CChessGUI::bufferPosition(std::span<const char> position) noexcept
{
	std::array<PieceSprite, boardSize> pieces{};
	std::array<PieceSprite, boardSize>::iterator back{ pieces.begin() };

	for (int rank{}; rank < rankSize; ++rank)
	{
		for (int file{}; file < fileSize; ++file)
		{
			const std::size_t index{ static_cast<std::size_t>(rank) * fileSize + file };
			const PieceSprite::Piece piece{ charToPiece(m_position[m_flipped ? 63 - index : index]) };
			if (piece == PieceSprite::Piece::NoPiece) continue;

			*back = PieceSprite(rank, file, piece);
			++back;
		}
	}

	m_window.bufferPieces(std::span(pieces.begin(), back));
}

void CChessGUI::play() noexcept
{
	resetCallback();

	while (m_window.open())
	{
		if (m_whiteToMove != m_playerIsWhite || m_engineMove)
		{
			if (!m_searching)
			{
				m_searching = true;
				engine_start_search(m_whiteToMove);
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
					m_engineMove = false;

					makeMove(source, destination);
				}
			}
		}

		m_window.draw();
	}
}

void CChessGUI::makeMove(int source, int destination) noexcept
{
	if (engine_move(m_whiteToMove, source, destination))
	{
		m_whiteToMove = !m_whiteToMove;

		m_window.bufferBoard(m_flipped ? 63 - source : source, m_flipped ? 63 - destination : destination);
	}

	const std::string_view position{ engine_get_position_char() };


	std::ranges::copy(position, m_position.begin());

	bufferPosition(m_position);
}



//callbacks
void CChessGUI::moveCallback(int source, int destination) noexcept
{
	if (m_flipped)
	{
		source = 63 - source;
		destination = 63 - destination;
	}

	makeMove(source, destination);
}

PieceSprite::Piece CChessGUI::pieceCallback(int square) noexcept
{
	square = m_flipped ? 63 - square : square;

	const PieceSprite::Piece piece{ charToPiece(m_position[square]) };
	m_position[square] = '.';
	bufferPosition(m_position);

	return piece;
}

void CChessGUI::resetCallback() noexcept
{
	engine_set_position_start();

	const std::string_view position{ engine_get_position_char() };
	std::ranges::copy(position, m_position.begin());

	bufferPosition(m_position);
	m_window.bufferBoard();
}

void CChessGUI::moveForwardCallback() noexcept
{
	if (engine_move_forward())
	{
		int source{}, destination{};
		engine_last_move(&source, &destination);

		const std::string_view position{ engine_get_position_char() };
		std::ranges::copy(position, m_position.begin());
		bufferPosition(m_position);

		if (source == destination)
		{
			m_window.bufferBoard();
		}
		else
		{
			m_window.bufferBoard(m_flipped ? 63 - source : source, m_flipped ? 63 - destination : destination);
		}
	}
}

void CChessGUI::moveBackCallback() noexcept
{
	if (engine_move_back())
	{
		int source{}, destination{};
		engine_last_move(&source, &destination);

		const std::string_view position{ engine_get_position_char() };
		std::ranges::copy(position, m_position.begin());
		bufferPosition(m_position);

		if (source == destination)
		{
			m_window.bufferBoard();
		}
		else
		{
			m_window.bufferBoard(m_flipped ? 63 - source : source, m_flipped ? 63 - destination : destination);
		}
	}
}

void CChessGUI::flipCallback() noexcept
{
	int source{}, destination{};
	engine_last_move(&source, &destination);

	m_flipped = !m_flipped;
	bufferPosition(m_position);

	if (source == destination)
	{
		m_window.bufferBoard();
	}
	else
	{
		m_window.bufferBoard(m_flipped ? 63 - source : source, m_flipped ? 63 - destination : destination);
	}
}

void CChessGUI::engineMoveCallback() noexcept
{
	m_engineMove = true;
}

void CChessGUI::playerColorCallback(bool playerIsWhite) noexcept
{
	m_playerIsWhite = playerIsWhite;
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