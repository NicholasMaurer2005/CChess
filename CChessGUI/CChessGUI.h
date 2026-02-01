#pragma once

#include "Window.h"



class CChessGUI
{
private:
	
	//	Private Members

	Window m_window{
		[this](int width, int height) { this->moveCallback(width, height); },
		[this]() { this->moveBackCallback(); },
		[this]() { this->moveForwardCallback(); }
	};



private:

	//	Private Methods

	void play() noexcept;

	void bufferPosition() noexcept;

	void moveCallback(int source, int destination) noexcept;

	void moveBackCallback() noexcept;

	void moveForwardCallback() noexcept;



public:

	//	Public Methods
	
	//constructor
	CChessGUI() noexcept;

	~CChessGUI() noexcept;
};

