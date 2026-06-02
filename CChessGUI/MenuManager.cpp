#include "MenuManager.h"

#include <string_view>
#include <utility>
#include <string>

#include "PieceSprite.h"



//	Public Methods

//constructors
MenuManager::MenuManager(GetPieceCallback getPieceCallback) noexcept
	: m_getPieceCallback(getPieceCallback) {}



//getters
bool* MenuManager::whiteIsEnginePtr() noexcept
{
	return &m_whiteIsEngine;
}

bool* MenuManager::blackIsEnginePtr() noexcept
{
	return &m_blackIsEngine;
}

bool* MenuManager::flippedPtr() noexcept
{
	return &m_flipped;
}

bool* MenuManager::pauseAfterEngineMovePtr() noexcept
{
	return &m_pauseAfterEngineMove;
}

float* MenuManager::engineSearchSecondsPtr() noexcept
{
	return &m_engineSearchSeconds;
}

bool MenuManager::engineShouldMove() noexcept
{
	const bool engineTurn{ m_whiteIsEngine && m_whiteToMove || m_blackIsEngine && !m_whiteToMove };
	const bool stopEngineTurn{ m_pauseAfterEngineMove && m_engineJustMoved };
	const bool engineShouldMove{ m_forceEngineMove || (!stopEngineTurn && engineTurn) };

	m_forceEngineMove = false;

	return engineShouldMove;
}

bool MenuManager::engineShouldRedraw() noexcept
{
	const bool value{ m_engineShouldRedraw };
	
	m_engineShouldRedraw = false;

	return value;
}

bool MenuManager::engineShouldReset() noexcept
{
	const bool value{ m_engineShouldReset };

	m_engineShouldReset = false;

	return value;
}

bool MenuManager::engineShouldMoveForward() noexcept
{
	const bool value{ m_engineShouldMoveForward };

	m_engineShouldMoveForward = false;

	return value;
}

bool MenuManager::engineShouldMoveBack() noexcept
{
	const bool value{ m_engineShouldMoveBack };

	m_engineShouldMoveBack = false;

	return value;
}

bool MenuManager::engineShouldParsePlayerMove() noexcept
{
	const bool value{ m_playerJustMoved };

	m_playerJustMoved = false;

	return value;
}

bool MenuManager::engineShouldUpdateSearchTime() noexcept
{
	const bool value{ m_engineShouldUpdateSearchTime };

	m_engineShouldUpdateSearchTime = false;

	return value;
}

std::string_view MenuManager::evaluationString() const noexcept
{
	return m_evaluationString;
}

bool MenuManager::searching() const noexcept
{
	return m_searching;
}

bool MenuManager::whiteToMove() const noexcept
{
	return m_whiteToMove;
}

std::pair<int, int> MenuManager::lastPlayerMove() const noexcept
{
	return std::pair(m_playerMoveSource, m_playerMoveDestination);
}

PieceSprite::Piece MenuManager::getPiece(int square) const noexcept
{
	return m_getPieceCallback(square);
}

float MenuManager::secondsRemaining() const noexcept
{
	return m_secondsRemaining;
}



//setters
void MenuManager::setSearching(bool searching) noexcept
{
	m_searching = searching;
}

void MenuManager::flipColorToMove() noexcept
{
	m_whiteToMove = !m_whiteToMove;
}

void MenuManager::setEvaluationString(std::string evaluationString) noexcept
{
	m_evaluationString = std::move(evaluationString);
}

void MenuManager::setPlayerMove(int source, int destination) noexcept
{
	m_playerMoveSource = source;
	m_playerMoveDestination = destination;
	m_playerJustMoved = true;
}

void MenuManager::setEngineShouldRedraw() noexcept
{
	m_engineShouldRedraw = true;
}

void MenuManager::forceEngineMove() noexcept
{
	m_forceEngineMove = true;
}

void MenuManager::setEngineJustMoved(bool value) noexcept
{
	m_engineJustMoved = value;
}

void MenuManager::setSecondsRemaining(float value) noexcept
{
	m_secondsRemaining = value;
}

void MenuManager::setEngineShouldUpdateSearchTime() noexcept
{
	m_engineShouldUpdateSearchTime = true;
}



//buttons
void MenuManager::engineMove() noexcept
{
	m_forceEngineMove = true;
}

void MenuManager::reset() noexcept
{
	m_evaluationString.clear();

	m_engineJustMoved = false;
	m_whiteIsEngine = false;
	m_blackIsEngine = true;
	m_flipped = false;
	m_whiteToMove = true;

	m_engineShouldReset = true;
}

void MenuManager::moveBack() noexcept
{
	m_engineShouldMoveBack = true;
}

void MenuManager::moveForward() noexcept
{
	m_engineShouldMoveForward = true;
}