#include <iostream>
#include <string_view>
#include <chrono>


#include "Engine.h"
#include "State.h"
#include "PreGen.h"

//TODO: renaming and namespaces
//TODO: for build -> performace guided optimization

constexpr std::string_view startFen{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" };
constexpr std::string_view debugFen{ "Q7/4k3/7Q/8/8/3N4/2NK4/8" }; //Castle::None

int main()
{
	Engine engine{ debugFen, Castle::None };
	engine.play();
}
