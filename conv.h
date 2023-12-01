/*
**  LICENSE: BSD
**  Author: CandyMi[https://github.com/candymi]
*/

#include <core.h>

#if defined(__APPLE__)
  #define USE_ICU
  #define _LIBICONV_VERSION 0x010B
  #include <unicode/uchar.h>
  #include <unicode/urename.h>
  int ucnv_convert(const char *to, const char *from, char *target, int targetCapacity, const char *source, int sourceLength, UErrorCode *err);
#else
  #define USE_ICONV

  #if defined(__linux__)
    #define LIBICONV_PLUG
  #endif

  #include <iconv.h>

  #ifndef _LIBICONV_VERSION
    #define _LIBICONV_VERSION (1 << 8)
  #endif

  #define CONVERT_TO (0)
  #define CONVERT_FROM (1)

  #define xrio_Buffer      luaL_Buffer
  #define xrio_buffinit    luaL_buffinit
  #define xrio_pushresult  luaL_pushresult
  #define xrio_addlstring  luaL_addlstring
  #define xrio_reset(B)
#endif

#define xrio_max_block (4096)

/* 为lib注入 key -> number 数据 */
#define luaL_setkn(L, k, n) ({ lua_pushstring(L, k); lua_pushnumber(L, n) ; lua_rawset(L, -3); })

/* 为lib注入 key -> string 数据 */
#define luaL_setkv(L, k, v) ({ lua_pushstring(L, k); lua_pushstring(L, v) ; lua_rawset(L, -3); })