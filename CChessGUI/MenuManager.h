#pragma once

#include <string>
#include <string_view>



class MenuManager
{
private:

	//	Private Members

	//settings
	bool whiteIsEngine{ false };
	bool blackIsEngine{ true };
	bool m_flipped{ false };
	bool m_engineMoveAutoPlay{ true };
	int m_engineSearchMilliseconds{ 500 };
	
	//info
	std::string m_principalVariation;
	std::string m_evaluationString;



public:

	//	Public Methods

	//getters
	bool engineShouldMove() noexcept;

	std::string_view principalVariation();

	std::string_view evaluationString();



	//setters
	bool* whiteIsEnginePtr() noexcept;

	bool* blackIsEnginePtr() noexcept;

	bool* engineMoveAutoPlayPtr() noexcept;

	bool* flipped() noexcept;

	int* engineSearchMilliseconds() noexcept;



	//buttons
	void engineMove() noexcept;

	void reset() const noexcept;

	void moveBack() const noexcept;

	void moveForward() const noexcept;
};

