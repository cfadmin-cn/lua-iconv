#define LUA_LIB

#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>

#include <errno.h>
#include <iconv.h>

/*
#include <core.h>
#include <iconv.h>
*/

// 开始转换
static inline int convert(lua_State *L, iconv_t cd, const char* text, size_t size) {
  size_t insize = size;
  char *in_buf = lua_newuserdata(L, insize);
  if (!in_buf) {
    iconv_close(cd);
    return luaL_error(L, "Not enough memory.");
  }
  memcpy(in_buf, text, size);

  luaL_Buffer B;
  size_t outsize = size * 3;
  char *out_buf = luaL_buffinitsize(L, &B, outsize);

  char **inbuf = &in_buf;
  char **outbuf = &out_buf;
  int ret = iconv(cd, inbuf, &insize, outbuf, &outsize);
  if (ret == -1){
    iconv_close(cd);
    luaL_pushresultsize(&B, 0);
    return luaL_error(L, strerror(errno));
  }
  iconv_close(cd);
  luaL_pushresultsize(&B, (size * 3) - outsize);
  return 1;
}

// 从其他字符集转换为UTF-8
static int lconvert_from(lua_State *L) {
  errno = 0;
  const char* opcode = luaL_checkstring(L, 1);
  if (!opcode)
    return luaL_error(L, "Invalid conversion character set.");

  size_t size = 0;
  const char* text = luaL_checklstring(L, 2, &size);
  if (!text)
    return luaL_error(L, "Please fill in the text content to be converted.");

  iconv_t cd = iconv_open("UTF-8", opcode);
  if (cd == (iconv_t)-1)
    return luaL_error(L, "Unsupported character set: %s", opcode);

  return convert(L, cd, text, size);
}

// 从UTF-8转换为其他字符集
static int lconvert_to(lua_State *L) {
  errno = 0;
  const char* opcode = luaL_checkstring(L, 1);
  if (!opcode)
    return luaL_error(L, "Invalid conversion character set.");

  size_t size = 0;
  const char* text = luaL_checklstring(L, 2, &size);
  if (!text)
    return luaL_error(L, "Please fill in the text content to be converted.");

  iconv_t cd = iconv_open(opcode, "UTF-8");
  if (cd == (iconv_t)-1)
    return luaL_error(L, "Unsupported character set: %s", opcode);

  return convert(L, cd, text, size);
}

LUAMOD_API int
luaopen_liconv(lua_State *L) {
  luaL_checkversion(L);
  luaL_Reg iconv_libs[] = {
    {"from", lconvert_from},
    {"to", lconvert_to},
    {NULL, NULL},
  };
  luaL_newlib(L, iconv_libs);
  return 1;
}
