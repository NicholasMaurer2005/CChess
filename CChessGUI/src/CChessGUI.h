#pragma once

#include <array>

#include "MenuManager.h"
#include "Window.h"
#include "PieceSprite.h"



class CChessGUI
{
private:

	//	Private Definitions

	//constants
	static constexpr int boardSize{ 64 };



	//usings
	using CharPosition = std::array<char, boardSize>;



private:

	//	Private Members

	MenuManager m_menuManager;
	Window m_window{ m_menuManager };
	CharPosition m_position{};
	int m_moveSource{};
	int m_moveDestination{};



private:

	//	Private Members

	void play() noexcept;

	void bufferPosition() noexcept;

	void updatePosition() noexcept;

	PieceSprite::Piece pieceCallback(int square) noexcept;

public:

	CChessGUI();

	~CChessGUI();
};