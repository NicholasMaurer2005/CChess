#include <exception>
#include <format>
#include <iostream>

#include "CChessGUI.h"



int main()
{
	try
	{
		CChessGUI gui;
	}
	catch (const std::exception& e)
	{
		std::cout << std::format("failed due to: {}\n", e.what());
	}
}