# create a static library project 
add_library(CChess STATIC)

# add source files
target_sources(CChess PRIVATE 
	src/BitBoard.cpp
	src/CChess.cpp
	src/Engine.cpp
	src/Evaluate.cpp
	src/KillerMoveHistory.cpp
	src/Main.cpp
	src/Move.cpp
	src/MoveGen.cpp
	src/PreGen.cpp
	src/State.cpp
)

# add include directories
target_include_directories(CChess
	PUBLIC include
	PRIVATE src
)

