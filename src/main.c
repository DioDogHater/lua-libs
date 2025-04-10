#include "lua_wrap_stdlibs.h"

int main(void){
	lua_State* L = open_lua_program("test.lua");
	luaopen_stdlibs(L);
	run_lua_program(L);
	lua_close(L);
	return 0;
}
