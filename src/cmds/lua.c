/**
 * @file
 * @brief Lua interpreter
 *
 * @date 11.11.11
 * @author Nikolay Korotkiy
 */

#include <embox/cmd.h>

#include <lua.h>
#include <lualib.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	lua_State *L = lua_open();
	luaL_openlibs(L);

	if (luaL_loadfile(L, argv[1])) {
		return -1;
	}

	lua_call(L, 0, LUA_MULTRET);

	lua_close(L);
	return 0;
}
