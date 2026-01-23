#pragma once

#include <thread>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <condition_variable>

#include "State.h"
#include "Evaluate.h"
#include "MoveGen.h"
#include "MoveList.hpp"
#include "Move.h"


//release - publish everything before me
//aquire - dont read until after me
//relazed - compiler does what it wants



namespace async
{
	class Engine
	{
	public:

		struct SearchInfo
		{
			int depth;
			int evaluation;
			float nodesPerSecond;
			float timeRemaining;
		};

	private:

		static constexpr int bestValue{ 9999999 };
		static constexpr int worstValue{ -9999999 };
		static constexpr int checkmateScore{ -999999 };

		using clock = std::chrono::high_resolution_clock;
		using duration = std::chrono::duration<float>;

		State m_currentState{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", Castle::All };
		bool m_currentWhiteToMove{ true };
		bool m_stopSearch{};
		std::jthread m_worker;
		SearchInfo m_info;
		int m_searchMilliseconds{ 500 };
		clock::time_point m_searchStart;
		std::uint64_t m_nodeCount;

		std::mutex m_mutex;
		std::condition_variable m_cv;

		

	private:

		int search(const State& state, bool whiteToMove, int depth, int alpha, int beta) noexcept;

		void logNodesPerSecond() noexcept;

	public:

		Engine() noexcept;

		~Engine();

		void startSearch() noexcept;

		void startSearch(const State& state, bool whiteToMove) noexcept;

		void stopSearch() noexcept;

		SearchInfo info() noexcept;

		void searchRun() noexcept;
	};

	static void findWhiteSquares(State& state) noexcept
	{
		std::uint64_t squares{};

		BitBoard pawns{ state.pieceOccupancyT<Piece::WhitePawn>() };
		BitBoard knights{ state.pieceOccupancyT<Piece::WhiteKnight>() };
		BitBoard bishops{ state.pieceOccupancyT<Piece::WhiteBishop>() };
		BitBoard rooks{ state.pieceOccupancyT<Piece::WhiteRook>() };
		BitBoard queens{ state.pieceOccupancyT<Piece::WhiteQueen>() };
		BitBoard kings{ state.pieceOccupancyT<Piece::WhiteKing>() };

		while (pawns.board())
		{
			const int index{ pawns.popLeastSignificantBit() };
			squares |= MoveGen::whitePawnMoves(index).board();
		}

		while (knights.board())
		{
			const int index{ knights.popLeastSignificantBit() };
			squares |= MoveGen::knightMoves(index).board();
		}

		while (bishops.board())
		{
			const int index{ bishops.popLeastSignificantBit() };
			squares |= MoveGen::bishopMoves(index, state.occupancy()).board();
		}

		while (rooks.board())
		{
			const int index{ rooks.popLeastSignificantBit() };
			squares |= MoveGen::rookMoves(index, state.occupancy()).board();
		}

		while (queens.board())
		{
			const int index{ queens.popLeastSignificantBit() };
			squares |= MoveGen::bishopMoves(index, state.occupancy()).board() | MoveGen::rookMoves(index, state.occupancy()).board();
		}

		const int kingIndex{ kings.popLeastSignificantBit() };
		squares |= MoveGen::kingMoves(kingIndex).board();

		state.setWhiteSquares(squares & ~state.whiteOccupancy().board());
	}

	static void findBlackSquares(State& state) noexcept
	{
		std::uint64_t squares{};

		BitBoard pawns{ state.pieceOccupancyT<Piece::BlackPawn>() };
		BitBoard knights{ state.pieceOccupancyT<Piece::BlackKnight>() };
		BitBoard bishops{ state.pieceOccupancyT<Piece::BlackBishop>() };
		BitBoard rooks{ state.pieceOccupancyT<Piece::BlackRook>() };
		BitBoard queens{ state.pieceOccupancyT<Piece::BlackQueen>() };
		BitBoard kings{ state.pieceOccupancyT<Piece::BlackKing>() };

		while (pawns.board())
		{
			const int index{ pawns.popLeastSignificantBit() };
			squares |= MoveGen::blackPawnMoves(index).board();
		}

		while (knights.board())
		{
			const int index{ knights.popLeastSignificantBit() };
			squares |= MoveGen::knightMoves(index).board();
		}

		while (bishops.board())
		{
			const int index{ bishops.popLeastSignificantBit() };
			squares |= MoveGen::bishopMoves(index, state.occupancy()).board();
		}

		while (rooks.board())
		{
			const int index{ rooks.popLeastSignificantBit() };
			squares |= MoveGen::rookMoves(index, state.occupancy()).board();
		}

		while (queens.board())
		{
			const int index{ queens.popLeastSignificantBit() };
			squares |= MoveGen::bishopMoves(index, state.occupancy()).board() | MoveGen::rookMoves(index, state.occupancy()).board();
		}

		const int kingIndex{ kings.popLeastSignificantBit() };
		squares |= MoveGen::kingMoves(kingIndex).board();

		state.setBlackSquares(squares & ~state.blackOccupancy().board());
	}

	static bool makeLegalMove(State& state, Move move, bool whiteToMove)
	{
		state.makeMove(whiteToMove, move);

		findWhiteSquares(state);
		findBlackSquares(state);

		return whiteToMove
			? (state.pieceOccupancyT<Piece::BlackKing>().board() && !state.whiteKingInCheck())
			: (state.pieceOccupancyT<Piece::WhiteKing>().board() && !state.blackKingInCheck());
	}

	static void worker(std::stop_token token, std::mutex& mutex, std::condition_variable& cv, Engine& engine) noexcept
	{
		while (!token.stop_requested())
		{
			{
				std::unique_lock lock(mutex);

				cv.wait(lock);
			}

			engine.searchRun();
		}
	}

	void Engine::logNodesPerSecond() noexcept
	{
		const clock::time_point now{ clock::now() };
		const duration elapsed{ now - m_searchStart };
		m_info.nodesPerSecond = m_nodeCount / elapsed.count();
	}

	static std::uint32_t logCounter{};
	int Engine::search(const State& state, bool whiteToMove, int depth, int alpha, int beta) noexcept
	{
		if (logCounter & 0x00010000) logNodesPerSecond();
		++logCounter;

		if (depth == 0)
		{
			++m_nodeCount;
			return evaluate(state);
		}

		if (m_stopSearch) return;

		const MoveList moves{ MoveGen::generateMoves(whiteToMove, state) };
		int legalMoves{};
		int bestScore{ worstValue };

		std::ranges::for_each(moves, [this, &state, whiteToMove, &legalMoves, depth, beta, &alpha, &bestScore](Move move) {
			State stateCopy{ state };

			if (makeLegalMove(stateCopy, move, whiteToMove))
			{
				++legalMoves;
				const int score{ -search(stateCopy, !whiteToMove, depth - 1, -beta, -alpha) };
				bestScore = std::max(bestScore, score);
				alpha = std::max(alpha, score);

				if (alpha >= beta)
				{
					//m_killerMoves.push(depth, move);
					return;
				}
			}
			});

		if (legalMoves == 0)
		{
			//check for white or black checkmate
			if (whiteToMove ? state.whiteKingInCheck() : state.blackKingInCheck())
			{
				return checkmateScore - depth;
			}
			else
			{
				return 0;
			}
		}
	}

	void Engine::searchRun() noexcept
	{
		static constexpr int maxDepth{ 50 };

		for (int depth{ 1 }; depth <= maxDepth; ++depth)
		{
			const int score{ search(m_currentState, m_currentWhiteToMove, depth) };
			m_info.depth = depth;
			m_info.evaluation = score;
		}
	}
	
	Engine::Engine() noexcept
		: m_worker(worker, std::ref(m_mutex), std::ref(m_cv), std::ref(*this)) { }

	Engine::~Engine()
	{
		m_cv.notify_one();
	}

	void Engine::startSearch() noexcept
	{
		m_stopSearch = false;
		m_cv.notify_one();
	}

	void Engine::startSearch(const State& state, bool whiteToMove) noexcept
	{
		m_currentState = state;
		m_currentWhiteToMove = whiteToMove;
		startSearch();
	}

	void Engine::stopSearch() noexcept
	{
		m_stopSearch = true;
	}

	Engine::SearchInfo Engine::info() noexcept
	{
		return m_info;
	}
}



struct SearchInfo
{
	int searchDepth;
	int evaluation;
};

class Engine
{

//private structs
private:

	struct ScoredMove
	{
		Move move;
		int score;
	};

	struct PositionHistroy
	{
		State state;
		Move move;
	};



//private properties
private:

	//engine
	MoveGen m_moveGen; //TODO: maybe make static?
	State m_state;
	bool m_whiteToMove;
	SearchInfo m_info;
	MoveList m_legalMoves;
	Move m_lastMove;
	std::array<char, 64> m_charPosition;

	//search
	bool m_gameOver;
	std::atomic_bool m_stopSearch;
	KillerMoveHistory m_killerMoves;
	int m_searchMilliseconds;

	//history
	std::vector<PositionHistroy> m_history;
	std::size_t m_historyPosition;

	//async
	bool m_stopping;
	bool m_searching;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	Move m_bestMove;
	std::jthread m_worker;



//private methods
private:

	//private methods
	std::uint64_t perftRun(int depth, bool white) noexcept;

	void findWhiteSquares(State& state) const noexcept;

	void findBlackSquares(State& state) const noexcept;

	bool makeLegalMove(State& state, bool white, Move move) const noexcept;

	int quiescenceSearch(const State& state, int alpha, int beta, bool white) noexcept;

	int searchRun(const State& state, int depth, int alpha, int beta, bool white) noexcept;

	ScoredMove searchStart(bool white, int depth) noexcept;

	void benchmarkRun(const State& state, std::uint64_t& nodes, std::atomic_bool& stopping, bool white, int depth) noexcept;

	void recordState(Move move) noexcept;



public:

	//constructor
	Engine() noexcept;

	~Engine();


	//getters
	std::string stateFen() const noexcept;

	Move search(bool white) noexcept;

	Move search() noexcept;

	const char* getCharPosition() noexcept;

	int searchMilliseconds() const noexcept;

	bool gameOver() const noexcept;

	Move getLastMove() const noexcept;

	const SearchInfo& searchInfo() const noexcept;



	//setters
	void setState(const State& state) noexcept;

	bool makeMove(int source, int destination) noexcept;

	void setSearchMilliseconds(int milliseconds) noexcept;

	void setStartState() noexcept;

	void engineMove(bool white) noexcept;

	void engineMove() noexcept;

	Move moveBackCallback() noexcept;

	Move moveForwardCallback() noexcept;

	//async
	void startAsyncSearch() noexcept;

	void stopAsyncSearch() noexcept;

	void getAsyncResults() const noexcept;

	bool getAsyncDone(Move& move) noexcept;


	
	//perft
	void perft(int depth) noexcept;

	void benchmark(double seconds) noexcept;
};

