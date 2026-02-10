#include "MenuManager.h"

#include <string_view>



//	Public Methods

//getters
bool MenuManager::engineShouldMove() const noexcept
{
	const bool engineTurn{ m_whiteIsEngine == m_whiteToMove || m_blackIsEngine == !m_whiteToMove };
	const bool stopEngineTurn{ m_engineMovePause && m_engineJustMoved };

	return m_forceEngineMove || (!stopEngineTurn && engineTurn);
}

bool MenuManager::engineShouldRedraw() const noexcept
{
	return m_engineShouldRedraw;
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
	return m_seraching;
}

bool MenuManager::whiteToMove() const noexcept
{
	return m_whiteToMove;
}



//setters
bool* MenuManager::whiteIsEnginePtr() noexcept
{
	return &m_whiteIsEngine;
}

bool* MenuManager::blackIsEnginePtr() noexcept
{
	return &m_blackIsEngine;
}

bool* MenuManager::engineMovePausePtr() noexcept
{
	return &m_engineMovePause;
}

bool* MenuManager::flippedPtr() noexcept
{
	return &m_flipped;
}

int* MenuManager::engineSearchMillisecondsPtr() noexcept
{
	return &m_engineSearchMilliseconds;
}

void MenuManager::playerJustMoved() noexcept
{
	m_engineJustMoved = false;
	m_whiteToMove = !m_whiteToMove;
}

void MenuManager::engineJustMoved() noexcept
{
	m_engineJustMoved = true;
	m_whiteToMove = !m_whiteToMove;
}

void MenuManager::engineJustRedrew() noexcept
{
	m_engineShouldRedraw = true;
}

void MenuManager::setSearching(bool searching) noexcept
{
	m_searching = searching;
}



//buttons
void MenuManager::engineMove() noexcept
{
	m_forceEngineMove = true;
}

void MenuManager::reset() const noexcept
{

}

void MenuManager::moveBack() const noexcept
{

}

void MenuManager::moveForward() const noexcept
{

}