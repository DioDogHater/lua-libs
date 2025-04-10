#ifndef LUA_TableExtension_H
#define LUA_TableExtension_H

#include "../lua_wrap.h"

// Extends from the basic lua table API
// Adds:
// - table.find(t,v)
// - table.remove_value(t,v)
// - table.remove_all(t,v)
// - table.print(t)
// - table.print_list(t)

static int Table_find(lua_State* L){
	luaL_argcheck(L,lua_istable(L,1),1,"'table' expected");
	luaL_argcheck(L,!lua_isnoneornil(L,2),2,"non 'nil' value expected");
	lua_pushnil(L);
	while(lua_next(L,1) != 0){
		if(lua_compare(L,-1,2,LUA_OPEQ)){
			lua_pop(L,1);
			return 1;
		}
    	lua_pop(L, 1);
	}
	lua_pushnil(L);
	return 1;
}

static int Table_remove_value(lua_State* L){
	luaL_argcheck(L,lua_istable(L,1),1,"'table' expected");
	luaL_argcheck(L,!lua_isnoneornil(L,2),2,"non 'nil' value expected");
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

static int Table_remove_all(lua_State* L){
	luaL_argcheck(L,lua_istable(L,1),1,"'table' expected");
	luaL_argcheck(L,!lua_isnoneornil(L,2),2,"non 'nil' value expected");
	lua_pushnil(L);
	while(lua_next(L,1) != 0){
		if(lua_compare(L,-1,2,LUA_OPEQ)){
			lua_copy(L,-2,-1);
			lua_pushnil(L);
			lua_settable(L,1);
		}else
    		lua_pop(L, 1);
	}
	return 0;
}

static void Table_print_value(lua_State* L, int index){
	int val_type = lua_type(L,index);
	switch(val_type){
	case LUA_TNUMBER:
		printf("%g",lua_tonumber(L,index));
		return;
	case LUA_TSTRING:
		printf("\"%s\"",lua_tostring(L,index));
		return;
	case LUA_TBOOLEAN:
		printf("%s",(lua_toboolean(L,index))?"true":"false");
		return;
	default:
		printf("%s",lua_typename(L,val_type));
	}
}

static int Table_print(lua_State* L){
	luaL_argcheck(L,lua_istable(L,1),1,"'table' expected");
	lua_pushnil(L);
	size_t i = 0;
	putchar('{');
	while(lua_next(L,1) != 0){
		if(i > 0) printf(", ");
		Table_print_value(L,-2);
		printf(" = ");
		Table_print_value(L,-1);
		lua_pop(L,1);
		i++;
	}
	printf("}\n");
	return 0;
}

static int Table_print_list(lua_State* L){
	luaL_argcheck(L,lua_istable(L,1),1,"'table' expected");
	lua_pushnil(L);
	size_t i = 0;
	putchar('{');
	while(lua_next(L,1) != 0){
		if(i > 0) printf(", ");
		Table_print_value(L,-1);
		lua_pop(L,1);
		i++;
	}
	printf("}\n");
	return 0;
}

static struct luaL_Reg table_extlib[] = {
	{"find",Table_find},
	{"remove_value",Table_remove_value},
	{"remove_all",Table_remove_all},
	{"print",Table_print},
	{"print_list",Table_print_list},
	{NULL,NULL}
};

int luaopen_table_extension(lua_State* L){
	lua_getglobal(L,"table");
	assign_metamethods(L,table_extlib);
	pop_metatable(L);
	return 0;
}

#endif