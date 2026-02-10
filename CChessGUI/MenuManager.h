#pragma once

#include <string>
#include <string_view>



class MenuManager
{
private:

	//	Private Members

	bool m_searching{ false };
	bool m_whiteToMove{ true };
	bool m_engineJustMoved{ false };
	bool m_forceEngineMove{ false };
	bool m_engineShouldRedraw{ false };

	//settings
	bool m_whiteIsEngine{ false };
	bool m_blackIsEngine{ true };
	bool m_flipped{ false };
	bool m_engineMovePause{ false };
	int m_engineSearchMilliseconds{ 500 };
	
	//info
	std::string m_principalVariation;
	std::string m_evaluationString;



public:

	//	Public Methods

	//getters
	bool engineShouldMove() const noexcept;

	bool engineShouldRedraw() const noexcept;

	std::string_view principalVariation();

	std::string_view evaluationString();

	bool searching() const noexcept;

	bool whiteToMove() const noexcept;



	//setters
	bool* whiteIsEnginePtr() noexcept;

	bool* blackIsEnginePtr() noexcept;

	bool* engineMovePausePtr() noexcept;

	bool* flippedPtr() noexcept;

	int* engineSearchMillisecondsPtr() noexcept;

	void playerJustMoved() noexcept;

	void engineJustMoved() noexcept;

	void engineJustRedrew() noexcept;

	void setSearching(bool searching) noexcept;



	//buttons
	void engineMove() noexcept;

	void reset() const noexcept;

	void moveBack() const noexcept;

	void moveForward() const noexcept;
};

