/*
** Lua binding: PlayerControl
** Generated automatically by tolua++-1.0.92 on Wed Oct 23 16:26:25 2013.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_PlayerControl_open (lua_State* tolua_S);

#include "PlayerControl.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"IObject");
 tolua_usertype(tolua_S,"CCObject");
 tolua_usertype(tolua_S,"PH::PlayerControl");
}

/* method: getHealth of class  PH::PlayerControl */
#ifndef TOLUA_DISABLE_tolua_PlayerControl_PH_PlayerControl_getHealth00
static int tolua_PlayerControl_PH_PlayerControl_getHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PH::PlayerControl",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PH::PlayerControl* self = (PH::PlayerControl*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getHealth'", NULL);
#endif
  {
   float tolua_ret = (float)  self->getHealth();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getHealth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getMaxHealth of class  PH::PlayerControl */
#ifndef TOLUA_DISABLE_tolua_PlayerControl_PH_PlayerControl_getMaxHealth00
static int tolua_PlayerControl_PH_PlayerControl_getMaxHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PH::PlayerControl",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PH::PlayerControl* self = (PH::PlayerControl*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getMaxHealth'", NULL);
#endif
  {
   float tolua_ret = (float)  self->getMaxHealth();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getMaxHealth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: __CCObject__ of class  PH::PlayerControl */
#ifndef TOLUA_DISABLE_tolua_get_PH__PlayerControl___CCObject__
static int tolua_get_PH__PlayerControl___CCObject__(lua_State* tolua_S)
{
  PH::PlayerControl* self = (PH::PlayerControl*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable '__CCObject__'",NULL);
#endif
#ifdef __cplusplus
   tolua_pushusertype(tolua_S,(void*)static_cast<CCObject*>(self), "CCObject");
#else
   tolua_pushusertype(tolua_S,(void*)((CCObject*)self), "CCObject");
#endif
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_PlayerControl_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_module(tolua_S,"PH",0);
  tolua_beginmodule(tolua_S,"PH");
   tolua_cclass(tolua_S,"PlayerControl","PH::PlayerControl","IObject",NULL);
   tolua_beginmodule(tolua_S,"PlayerControl");
    tolua_function(tolua_S,"getHealth",tolua_PlayerControl_PH_PlayerControl_getHealth00);
    tolua_function(tolua_S,"getMaxHealth",tolua_PlayerControl_PH_PlayerControl_getMaxHealth00);
    tolua_variable(tolua_S,"__CCObject__",tolua_get_PH__PlayerControl___CCObject__,NULL);
   tolua_endmodule(tolua_S);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_PlayerControl (lua_State* tolua_S) {
 return tolua_PlayerControl_open(tolua_S);
};
#endif

