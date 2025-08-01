#ifndef CCHESS_INCLUDE
#define CCHESS_INCLUDE

#ifdef __cplusplus

//noexcept define
#define CCHESS_NOEXCEPT noexcept

extern "C" {
#endif

	/* ENGINE */

	//create new instance of engine. must be called before any other function
	void engine_create() CCHESS_NOEXCEPT;

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
	//memory of the returned FEN
	const char* engine_get_position() CCHESS_NOEXCEPT;



	/* SEARCH */

	//search for the best move for the side to play and write to "source" and "destination". the 
	//engine is not responsible for freeing the memory of "source" and "destination"
	void engine_search(int* source, int* destination) CCHESS_NOEXCEPT;

	//search for the best move for side "white" and write to "source" and "destination". if "white" 
	//is 1 it will search for white, if "white" is 0 it will search for black. the engine is 
	//not responsible for freeing the memory of "source" and "destination"
	void engine_search_color(int white, int* source, int* destination) CCHESS_NOEXCEPT;

	//search and make the best move for the side to play
	void engine_search_and_move() CCHESS_NOEXCEPT;

	//search and make the best move for the side "white". if "white" is 1 it will for white, 
	//if "white" is 0 it will search for black
	void engine_search_color_and_move(int white) CCHESS_NOEXCEPT;



	/* MOVE */

	//make move. if move is legal it will make the move and return 1. if the move is not legal it
	//will do nothing and return 0
	int engine_move(int source, int destination) CCHESS_NOEXCEPT;


#ifdef __cplusplus
}
#endif

#endif