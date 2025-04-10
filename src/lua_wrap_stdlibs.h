#ifndef LUA_WRAP_STDLIBS_H
#define LUA_WRAP_STDLIBS_H

#include "lua_wrap.h"
#include "libs/random.h"
#include "libs/time.h"
#include "libs/vector.h"
#include "libs/tableextension.h"
#include "libs/event.h"

#define LUA_STDLIBS_SDL

#if defined(LUA_STDLIBS_SDL)
#include "libs/sdl.h"
#endif

int luaopen_stdlibs(lua_State* L){
	luaopen_random(L);
	luaopen_time(L);
	luaopen_vector(L);
	luaopen_table_extension(L);
	luaopen_event(L);
	#if defined(LUA_STDLIBS_SDL)
	luaopen_sdl(L);
	#endif
	return 0;
}

#endif