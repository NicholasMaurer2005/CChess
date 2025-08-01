#ifndef CCHESS_INCLUDE
#define CCHESS_INCLUDE

#ifdef __cplusplus

//noexcept define
#define CCHESS_NOEXCEPT noexcept
#define CCHESS_NODISCARD [[nodiscard]]

extern "C" {
#endif

	/* ENGINE */

	//create new instance of engine. must be called before any other function
	int engine_create() CCHESS_NOEXCEPT;

	//destroy engine. call to free up the engines resources
	void engine_destroy() CCHESS_NOEXCEPT;

	//set position to the start position. engine_create() will initialize to the start position, 
	//calling on engine creation is optional
	void engine_position_start() CCHESS_NOEXCEPT;

	//set position to FEN notation string. if "fen" is a valid FEN then the function will switch to that position
	// and it will return 1. if "fen" is not a valid FEN than nothing will happen and it will return 0. 
	//the engine is not responsible for freeing the memory of "fen"
	int engine_position_fen(const char* fen) CCHESS_NOEXCEPT;

	//return engine position as FEN notation string. the engine is not responsible for freeing the
	//memory of the returned const char*
	CCHESS_NODISCARD const char* engine_get_position() CCHESS_NOEXCEPT;

	//return engine position as a char[64] array. this matches the chess board with each piece 
	//represented with a letter, e.g. 'P' for white pawn, 'r' for black rook. no-piece is represented
	// by ' '. the engine is not responsible for freeing the memory of the returned const char*
	//TODO: maybe find a better way? with my gui this will have 3 allocations and 2 copies of the data
	CCHESS_NODISCARD const char* engine_get_char_position() CCHESS_NOEXCEPT;

	void engine_set_search_milliseconds(int milliseconds) CCHESS_NOEXCEPT;

	int engine_get_search_milliseconds() CCHESS_NOEXCEPT;



	/* SEARCH */

	//search for the best move for the side to play and write to "source" and "destination". the 
	//engine is not responsible for freeing the memory of "source" and "destination"
	void engine_search(int* source, int* destination) CCHESS_NOEXCEPT;

	//search for the best move for side "white" and write to "source" and "destination". if "white" 
	//is 1 it will search for white, if "white" is 0 it will search for black. the engine is 
	//not responsible for freeing the memory of "source" and "destination"
	void engine_search_color(int white, int* source, int* destination) CCHESS_NOEXCEPT;



	/* MOVE */

	//make move. if move is legal it will make the move and return 1. if the move is not legal it
	//will do nothing and return 0
	int engine_move(int source, int destination) CCHESS_NOEXCEPT;


#ifdef __cplusplus
}
#endif

#endif