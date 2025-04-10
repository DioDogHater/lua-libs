#ifndef LUA_SDL_Implementation_H
#define LUA_SDL_Implementation_H

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#undef main

#include "../lua_wrap.h"

/*

SDL.init(flags...) -> nil:
	initializes the SDL library
	if there are multiple flags, they will be OR'ed together

SDL.create_window(title,dimensions) -> SDL.Window:
	dimensions must be {x=..., y=..., w=..., h=...}, where x, y, w and h are in pixels
	renderer will be initialized with SDL_RENDERER_ACCELERATED as default flag

SDL.poll_events(window) -> EventIterator (function)
	returns iterator that will give all events since last frame

SDL.quit() -> nil:
	quits SDL

SDL.color(r,g,b,a) -> table ({r=...,g=...,b=...})
	all args are optional, r,g,b have default 0, but a has default 255

SDL.INIT_TIMER
SDL.INIT_AUDIO
SDL.INIT_VIDEO
SDL.INIT_JOYSTICK
SDL.INIT_HAPTIC
SDL.INIT_GAMECONTROLLER
SDL.INIT_EVENTS
SDL.INIT_EVERYTHING

SDL.AUDIODEVICEADDED
SDL.AUDIODEVICEREMOVED
SDL.CONTROLLERBUTTONDOWN
SDL.CONTROLLERBUTTONUP
SDL.CONTROLLERDEVICEADDED
SDL.CONTROLLERDEVICEREMOVED
SDL.CONTROLLERDEVICEREMAPPED
SDL.DROPFILE
SDL.DROPTEXT
SDL.DROPBEGIN
SDL.DROPCOMPLETE
SDL.JOYAXISMOTION
SDL.JOYBALLMOTION
SDL.JOYHATMOTION
SDL.KEYDOWN
SDL.KEYUP
SDL.MOUSEBUTTONDOWN
SDL.MOUSEBUTTONUP
SDL.MOUSEMOTION
SDL.MOUSEWHEEL
SDL.WINDOWEVENT
SDL.QUIT

SDL.key.UP/DOWN
SDL.key.LEFT/RIGHT
SDL.key.ESCAPE
SDL.key.LSHIFT/RSHIFT
SDL.key.LCTRL/RCTRL
SDL.key.LALT/RALT
SDL.key.TAB
SDL.key.RETURN
SDL.key.PAUSE
SDL.key.ENTER
SDL.key.BACKSPACE
SDL.key.DELETE
SDL.key.INSERT
SDL.key.PAGEUP
SDL.key.PAGEDOWN
SDL.key.HOME
SDL.key.CAPSLOCK
SDL.key.F[1->12]
SDL.key.KEYPAD_[0->9]
SDL.key.KEYPAD_PLUS/MINUS
SDL.key.KEYPAD_DIVIDE/MULTIPLY
SDL.key.KEYPAD_PERIOD/EQUALS
SDL.key.KEYPAD_ENTER

Userdatum:
	- SDL.Window -> full userdata, contains SDL_Window*, SDL_Renderer* pointers + window size

SDL.Window:

:is_active()
	returns true if active, false if closed

:close()
	closes window using SDL.close_window(), garbage collector also calls this function

:clear(color)
	clear the window's screen (color is a {r=...,g=...,b=...} table)

:update()
	update the window's screen

:get_size()
	returns size as a {w=...,h=...} table

:set_size(size)
	sets the window size to size (size is a {w=...,h=...} table)

:draw_rect(rect,color,filled)
	draws rect with color (rect is a {x=...,y=...,w=...,h=...} table and color is a {r=...,g=...,b=...} table)
	filled defaults to true, if false, will only draw outline

:draw_line(a,b,color)
	draws line between point a and b with color (a and b are {x=...,y=...} tables, color is a {r=...,g=...,b=...} table)

*/

static const char* SDL_WINDOW_METATABLE = "SDL.Window";

// Window userdata
struct LUA_SDL_Window {
	SDL_Window* window;
	SDL_Renderer* renderer;
	int width, height;
};

// Check if 1st arg is LUA_SDL_Window
static struct LUA_SDL_Window* check_Window(lua_State* L){
	return (struct LUA_SDL_Window*) check_metatable(L,1,"SDL.Window","'SDL.Window' expected");
}

static struct LUA_SDL_Window* create_Window(lua_State* L){
	struct LUA_SDL_Window* r = (struct LUA_SDL_Window*) lua_newuserdata(L,sizeof(struct LUA_SDL_Window));
	set_metatable(L,SDL_WINDOW_METATABLE);
	return r;
}

#define LUA_SDL_int_field(k,i) ({lua_getfield(L,(i),k);luaL_argcheck(L,!lua_isnil(L,-1) && lua_isnumber(L,-1),(i),"'"k"' field expected");})

static SDL_Color check_sdl_color(lua_State* L, int index){
	luaL_argcheck(L,lua_istable(L,index),index,"'table' expected");
	SDL_Color r;
	LUA_SDL_int_field("r",index);
	r.r = lua_tointeger(L,-1);
	lua_pop(L,1);
	LUA_SDL_int_field("g",index);
	r.g = lua_tointeger(L,-1);
	lua_pop(L,1);
	LUA_SDL_int_field("b",index);
	r.b = lua_tointeger(L,-1);
	lua_pop(L,1);
	lua_getfield(L,index,"a");
	if(!lua_isnil(L,-1) && lua_isinteger(L,-1)) r.a = lua_tointeger(L,-1);
	else r.a = 255;
	lua_pop(L,1);
	return r;
}

static int sdl_x_default = 0;
static int sdl_y_default = 0;
static SDL_Rect check_sdl_rect(lua_State* L, int index){
	luaL_argcheck(L,lua_istable(L,index),index,"'table' expected");
	SDL_Rect r;
	lua_getfield(L,index,"x");
	if(!lua_isnil(L,-1) && lua_isinteger(L,-1)) r.x = lua_tointeger(L,-1);
	else r.x = sdl_x_default;
	lua_pop(L,1);
	lua_getfield(L,index,"y");
	if(!lua_isnil(L,-1) && lua_isinteger(L,-1)) r.y = lua_tointeger(L,-1);
	else r.y = sdl_y_default;
	lua_pop(L,1);
	LUA_SDL_int_field("w",index);
	r.w = lua_tointeger(L,-1);
	lua_pop(L,1);
	LUA_SDL_int_field("h",index);
	r.h = lua_tointeger(L,-1);
	lua_pop(L,1);
	return r;
}

static SDL_Point check_sdl_point(lua_State* L, int index){
	luaL_argcheck(L,lua_istable(L,index),index,"'table' expected");
	SDL_Point r;
	LUA_SDL_int_field("x",index);
	r.x = lua_tointeger(L,-1);
	lua_pop(L,1);
	LUA_SDL_int_field("y",index);
	r.y = lua_tointeger(L,-1);
	lua_pop(L,1);
	return r;
}

// SDL.init
static int SDL_init(lua_State* L){
	int stack_size = lua_gettop(L);
	Uint32 flags;
	if(stack_size == 0) flags = SDL_INIT_EVERYTHING;
	else{
		for(int i = 1; i <= stack_size; i++){
			Uint32 flag = (Uint32)luaL_checkinteger(L,i);
			flags |= flag;
		}
	}
	if(SDL_Init(flags) < 0)
		luaL_error(L,"SDL.init() error: %s",SDL_GetError());
	return 0;
}

// SDL.quit
static int SDL_quit(lua_State* L){
	luaL_argcheck(L,lua_gettop(L) == 0,0,"expected no arguments");
	SDL_Quit();
	return 0;
}

// EventIterator (function)
static int SDL_event_iterator(lua_State* L){
	SDL_Event e;
	if(SDL_PollEvent(&e) == 0){
		lua_pushnil(L);
		return 1;
	}
	// Event.type
	lua_newtable(L);
	lua_pushnumber(L,(double)e.type);
	lua_setfield(L,-2,"type");

	// Event.* (except Event.type)
	switch(e.type){
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		if(e.key.keysym.sym >= 32 && e.key.keysym.sym <= 126)
			lua_pushfstring(L,"%c",e.key.keysym.sym);
		else
			lua_pushnumber(L,(double)e.key.keysym.sym);
		lua_setfield(L,-2,"key");
		break;
	}
	return 1;
}

// SDL.poll_events()
static int SDL_poll_events(lua_State* L){
	lua_pushcfunction(L,SDL_event_iterator);
	lua_pushnil(L);
	lua_pushnil(L);
	return 3;
}

// SDL.create_window
static int SDL_create_window(lua_State* L){
	const char* caption = luaL_checkstring(L,1);
	sdl_x_default = SDL_WINDOWPOS_UNDEFINED;
	sdl_y_default = SDL_WINDOWPOS_UNDEFINED;
	SDL_Rect rect = check_sdl_rect(L,2);
	sdl_x_default = 0;
	sdl_y_default = 0;
	struct LUA_SDL_Window* window = create_Window(L);
	window->window = SDL_CreateWindow(caption,rect.x,rect.y,rect.w,rect.h,SDL_WINDOW_SHOWN);
	if(window->window == NULL)
		luaL_error(L,"SDL.create_window() error (creating the window): %s",SDL_GetError());
	window->renderer = SDL_CreateRenderer(window->window,-1,SDL_RENDERER_ACCELERATED);
	if(window->renderer == NULL)
		luaL_error(L,"SDL.create_window() error (creating the renderer): %s",SDL_GetError());
	window->width = rect.w;
	window->height = rect.h;
	return 1;
}

// SDL.color()
static int SDL_color(lua_State* L){
	int stack_size = lua_gettop(L);
	unsigned int r=0, g=0, b=0, a=255;
	switch(stack_size){
	case 1:
		r = luaL_checkinteger(L,1);
		g = r; b = r;
		break;
	case 4:
		a = luaL_checkinteger(L,4);
	case 3:
		r = luaL_checkinteger(L,1);
		g = luaL_checkinteger(L,2);
		b = luaL_checkinteger(L,3);
	case 0:
		break;
	default:
		luaL_argcheck(L,0,stack_size,"expected 0, 1, 3 or 4 arguments");
	}
	lua_createtable(L,0,3);
	lua_pushnumber(L,r);
	lua_setfield(L,-2,"r");
	lua_pushnumber(L,g);
	lua_setfield(L,-2,"g");
	lua_pushnumber(L,b);
	lua_setfield(L,-2,"b");
	lua_pushnumber(L,a);
	lua_setfield(L,-2,"a");
	return 1;
}

// SDL.Window:close()
static int SDL_window_close(lua_State* L){
	struct LUA_SDL_Window* window = check_Window(L);
	if(window->renderer != NULL) SDL_DestroyRenderer(window->renderer);
	window->renderer = NULL;
	if(window->window != NULL) SDL_DestroyWindow(window->window);
	window->window = NULL;
	window->width = 0;
	window->height = 0;
	return 0;
}

// SDL.Window:clear()
static int SDL_window_clear(lua_State* L){
	struct LUA_SDL_Window* window = check_Window(L);
	if(window->window == NULL) return 0;
	SDL_Color color = (SDL_Color){0,0,0,0};
	if(lua_gettop(L) > 1)
		color = check_sdl_color(L,2);
	SDL_SetRenderDrawColor(window->renderer,color.r,color.g,color.b,color.a);
	SDL_RenderClear(window->renderer);
	return 0;
}

// SDL.Window:update()
static int SDL_window_update(lua_State* L){
	struct LUA_SDL_Window* window = check_Window(L);
	if(window->window == NULL) return 0;
	SDL_RenderPresent(window->renderer);
	return 0;
}

// SDL.Window:get_size()
static int SDL_window_get_size(lua_State* L){
	struct LUA_SDL_Window* window = check_Window(L);
	lua_createtable(L,0,2);
	lua_pushnumber(L,window->width);
	lua_setfield(L,-2,"w");
	lua_pushnumber(L,window->height);
	lua_setfield(L,-2,"h");
	return 1;
}

// SDL.Window:set_size()
static int SDL_window_set_size(lua_State* L){
	struct LUA_SDL_Window* window = check_Window(L);
	luaL_argcheck(L,lua_istable(L,2),2,"'table' expected");
	if(window->window == NULL) return 0;
	LUA_SDL_int_field("w",2);
	window->width = lua_tointeger(L,-1);
	lua_pop(L,1);
	LUA_SDL_int_field("h",2);
	window->height = lua_tointeger(L,-1);
	lua_pop(L,1);
	SDL_SetWindowSize(window->window,window->width,window->height);
	return 0;
}

// SDL.Window:draw_rect()
static int SDL_window_draw_rect(lua_State* L){
	struct LUA_SDL_Window* window = check_Window(L);
	SDL_Rect rect = check_sdl_rect(L,2);
	SDL_Color color = check_sdl_color(L,3);
	int filled = 1;
	if(!lua_isnoneornil(L,4) && lua_isboolean(L,4)) filled = lua_toboolean(L,4);
	if(window->window == NULL) return 0;
	SDL_SetRenderDrawColor(window->renderer,color.r,color.g,color.b,color.a);
	if(filled) SDL_RenderFillRect(window->renderer,&rect);
	else SDL_RenderDrawRect(window->renderer,&rect);
	return 0;
}

// SDL.Window:draw_line()
static int SDL_window_draw_line(lua_State* L){
	struct LUA_SDL_Window* window = check_Window(L);
	SDL_Point a = check_sdl_point(L,2);
	SDL_Point b = check_sdl_point(L,3);
	SDL_Color color = check_sdl_color(L,4);
	if(window->window == NULL) return 0;
	SDL_SetRenderDrawColor(window->renderer,color.r,color.g,color.b,color.a);
	SDL_RenderDrawLine(window->renderer,a.x,a.y,b.x,b.y);
	return 0;
}

// SDL.Window:is_active()
static int SDL_window_is_active(lua_State* L){
	struct LUA_SDL_Window* window = check_Window(L);
	lua_pushboolean(L,window->window != NULL);
	return 1;
}

// Convert a SDL.Window into string
static int SDL_window_tostring(lua_State* L){
	struct LUA_SDL_Window* window = check_Window(L);
	lua_pushfstring(L,"SDL.Window(%s, %d x %d)",(window->window!=NULL)?"active":"closed",window->width,window->height);
	return 1;
}

// SDL global functions
static struct luaL_Reg sdllib[] = {
	{"init",SDL_init},
	{"quit",SDL_quit},
	{"create_window",SDL_create_window},
	{"poll_events",SDL_poll_events},
	{"color",SDL_color},
	{NULL,NULL}
};

// SDL.Window metamethods
static struct luaL_Reg sdlwindowlib_m[] = {
	{"close",SDL_window_close},
	{"clear",SDL_window_clear},
	{"update",SDL_window_update},
	{"is_active",SDL_window_is_active},
	{"get_size",SDL_window_get_size},
	{"set_size",SDL_window_set_size},
	{"draw_rect",SDL_window_draw_rect},
	{"draw_line",SDL_window_draw_line},
	{"__gc",SDL_window_close},
	{"__tostring",SDL_window_tostring},
	{NULL,NULL}
};

#define LUA_SDL_ENUM(n) {#n,SDL_##n}

// SDL.* (Enums)
static struct{
	const char* key;
	Uint32 value;
} sdl_enums[] = {
LUA_SDL_ENUM(INIT_TIMER),
LUA_SDL_ENUM(INIT_AUDIO),
LUA_SDL_ENUM(INIT_VIDEO),
LUA_SDL_ENUM(INIT_JOYSTICK),
LUA_SDL_ENUM(INIT_HAPTIC),
LUA_SDL_ENUM(INIT_GAMECONTROLLER),
LUA_SDL_ENUM(INIT_EVENTS),
LUA_SDL_ENUM(INIT_EVERYTHING),
LUA_SDL_ENUM(AUDIODEVICEADDED),
LUA_SDL_ENUM(AUDIODEVICEREMOVED),
LUA_SDL_ENUM(CONTROLLERBUTTONDOWN),
LUA_SDL_ENUM(CONTROLLERBUTTONUP),
LUA_SDL_ENUM(CONTROLLERDEVICEADDED),
LUA_SDL_ENUM(CONTROLLERDEVICEREMOVED),
LUA_SDL_ENUM(CONTROLLERDEVICEREMAPPED),
LUA_SDL_ENUM(DROPFILE),
LUA_SDL_ENUM(DROPTEXT),
LUA_SDL_ENUM(DROPBEGIN),
LUA_SDL_ENUM(DROPCOMPLETE),
LUA_SDL_ENUM(JOYAXISMOTION),
LUA_SDL_ENUM(JOYBALLMOTION),
LUA_SDL_ENUM(JOYHATMOTION),
LUA_SDL_ENUM(KEYDOWN),
LUA_SDL_ENUM(KEYUP),
LUA_SDL_ENUM(MOUSEBUTTONDOWN),
LUA_SDL_ENUM(MOUSEBUTTONUP),
LUA_SDL_ENUM(MOUSEMOTION),
LUA_SDL_ENUM(MOUSEWHEEL),
LUA_SDL_ENUM(WINDOWEVENT),
LUA_SDL_ENUM(QUIT),
{NULL,0}
};

#define LUA_SDLK_ENUM(n) {#n,SDLK_##n}
#define LUA_SDLKP_ENUM(n) {"KEYPAD_"#n,SDLK_KP_##n}

static struct{
	const char* key;
	Uint32 value;
} sdl_key_enums[] = {
LUA_SDLK_ENUM(UP),
LUA_SDLK_ENUM(DOWN),
LUA_SDLK_ENUM(LEFT),
LUA_SDLK_ENUM(RIGHT),
LUA_SDLK_ENUM(ESCAPE),
LUA_SDLK_ENUM(LSHIFT),
LUA_SDLK_ENUM(RSHIFT),
LUA_SDLK_ENUM(LCTRL),
LUA_SDLK_ENUM(RCTRL),
LUA_SDLK_ENUM(LALT),
LUA_SDLK_ENUM(RALT),
LUA_SDLK_ENUM(TAB),
LUA_SDLK_ENUM(RETURN),
LUA_SDLK_ENUM(PAUSE),
LUA_SDLK_ENUM(BACKSPACE),
LUA_SDLK_ENUM(DELETE),
LUA_SDLK_ENUM(INSERT),
LUA_SDLK_ENUM(PAGEUP),
LUA_SDLK_ENUM(PAGEDOWN),
LUA_SDLK_ENUM(HOME),
LUA_SDLK_ENUM(CAPSLOCK),
LUA_SDLK_ENUM(F1),
LUA_SDLK_ENUM(F2),
LUA_SDLK_ENUM(F3),
LUA_SDLK_ENUM(F4),
LUA_SDLK_ENUM(F5),
LUA_SDLK_ENUM(F6),
LUA_SDLK_ENUM(F7),
LUA_SDLK_ENUM(F8),
LUA_SDLK_ENUM(F9),
LUA_SDLK_ENUM(F10),
LUA_SDLK_ENUM(F11),
LUA_SDLK_ENUM(F12),
LUA_SDLKP_ENUM(0),
LUA_SDLKP_ENUM(1),
LUA_SDLKP_ENUM(2),
LUA_SDLKP_ENUM(3),
LUA_SDLKP_ENUM(4),
LUA_SDLKP_ENUM(5),
LUA_SDLKP_ENUM(6),
LUA_SDLKP_ENUM(7),
LUA_SDLKP_ENUM(8),
LUA_SDLKP_ENUM(9),
LUA_SDLKP_ENUM(PERIOD),
LUA_SDLKP_ENUM(DIVIDE),
LUA_SDLKP_ENUM(MULTIPLY),
LUA_SDLKP_ENUM(MINUS),
LUA_SDLKP_ENUM(PLUS),
LUA_SDLKP_ENUM(ENTER),
LUA_SDLKP_ENUM(EQUALS),
{NULL,0}
};

static void add_sdl_enums(lua_State* L){
	int i = 0;
	while(sdl_enums[i].key != NULL){
		lua_pushnumber(L,sdl_enums[i].value);
		lua_setfield(L,-2,sdl_enums[i].key);
		i++;
	}
}

static void add_sdl_key_enums(lua_State* L){
	int key_len = sizeof(sdl_key_enums)/(sizeof(const char*)+sizeof(Uint32))-1;
	lua_createtable(L,0,key_len);
	int i = 0;
	while(sdl_key_enums[i].key != NULL){
		lua_pushnumber(L,sdl_key_enums[i].value);
		lua_setfield(L,-2,sdl_key_enums[i].key);
		i++;
	}
	lua_setfield(L,-2,"key");
}

int luaopen_sdl(lua_State* L){
	// SDL.Window metamethods
	add_metatable(L,"SDL.Window");
	lua_pushstring(L,"__index");
	lua_pushvalue(L,-2);
	lua_settable(L,-3);
	assign_metamethods(L,sdlwindowlib_m);
	pop_metatable(L);

	// SDL's global table
	add_clibrary(L,sdllib,"SDL");
	lua_getglobal(L,"SDL");

	// Load enums
	add_sdl_enums(L);
	add_sdl_key_enums(L);

	// Pop the SDL table
	lua_pop(L,1);
	return 0;
}

#endif