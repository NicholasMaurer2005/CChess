#pragma once

#ifndef CCHESS_INCLUDE
#define CCHESS_INCLUDE

#ifdef __cplusplus

//noexcept define
#define CCHESS_NOEXCEPT		noexcept
#define CCHESS_NODISCARD	[[nodiscard]]

extern "C" {
#endif

#define CCHESS_BOOL		int
#define CCHESS_TRUE		1
#define CCHESS_FALSE	0

	CCHESS_BOOL engine_create() CCHESS_NOEXCEPT;

	void engine_destroy() CCHESS_NOEXCEPT;

	void engine_position_start() CCHESS_NOEXCEPT;

	CCHESS_BOOL engine_position_fen(const char* fen) CCHESS_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif