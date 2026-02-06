#pragma once

#include <array>
#include <span>

#include "Window.h"
#include "PieceSprite.h"



class CChessGUI
{
private:

	//	Private Definitions

	//constants
	static constexpr int boardSize{ 64 };
	static constexpr int halfMoveCount{ 100 };



	//usings
	using CharPosition = std::array<char, boardSize>;



private:

	//	Private Members

	Window m_window{
		[this](int width, int height)	{ this->moveCallback(width, height); },
		[this](int square)				{ return this->pieceCallback(square); },
		[this]()						{ this->resetCallback(); },
		[this]()						{ this->moveForwardCallback(); },
		[this]()						{ this->moveBackCallback(); },
		[this]()						{ this->flipCallback(); },
		[this]()						{ this->engineMoveCallback(); },
		[this](bool playerIsWhite)		{ this->playerColorCallback(playerIsWhite); },
		[this](bool enginePlayItself)	{ this->enginePlayItselfCallback(enginePlayItself); },
	};

	bool m_whiteToMove{ true };
	bool m_searching{ false };
	bool m_playerIsWhite{ true };
	bool m_flipped{ false };
	bool m_engineMove{ false };
	bool m_enginePlayItself{ false };

	CharPosition m_position{};



private:

	//	Private Methods

	void play() noexcept;

	void bufferPosition(std::span<const char> position) noexcept;

	void makeMove(int source, int destination) noexcept;



	//callbacks
	void moveCallback(int source, int destination) noexcept;

	PieceSprite::Piece pieceCallback(int square) noexcept;

	void resetCallback() noexcept;

	void moveBackCallback() noexcept;

	void moveForwardCallback() noexcept;

	void flipCallback() noexcept;

	void engineMoveCallback() noexcept;

	void playerColorCallback(bool playerIsWhite) noexcept;

	void enginePlayItselfCallback(bool enginePlayItself) noexcept;



public:

	//	Public Methods
	
	//constructor
	CChessGUI();

	~CChessGUI() noexcept;
};

