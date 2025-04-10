#ifndef LUA_WRAP_LIB_H
#define LUA_WRAP_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define prog_error(l,fmt,...) ({printf((fmt),##__VA_ARGS__);lua_close((l));exit(EXIT_FAILURE);})

void add_cfunction(lua_State* L, int (*func)(lua_State*), char* name){
	lua_pushcfunction(L,func);
	lua_setglobal(L,name);
}

void add_metatable(lua_State* L, const char* name){
	luaL_newmetatable(L,name);
}

#define pop_metatable(l) lua_pop(l,1)

void assign_metamethods(lua_State* L, struct luaL_Reg* lib){
	luaL_setfuncs(L,lib,0);
}

void get_metatable(lua_State* L, const char* name){
	luaL_getmetatable(L,name);
}

void set_metatable(lua_State* L, const char* name){
	get_metatable(L,name);
	lua_setmetatable(L,-2);
}

void* check_metatable(lua_State* L, int stack_index, const char* name, const char* error_msg){
	void* ud = luaL_checkudata(L, stack_index, name);
	luaL_argcheck(L,ud != NULL, stack_index, error_msg);
	return ud;
}

void add_clibrary(lua_State* L, struct luaL_Reg* lib, char* name){
	lua_getglobal(L,name);
	if(lua_isnil(L,-1)){
		lua_pop(L,1);
		lua_newtable(L);
	}
	luaL_setfuncs(L,lib,0);
	lua_setglobal(L,name);
}

lua_State* open_lua_program(char* filepath){
	int s = 0;
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	if(luaL_loadfile(L,filepath))
		prog_error(L,"\e[38;5;196mFailed to parse file 💔. Ts pmo icl 💔.\n\e[0m%s\n",lua_tostring(L,-1));
	return L;
}

void run_lua_program(lua_State* L){
	if(lua_pcall(L,0,0,0))
		prog_error(L,"\e[38;5;196mts pmo 💔:\n\e[0m%s\n",lua_tostring(L,-1));
}

#endif