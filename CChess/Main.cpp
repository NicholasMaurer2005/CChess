#include <string_view>


#include "Engine.h"

//TODO: 
// 
//	find much better system for move sorting (currently 40% of CPU)
// 
//	renaming and namespaces
// 
//	for build -> performace guided optimization
// 
//	split engine into search class and state management class
//	

constexpr std::string_view startFen{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" };
constexpr std::string_view debugFen{ "Q7/4k3/7Q/8/8/3N4/2NK4/8" }; //Castle::None

int main()
{
	Engine engine;
}
