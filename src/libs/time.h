#ifndef LUA_Time_H
#define LUA_Time_H

#include <time.h>
#include <stdint.h>

#include "../lua_wrap.h"

#if defined(__unix__)

uint64_t get_ms(){
	struct timespec tp;
	clock_gettime(CLOCK_BOOTTIME,&tp);
	return tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
}

#elif defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

uint64_t get_ms(){
	return GetTickCount64();
}

#else
#error "OS is not supported by this library."
#endif

static int Time_getsecs(lua_State* L){
	lua_pushnumber(L,(double)get_ms()/1000.f);
	return 1;
}

static int Time_getms(lua_State* L){
	lua_pushnumber(L,(double)get_ms());
	return 1;
}

static int Time_sleepms(lua_State* L){
	uint64_t ms = (uint64_t)luaL_checknumber(L,1);
	uint64_t start_time = get_ms();
	while(get_ms()-start_time < ms);
	return 0;
}

static int Time_sleep(lua_State* L){
	uint64_t ms = (uint64_t)(luaL_checknumber(L,1)*1000.f);
	uint64_t start_time = get_ms();
	while(get_ms()-start_time < ms);
	return 0;
}

static struct luaL_Reg timelib[] = {
	{"getsecs",Time_getsecs},
	{"getms",Time_getms},
	{"sleepms",Time_sleepms},
	{"sleep",Time_sleep},
	{NULL,NULL}
};

int luaopen_time(lua_State* L){
	add_clibrary(L,timelib,"Time");
	return 0;
}

#endif