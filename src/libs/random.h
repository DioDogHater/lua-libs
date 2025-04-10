#ifndef LUA_Random_H
#define LUA_Random_H

#include <stdlib.h> // rand(), srand()
#include <time.h>	// time(NULL)

#include "../lua_wrap.h"

static int Random_randint(lua_State* L){
	int min = (int)luaL_checknumber(L,1);
	int max = (int)luaL_checknumber(L,2);
	lua_pushnumber(L,(double)(rand()%(max-min+1)+min));
	return 1;
}

static int Random_randrange(lua_State* L){
	double min = luaL_checknumber(L,1);
	double max = luaL_checknumber(L,2);
	lua_pushnumber(L,(double)rand()/(double)RAND_MAX*(max-min)+min);
	return 1;
}

static int Random_random(lua_State* L){
	lua_pushnumber(L,(double)rand()/(double)RAND_MAX);
	return 1;
}

static int Random_rand(lua_State* L){
	lua_pushnumber(L,(double)rand());
	return 1;
}

static int Random_randseed(lua_State* L){
	int seed = (int)luaL_checknumber(L,1);
	srand(seed);
	return 0;
}

static struct luaL_Reg randomlib[] = {
	{"randint",Random_randint},
	{"randrange",Random_randrange},
	{"random",Random_random},
	{"rand",Random_rand},
	{"seed",Random_randseed},
	{NULL,NULL}
};

int luaopen_random(lua_State* L){
	srand(time(NULL));
	add_clibrary(L,randomlib,"Random");
	return 0;
}

#endif