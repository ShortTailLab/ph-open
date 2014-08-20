/*
** Lua binding: Gem
** Generated automatically by tolua++-1.0.92 on Wed Oct 23 16:26:25 2013.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_Gem_open (lua_State* tolua_S);

#include "Gem.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
}

/* Open function */
TOLUA_API int tolua_Gem_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_module(tolua_S,"PH",0);
  tolua_beginmodule(tolua_S,"PH");
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_Gem (lua_State* tolua_S) {
 return tolua_Gem_open(tolua_S);
};
#endif

