#pragma once

#include <array>

#include "Window.h"
#include "PieceSprite.h"



class CChessGUI
{
private:

	//	Private Definitions

	static constexpr int boardSize{ 64 };



private:
	
	//	Private Members

	Window m_window{
		[this](int width, int height) { return this->moveCallback(width, height); },
		[this](std::size_t square) { return this->pieceCallback(square); },
	};

	bool m_whiteToMove{ true };
	bool m_searching{ false };
	std::array<char, boardSize> m_position;



private:

	//	Private Methods

	void play() noexcept;

	void bufferPosition() noexcept;

	bool moveCallback(int source, int destination) noexcept;

	PieceSprite::Piece pieceCallback(std::size_t square) noexcept;



public:

	//	Public Methods
	
	//constructor
	CChessGUI();

	~CChessGUI() noexcept;
};

