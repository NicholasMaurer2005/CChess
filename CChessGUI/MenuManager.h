#pragma once

#include <string>
#include <string_view>
#include <utility>
#include <functional>

#include "PieceSprite.h"



class MenuManager
{
private:

	//	Private Definitions

	using GetPieceCallback = std::function<PieceSprite::Piece(int square)>;



private:

	//	Private Members

	bool m_searching{};
	bool m_whiteToMove{ true };
	bool m_engineJustMoved{};
	bool m_forceEngineMove{};

	bool m_engineShouldRedraw{};
	bool m_engineShouldReset{};
	bool m_engineShouldMoveForward{};
	bool m_engineShouldMoveBack{};

	int m_playerMoveSource{};
	int m_playerMoveDestination{};
	bool m_playerJustMoved{};

	//settings
	bool m_whiteIsEngine{};
	bool m_blackIsEngine{ true };
	bool m_flipped{};
	bool m_pauseAfterEngineMove{ true };
	int m_engineSearchMilliseconds{ 500 };
	
	//info
	std::string m_principalVariation;
	std::string m_evaluationString;

	//callback
	GetPieceCallback m_getPieceCallback;



public:

	//	Public Methods

	//constructors
	MenuManager(GetPieceCallback getPieceCallback) noexcept;



	//getters
	bool* whiteIsEnginePtr() noexcept;

	bool* blackIsEnginePtr() noexcept;

	bool* flippedPtr() noexcept;

	bool* pauseAfterEngineMovePtr() noexcept;

	int* engineSearchMillisecondsPtr() noexcept;

	bool engineShouldMove() noexcept;

	bool engineShouldRedraw() noexcept;

	bool engineShouldReset() noexcept;

	bool engineShouldMoveForward() noexcept;

	bool engineShouldMoveBack() noexcept;

	bool engineShouldParsePlayerMove() noexcept;

	std::string_view principalVariation();

	std::string_view evaluationString();

	bool searching() const noexcept;

	bool whiteToMove() const noexcept;

	std::pair<int, int> lastPlayerMove() const noexcept;

	PieceSprite::Piece getPiece(int square) const noexcept
	{
		return m_getPieceCallback(square);
	}




	//setters
	void setSearching(bool searching) noexcept;

	void setPrincipalVariation(std::string_view principalVariation) noexcept;

	void setEvaluationString(std::string_view evaluationString) noexcept;

	void setPlayerMove(int source, int destination) noexcept;

	void setEngineShouldRedraw() noexcept;


	//buttons
	void engineMove() noexcept;

	void reset() noexcept;

	void moveBack() noexcept;

	void moveForward() noexcept;
};

