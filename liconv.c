#define LUA_LIB

#include <core.h>
#include <iconv.h>

#ifndef _LIBICONV_VERSION
  #define _LIBICONV_VERSION (1 << 8)
#endif

#define CONVERT_TO (0)
#define CONVERT_FROM (1)

/* 为lib注入 key -> number 数据 */
#define luaL_setkn(L, k, n) ({ lua_pushstring(L, k); lua_pushnumber(L, n) ; lua_rawset(L, -3); })

/* 为lib注入 key -> string 数据 */
#define luaL_setkv(L, k, v) ({ lua_pushstring(L, k); lua_pushstring(L, v) ; lua_rawset(L, -3); })

/* 获取libiconv 大、小版本号 */
static inline void luaL_geticonv(lua_Integer* max, lua_Integer* min) {
  if (max)
    *max = _LIBICONV_VERSION >> 8;
  if (min)
    *min = (_LIBICONV_VERSION - ((_LIBICONV_VERSION >> 8) << 8));
}

/* 实际转换代码 */
static inline int iconv_convert(lua_State *L, iconv_t cd, const char* text, size_t size) {
  luaL_Buffer B;
  size_t outsize = size * 4;
  char *out_buf = luaL_buffinitsize(L, &B, outsize);

  size_t insize = size;
  char **inbuf = (char**)&text;
  char **outbuf = (char**)&out_buf;
  if (iconv(cd, inbuf, &insize, outbuf, &outsize) == -1){
    iconv_close(cd);
    lua_pushboolean(L, 0);
    lua_pushfstring(L, "[ICONV ERROR]: %s. (%d)", strerror(errno), errno);
    return 2;
  }
  // 转换缓存
  iconv_close(cd);
  luaL_pushresultsize(&B, (size * 4) - outsize);
  return 1;
}

/* 增加自动判断 */ 
static inline iconv_t new_iconv(uint8_t mode, const char* opcode) {
  if (mode == CONVERT_FROM) 
    return iconv_open("UTF-8", opcode);

  if (mode == CONVERT_TO)
    return iconv_open(opcode, "UTF-8");

  return (iconv_t)-1;
}

// 开始转换
static inline int convert(lua_State *L, uint8_t mode, const char* opcode, const char* text, size_t size) {
  iconv_t cd = new_iconv(mode, opcode);
  if (cd == (iconv_t)-1)
    return luaL_error(L, "[ICONV ERROR]: Unsupported characterset (%s).", opcode);

  return iconv_convert(L, cd, text, size);
}

static int lconvert_convert(lua_State *L) {
  const char* to = luaL_checkstring(L, 1);
  if (!to)
    return luaL_error(L, "[ICONV ERROR]: Invalid conversion character set. #1");
  const char* from = luaL_checkstring(L, 2);
  if (!from)
    return luaL_error(L, "[ICONV ERROR]: Invalid conversion character set. #2");

  size_t size = 0;
  const char* text = luaL_checklstring(L, 3, &size);
  if (!text)
    return luaL_error(L, "[ICONV ERROR]: Please fill in the text content to be converted."); 
  
  iconv_t cd = iconv_open(to, from);
  if (cd == (iconv_t)-1)
    return luaL_error(L, "[ICONV ERROR]: Unsupported characterset (%s, %s).", to, from);
  
  return iconv_convert(L, cd, text, size);
}


// 从其他字符集转换为UTF-8
static int lconvert_from(lua_State *L) {
  errno = 0;
  const char* opcode = luaL_checkstring(L, 1);
  if (!opcode)
    return luaL_error(L, "[ICONV ERROR]: Invalid conversion character set.");

  size_t size = 0;
  const char* text = luaL_checklstring(L, 2, &size);
  if (!text)
    return luaL_error(L, "[ICONV ERROR]: Please fill in the text content to be converted.");

  return convert(L, CONVERT_FROM, opcode, text, size);
}

// 从UTF-8转换为其他字符集
static int lconvert_to(lua_State *L) {
  errno = 0;
  const char* opcode = luaL_checkstring(L, 1);
  if (!opcode)
    return luaL_error(L, "[ICONV ERROR]: Invalid conversion character set.");

  size_t size = 0;
  const char* text = luaL_checklstring(L, 2, &size);
  if (!text)
    return luaL_error(L, "[ICONV ERROR]: Please fill in the text content to be converted.");

  return convert(L, CONVERT_TO, opcode, text, size);
}

//  初始化内置库
static inline void luaL_add_iconv_version(lua_State *L) {
  /* 根据情况拿到大、小版本号 */
  lua_Integer max = 0; lua_Integer min = 0;
  luaL_geticonv(&max, &min);
  /* 导入libiconv版本号 */
  luaL_setkn(L, "LIBICONV_VERSION", max + /* 如果小版本是三位数 */ ( min > 99 ? min * 1e-3 : min * 1e-2 ));
  /* 导入lua-iconv版本号 */
  luaL_setkn(L, "VERSION", 0.1);
}

LUAMOD_API int
luaopen_liconv(lua_State *L) {
  luaL_checkversion(L);
  luaL_Reg iconv_libs[] = {
    { "convert", lconvert_convert },
    { "from", lconvert_from },
    { "to", lconvert_to },
    {NULL, NULL},
  };
  luaL_newlib(L, iconv_libs);
  luaL_add_iconv_version(L);
  return 1;
}
