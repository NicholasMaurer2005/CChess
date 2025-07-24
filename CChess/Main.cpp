#include <iostream>
#include <string_view>
#include <chrono>


#include "Engine.h"
#include "State.h"
#include "PreGen.h"

//TODO: renaming and namespaces
//TODO: for build -> performace guided optimization

constexpr std::string_view startFen{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" };
constexpr std::string_view debugFen{ "rnb1kbnr/pp2pppp/8/Pqpp4/8/8/1PPPPPPP/R1BQKBNR" }; //Castle::All

int main()
{
	Engine engine{ startFen, Castle::All };
	engine.play();
}
