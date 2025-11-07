#pragma once

#include "Window.h"



class CChessGUI
{
//private properties
private:

	Window m_window;
	bool m_newPosition;
	bool m_whiteToMove;



//private methods
private:

	void play() noexcept;

	void moveCallback(int source, int destination) noexcept;

	void bufferPosition() noexcept;

	void moveBack() noexcept;

	void moveForward() noexcept;



//public methods
public:

	//constructor
	CChessGUI() noexcept;

	~CChessGUI() noexcept;
};

