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

	MenuManager m_menuManager{ [this](int square) { return };
	Window m_window{ m_menuManager };
	CharPosition m_position{};
	int m_moveSource{};
	int m_moveDestination{};



private:

	//	Private Members

	void play() noexcept;

	void bufferPosition() noexcept;

	void drawPosition() noexcept;

	void makeMove(int source, int destination) noexcept;


public:

	CChessGUI();

	~CChessGUI();
};