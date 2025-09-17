#pragma once

#include "Window.h"



class CChessGUI
{
//private properties
private:

	Window m_window;
	bool m_newPosition;


//private methods
private:

	void play() noexcept;



//public methods
public:

	//constructor
	CChessGUI() noexcept;

	~CChessGUI() noexcept;
};

