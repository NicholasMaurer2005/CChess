#pragma once

#include "State.h"
#include "MoveList.h"

class MoveGen //TODO: make this a namespace and have all functions static?
{

public:

	MoveGen() noexcept;

	MoveList generateMoves(bool white, const State& state) const noexcept;

};

