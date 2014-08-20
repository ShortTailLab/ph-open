/*
** Lua binding: SerializationUtils
** Generated automatically by tolua++-1.0.92 on Wed Oct 23 16:26:25 2013.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_SerializationUtils_open (lua_State* tolua_S);

#include "SerializationUtils.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"PH::IObject");
}

/* Open function */
TOLUA_API int tolua_SerializationUtils_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_module(tolua_S,"PH",0);
  tolua_beginmodule(tolua_S,"PH");
   tolua_cclass(tolua_S,"IObject","PH::IObject","",NULL);
   tolua_beginmodule(tolua_S,"IObject");
   tolua_endmodule(tolua_S);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_SerializationUtils (lua_State* tolua_S) {
 return tolua_SerializationUtils_open(tolua_S);
};
#endif

