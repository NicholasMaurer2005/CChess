#include <iostream>
#include <string_view>
#include <chrono>


#include "Engine.h"
#include "State.h"
#include "PreGen.h"

//TODO: renaming and namespaces
//TODO: for build -> performace guided optimization

constexpr std::string_view startFen{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" };
constexpr std::string_view debugFen{ "8/3n1k1p/3p1p2/2pq4/P1N5/1K5P/8/8" };

int main()
{
	Engine engine{ debugFen, Castle::None};

	engine.play();
}
