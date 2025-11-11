#pragma once

#include "Window.h"



class CChessGUI
{
//private properties
private:

	Window m_window;
	bool m_newPosition;
	bool m_whiteToMove;
	int m_sourceIndex;
	int m_destinationIndex;



//private methods
private:

	void play() noexcept;

	void bufferPosition() noexcept;

	//callbacks
	void moveCallback(int source, int destination) noexcept;

	void moveBackCallback() noexcept;

	void moveForwardCallback() noexcept;


//public methods
public:

	//constructor
	CChessGUI() noexcept;

	~CChessGUI() noexcept;
};

