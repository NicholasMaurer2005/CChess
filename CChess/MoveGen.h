#pragma once

#include "State.h"
#include "MoveList.h"

class MoveGen //TODO: make this a namespace and have all function static?
{

public:

	MoveGen() noexcept;

	MoveList generateMoves(const State& state) noexcept;

};

