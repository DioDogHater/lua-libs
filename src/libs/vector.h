#ifndef LUA_Vec3_H
#define LUA_Vec3_H

#include <string.h>
#include <math.h>

#include "../lua_wrap.h"

#ifndef squared
#define squared(x) (x)*(x)
#endif

static const char* VECTOR3_METATABLE = "stdlibs.Vector3";

struct Vector3 {
	double x, y, z;
};

static struct Vector3* checki_Vector3(lua_State* L, int i){
	return (struct Vector3*) check_metatable(L,i,VECTOR3_METATABLE,"'Vector3' expected");
}

static struct Vector3* check_Vector3(lua_State* L){
	return checki_Vector3(L,1);
}

static struct Vector3* create_Vector3(lua_State* L, double x, double y, double z){
	struct Vector3* r = (struct Vector3*) lua_newuserdata(L,sizeof(struct Vector3));
	*r = (struct Vector3){x,y,z};
	set_metatable(L,VECTOR3_METATABLE);
	return r;
}

static int Vector3_new(lua_State* L){
	struct Vector3* new_vector;
	int stack_size = lua_gettop(L);
	if(stack_size >= 3)
		(void)create_Vector3(L,luaL_checknumber(L,1),luaL_checknumber(L,2),luaL_checknumber(L,3));
	else if(stack_size == 1){
		if(lua_isnumber(L,1)){
			// If arg is number
			double arg = luaL_checknumber(L,1);
			(void)create_Vector3(L,arg,arg,arg);
		}else{
			// if arg is maybe Vector3
			struct Vector3* other_vector = check_Vector3(L);
			(void)create_Vector3(L,other_vector->x,other_vector->y,other_vector->z);
		}
	}else
		(void)create_Vector3(L,0.f,0.f,0.f);
	return 1;
}

static int Vector3_zero(lua_State* L){
	(void)create_Vector3(L,0.f,0.f,0.f);
	return 1;
}

static int Vector3_get(lua_State* L){
	struct Vector3* v = check_Vector3(L);
	lua_createtable(L,0,3);
	lua_pushstring(L,"x");
	lua_pushnumber(L,v->x);
	lua_settable(L,-3);
	lua_pushstring(L,"y");
	lua_pushnumber(L,v->y);
	lua_settable(L,-3);
	lua_pushstring(L,"z");
	lua_pushnumber(L,v->z);
	lua_settable(L,-3);
	return 1;
}

static int Vector3_index(lua_State* L){
	struct Vector3* v = check_Vector3(L);
	const char* key = luaL_checkstring(L,2);
	if(strcmp(key,"x") && strcmp(key,"y") && strcmp(key,"z"))
		get_metatable(L,VECTOR3_METATABLE);
	else
		(void)Vector3_get(L);
	lua_pushstring(L,key);
	lua_gettable(L,-2);
	return 1;
}

static int Vector3_newindex(lua_State* L){
	struct Vector3* v = check_Vector3(L);
	const char* key = luaL_checkstring(L,2);
	luaL_argcheck(L,!strcmp(key,"x") || !strcmp(key,"y") || !strcmp(key,"z"),2,"'x','y' or 'z' expected");
	double val = luaL_checknumber(L,3);
	if(key[0] == 'x') v->x = val;
	else if(key[0] == 'y') v->y = val;
	else v->z = val;
	return 0;
}

static int Vector3_magnitude(lua_State* L){
	struct Vector3* v = check_Vector3(L);
	double magnitude = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
	lua_pushnumber(L,magnitude);
	return 1;
}

static int Vector3_normalized(lua_State* L){
	struct Vector3* v = check_Vector3(L);
	(void)Vector3_magnitude(L);
	double magnitude = lua_tonumber(L,-1);
	(void)create_Vector3(L,v->x/magnitude,v->y/magnitude,v->z/magnitude);
	return 1;
}

static int Vector3_add(lua_State* L){
	struct Vector3* a = check_Vector3(L);
	struct Vector3* b = checki_Vector3(L,2);
	(void)create_Vector3(L,a->x+b->x,a->y+b->y,a->z+b->z);
	return 1;
}

static int Vector3_sub(lua_State* L){
	struct Vector3* a = check_Vector3(L);
	struct Vector3* b = checki_Vector3(L,2);
	(void)create_Vector3(L,a->x-b->x,a->y-b->y,a->z-b->z);
	return 1;
}

static int Vector3_mul(lua_State* L){
	struct Vector3* a = check_Vector3(L);
	if(lua_isnumber(L,2)){
		double b = luaL_checknumber(L,2);
		(void)create_Vector3(L,a->x*b,a->y*b,a->z*b);
	}else{
		struct Vector3* b = checki_Vector3(L,2);
		(void)create_Vector3(L,a->x*b->x,a->y*b->y,a->z*b->z);
	}
	return 1;
}

static int Vector3_dot(lua_State* L){
	struct Vector3* a = check_Vector3(L);
	struct Vector3* b = checki_Vector3(L,2);
	lua_pushnumber(L,a->x*b->x + a->y*b->y + a->z*b->z);
	return 1;
}

static int Vector3_div(lua_State* L){
	struct Vector3* a = check_Vector3(L);
	if(lua_isnumber(L,2)){
		double b = luaL_checknumber(L,2);
		(void)create_Vector3(L,a->x/b,a->y/b,a->z/b);
	}else{
		struct Vector3* b = checki_Vector3(L,2);
		(void)create_Vector3(L,a->x/b->x,a->y/b->y,a->z/b->z);
	}
	return 1;
}

static int Vector3_distance_to(lua_State* L){
	struct Vector3* a = check_Vector3(L);
	struct Vector3* b = checki_Vector3(L,2);
	double result = sqrt(squared(a->x-b->x) + squared(a->y-b->y) + squared(a->z-b->z));
	lua_pushnumber(L,result);
	return 1;
}

static int Vector3_distance_squared_to(lua_State* L){
	struct Vector3* a = check_Vector3(L);
	struct Vector3* b = checki_Vector3(L,2);
	double result = squared(a->x-b->x) + squared(a->y-b->y) + squared(a->z-b->z);
	lua_pushnumber(L,result);
	return 1;
}

static int Vector3_cmp(lua_State* L){
	struct Vector3* a = check_Vector3(L);
	struct Vector3* b = checki_Vector3(L,2);
	lua_pushboolean(L,(a->x == b->x && a->y == b->y && a->z == b->z));
	return 1;
}

static int Vector3_tostring(lua_State* L){
	struct Vector3* v = check_Vector3(L);
	lua_pushfstring(L,"Vec3(%f, %f, %f)",v->x,v->y,v->z);
	return 1;
}

static struct luaL_Reg vector3lib[] = {
	{"new",Vector3_new},
	{"zero",Vector3_zero},
	{NULL,NULL}
};

static struct luaL_Reg vector3lib_m[] = {
	{"__index",Vector3_index},
	{"__newindex",Vector3_newindex},
	{"__len",Vector3_magnitude},
	{"__add",Vector3_add},
	{"__sub",Vector3_sub},
	{"__mul",Vector3_mul},
	{"__div",Vector3_div},
	{"__concat",Vector3_dot},
	{"__eq",Vector3_cmp},
	{"magnitude",Vector3_magnitude},
	{"normalized",Vector3_normalized},
	{"distance_to",Vector3_distance_to},
	{"distance_squared_to",Vector3_distance_squared_to},
	{"get",Vector3_get},
	{"__tostring",Vector3_tostring},
	{NULL,NULL}
};

int luaopen_vector(lua_State* L){
	add_metatable(L,VECTOR3_METATABLE);
	assign_metamethods(L,vector3lib_m);
	pop_metatable(L);
	add_clibrary(L,vector3lib,"Vec3");
	return 0;
}

#endif