#pragma once

#ifndef CCHESS_INCLUDE
#define CCHESS_INCLUDE

	#ifdef __cplusplus

		#define CCHESS_NOEXCEPT	noexcept

		extern "C" {

	#endif

	#define CCHESS_BOOL			int
	#define CCHESS_TRUE			1
	#define CCHESS_FALSE		0



	//	ENGINE
	
	//	Create new engine instance.
	CCHESS_BOOL engine_create() CCHESS_NOEXCEPT;

	//	Destroy current engine instance.
	void engine_destroy() CCHESS_NOEXCEPT;



	//	POSITION
	
	//	Set the current position to the start position.
	void engine_set_position_start() CCHESS_NOEXCEPT;

	//	Set the current position with a FEN notation string. Return CCHESS_TRUE if parse was successful. 
	CCHESS_BOOL engine_set_position_fen(const char* position) CCHESS_NOEXCEPT;

	//	Set the current position with a FEN notation string. Return CCHESS_TRUE if parse was successful. 
	CCHESS_BOOL engine_set_position_char(const char* position) CCHESS_NOEXCEPT;

	//	Get the current position as a FEN notation string. Memory is managed by the engine and invalid after engine_destroy() is 
	//	called.
	const char* engine_get_position_fen() CCHESS_NOEXCEPT;

	//	Get the current position as a FEN notation string. Memory is managed by the engine and invalid after engine_destroy() is 
	//	called.
	const char* engine_get_position_char() CCHESS_NOEXCEPT;

	//	Get the source and destination of the move that caused the current state. If it is currently the start position 'source' and
	//	'destination' are not changed.
	void engine_last_move(int* source, int* destination) CCHESS_NOEXCEPT;


	//	SEARCH
	
	//	Start async search. Stop with engine_stop_search(). Get statistics with engine_search_statistics().
	void engine_start_search(CCHESS_BOOL whiteToMove) CCHESS_NOEXCEPT;

	//	Stop the async search. 
	void engine_stop_search() CCHESS_NOEXCEPT;

	//	Get stats about the async search. Returns CCHESS_TRUE if any values changed since the last call - 
	//		done:					Engine is done searching.
	//		depth:					The current or final search depth.
	//		nodes_per_second:		How many nodes (brances of search tree, moves made) are reached per second.
	//		principal_variation:	A string containing the principal variation (line of best moves) seperated by commas. Memory 
	//								is owned by the engine and invalid after engine_destroy() is called.
	CCHESS_BOOL engine_search_info(CCHESS_BOOL* done, int* evaluation, int* depth, float* nodes_per_second, float* timeRemaining, const char** principal_variation) CCHESS_NOEXCEPT;

	//	Get the best move after the search is done. If the search is not done or stopSearch() has not been called 'source' and 
	//	'destination' are not modified and the function returns CCHESS_FALSE
	CCHESS_BOOL engine_best_move(int* source, int* destination) CCHESS_NOEXCEPT;



	//	MOVE

	//	Try to make a legal move. If the move could be made the engine returns CCHESS_TRUE. If the engine is searching or the move 
	//	is illegal the engine returns CCHESS_FALSE.
	CCHESS_BOOL engine_move(CCHESS_BOOL whiteToMove, int source, int destination) CCHESS_NOEXCEPT;

	//	Try to move back one half move. If the move back could be made the engine returns CCHESS_TRUE. If the engine is searching or 
	//	their are no prior half moves the engine returns CCHESS_FALSE.
	CCHESS_BOOL engine_move_forward() CCHESS_NOEXCEPT;

	//	Try to move back one half move. If the move back could be made the engine returns CCHESS_TRUE. If the engine is searching or 
	//	their are no further half moves the engine returns CCHESS_FALSE.
	CCHESS_BOOL engine_move_back() CCHESS_NOEXCEPT;



	#ifdef __cplusplus
		}
	#endif

#endif