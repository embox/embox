#ifndef LUASOCKET_LUA_H_
#define LUASOCKET_LUA_H_

#include_next <lua.h>

#include <lauxlib.h>
#include <linux/types.h>

typedef luaL_Reg luaL_reg;

#define luaL_putchar(B,c) luaL_addchar(B,c)

static inline int luaL_typerror(lua_State *L, int narg, const char *tname) {
	const char *msg = lua_pushfstring(L, "%s expected, got %s",
			tname, luaL_typename(L, narg));
	return luaL_argerror(L, narg, msg);
}

#endif /* LUASOCKET_LUA_H_ */
