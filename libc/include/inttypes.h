
#pragma once
#ifndef _INTTYPES_H_
#define _INTTYPES_H_

#include <stdint.h>

#define PRId8 "%hhd"
#define PRId16 "%hd"
#define PRId32 "%d"
#define PRId64 "%ld"
#define PRIdMAX "%zd"
#define PRIdPTR "%zd"

#define PRIi8 "%hhi"
#define PRIi16 "%hi"
#define PRIi32 "%i"
#define PRIi64 "%li"
#define PRIiMAX "%zi"
#define PRIiPTR "%zi"

#define PRIu8 "%hhu"
#define PRIu16 "%hu"
#define PRIu32 "%u"
#define PRIu64 "%lu"
#define PRIuMAX "%zu"
#define PRIuPTR "%zu"

#define PRIx8 "%hhx"
#define PRIx16 "%hx"
#define PRIx32 "%x"
#define PRIx64 "%lx"
#define PRIxMAX "%zx"
#define PRIxPTR "%zx"

#define PRIX8 "%hhX"
#define PRIX16 "%hX"
#define PRIX32 "%X"
#define PRIX64 "%lX"
#define PRIXMAX "%zX"
#define PRIXPTR "%zX"

#define PRIo8 "%hho"
#define PRIo16 "%ho"
#define PRIo32 "%o"
#define PRIo64 "%lo"
#define PRIoMAX "%zo"
#define PRIoPTR "%zo"

#endif // _INTTYPES_H_

