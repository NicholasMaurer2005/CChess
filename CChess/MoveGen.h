#pragma once

#include "PreGen.h"
#include "State.h"

class MoveGen
{
private:

	const PreGen m_PreGen;

public:

	MoveGen();

	MoveList generateMoves(State& state);
};

