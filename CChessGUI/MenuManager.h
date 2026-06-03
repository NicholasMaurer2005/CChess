#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <utility>

#include "PieceSprite.h"



class MenuManager
{
private:

	//	Private Definitions

	using GetPieceCallback = std::function<PieceSprite::Piece(int square)>;



private:

	//	Private Members

	bool m_engineShouldRedraw{};
	bool m_engineShouldReset{};
	bool m_engineShouldMoveForward{};
	bool m_engineShouldMoveBack{};
	bool m_engineShouldUpdateSearchTime{ true };

	int m_playerMoveSource{};
	int m_playerMoveDestination{};
	bool m_playerJustMoved{};
	bool m_whiteToMove{ true };
	bool m_engineJustMoved{};
	bool m_forceEngineMove{};

	//settings
	bool m_whiteIsEngine{};
	bool m_blackIsEngine{ true };
	bool m_flipped{};
	bool m_pauseAfterEngineMove{ true };
	float m_engineSearchSeconds{ 1.0f };
	
	//info
	bool m_searching{};
	float m_secondsRemaining{};
	float m_knps{};
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

	float* engineSearchSecondsPtr() noexcept;

	bool engineShouldMove() noexcept;

	bool engineShouldParsePlayerMove() noexcept;

	bool engineShouldReset() noexcept;

	bool engineShouldMoveForward() noexcept;

	bool engineShouldMoveBack() noexcept;

	bool engineShouldRedraw() noexcept;

	bool engineShouldUpdateSearchTime() noexcept;

	std::string_view evaluationString() const noexcept;

	bool searching() const noexcept;

	bool whiteToMove() const noexcept;

	std::pair<int, int> lastPlayerMove() const noexcept;

	PieceSprite::Piece getPiece(int square) const noexcept;

	float secondsRemaining() const noexcept;

	float knps() const noexcept;



	//setters
	void setSearching(bool searching) noexcept;

	void flipColorToMove() noexcept;

	void setEvaluationString(std::string evaluationString) noexcept;

	void setPlayerMove(int source, int destination) noexcept;

	void setEngineShouldRedraw() noexcept;

	void forceEngineMove() noexcept;

	void setEngineJustMoved(bool value) noexcept;

	void setSecondsRemaining(float value) noexcept;

	void setEngineShouldUpdateSearchTime() noexcept;

	void setKnps(float value) noexcept;



	//buttons
	void engineMove() noexcept;

	void reset() noexcept;

	void moveBack() noexcept;

	void moveForward() noexcept;
};

