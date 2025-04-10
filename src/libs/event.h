#ifndef LUA_Event_H
#define LUA_Event_H

#include "../lua_wrap.h"

// Event -> table
// {(function) f1, f2, f3 ...}
// Event.new() -> Event(e)
// e:connect(f) -> table.insert(e,f)
// e:disconnect(f) -> table.remove_value(e,f)
// e:trigger(args...) -> for _,v in ipairs(e) do v(args...) end
// e:clear() -> Event.new()

static const char* EVENT_METATABLE = "stdlibs.Event";

static void check_Event(lua_State* L){
	luaL_argcheck(L,!lua_isnil(L,1) && lua_istable(L,1),1,"'Event' expected");
	get_metatable(L,EVENT_METATABLE);
	lua_getmetatable(L,1);
	luaL_argcheck(L,!lua_isnil(L,-1) && lua_compare(L,-1,-2,LUA_OPEQ),1,"'Event' expected");
	lua_pop(L,2);
}

static int Event_new(lua_State* L){
	lua_newtable(L);
	set_metatable(L,EVENT_METATABLE);
	return 1;
}

static int Event_connect(lua_State* L){
	check_Event(L);
	luaL_argcheck(L,lua_isfunction(L,2),2,"'function' expected");
	lua_len(L,1);
	int old_len = (int)lua_tonumber(L,-1);
	lua_pushvalue(L,2);
	lua_rawseti(L,1,old_len+1);
	return 0;
}

static int Event_disconnect(lua_State* L){
	check_Event(L);
	luaL_argcheck(L,lua_isfunction(L,2),2,"'function' expected");
	lua_pushnil(L);
	while(lua_next(L,1) != 0){
		if(lua_compare(L,-1,2,LUA_OPEQ)){
			lua_pop(L,1);
			lua_pushnil(L);
			lua_settable(L,1);
			return 0;
		}
    	lua_pop(L, 1);
	}
	return 0;
}

static int Event_trigger(lua_State* L){
	check_Event(L);
	int stack_size = lua_gettop(L);
	lua_pushnil(L);
	while(lua_next(L,1) != 0){
		// Right now, function should be on top
		for(int i = 2; i <= stack_size; i++){
			// Push each argument
			lua_pushvalue(L,i);
		}
		// We call the function
		lua_call(L,stack_size-1,0);
	}
	return 0;
}

static int Event_clear(lua_State* L){
	check_Event(L);
	lua_pushnil(L);
	while(lua_next(L,1) != 0){
		lua_copy(L,-2,-1);
		lua_pushnil(L);
		lua_settable(L,1);
	}
	return 0;
}

static int Event_tostring(lua_State* L){
	check_Event(L);
	lua_len(L,1);
	int callback_count = (int)lua_tonumber(L,-1);
	lua_pushfstring(L,"Event(%d callbacks)",callback_count);
	return 1;
}

static struct luaL_Reg eventlib[] = {
	{"new",Event_new},
	{NULL,NULL}
};

static struct luaL_Reg eventlib_m[] = {
	{"connect",Event_connect},
	{"disconnect",Event_disconnect},
	{"trigger",Event_trigger},
	{"clear",Event_clear},
	{"__tostring",Event_tostring},
	{NULL,NULL}
};

int luaopen_event(lua_State* L){
	add_metatable(L,EVENT_METATABLE);
	lua_pushstring(L,"__index");
	lua_pushvalue(L,-2);
	lua_settable(L,-3);
	assign_metamethods(L,eventlib_m);
	pop_metatable(L);
	add_clibrary(L,eventlib,"Event");
	return 0;
}

#endif