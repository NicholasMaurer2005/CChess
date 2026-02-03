#pragma once

#include "Window.h"



class CChessGUI
{
private:
	
	//	Private Members

	Window m_window{
		[this](int width, int height) { return this->moveCallback(width, height); },
		[this]() { this->moveBackCallback(); },
		[this]() { this->moveForwardCallback(); }
	};

	bool m_whiteToMove{ true };
	bool m_searching{ false };



private:

	//	Private Methods

	void play() noexcept;

	void bufferPosition() noexcept;

	bool moveCallback(int source, int destination) noexcept;

	void moveBackCallback() noexcept;

	void moveForwardCallback() noexcept;



public:

	//	Public Methods
	
	//constructor
	CChessGUI();

	~CChessGUI() noexcept;
};

