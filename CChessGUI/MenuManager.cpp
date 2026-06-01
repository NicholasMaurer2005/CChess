#include "MenuManager.h"

#include <string_view>
#include <utility>
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

int* MenuManager::engineSearchMillisecondsPtr() noexcept
{
	return &m_engineSearchMilliseconds;
}

bool MenuManager::engineShouldMove() noexcept
{
	const bool engineTurn{ m_whiteIsEngine == m_whiteToMove || m_blackIsEngine == !m_whiteToMove };
	const bool stopEngineTurn{ m_pauseAfterEngineMove && m_engineJustMoved };
	const bool engineShouldMove{ m_forceEngineMove || (!stopEngineTurn && engineTurn) };

	m_forceEngineMove = false;
	m_engineJustMoved = true;
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

std::string_view MenuManager::principalVariation()
{
	return m_principalVariation;
}

std::string_view MenuManager::evaluationString()
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



//setters
void MenuManager::setSearching(bool searching) noexcept
{
	m_searching = searching;
}

void MenuManager::setPrincipalVariation(std::string_view principalVariation) noexcept
{
	m_principalVariation = principalVariation;
}

void MenuManager::setEvaluationString(std::string_view evaluationString) noexcept
{
	m_evaluationString = evaluationString;
}

void MenuManager::setPlayerMove(int source, int destination) noexcept
{
	m_playerMoveSource = source;
	m_playerMoveDestination = destination;
	m_playerJustMoved = true;
	m_engineShouldRedraw = true;
}

void MenuManager::setEngineShouldRedraw() noexcept
{
	m_engineShouldRedraw = true;
}



//buttons
void MenuManager::engineMove() noexcept
{
	m_forceEngineMove = true;
}

void MenuManager::reset() noexcept
{
	m_engineShouldReset = true;
	m_engineShouldRedraw = true;
}

void MenuManager::moveBack() noexcept
{
	m_engineShouldMoveForward = true;
	m_engineShouldRedraw = true;
}

void MenuManager::moveForward() noexcept
{
	m_engineShouldMoveBack = true;
	m_engineShouldRedraw = true;
}