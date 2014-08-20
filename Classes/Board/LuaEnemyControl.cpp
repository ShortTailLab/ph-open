/*
** Lua binding: EnemyControl
** Generated automatically by tolua++-1.0.92 on Wed Oct 23 16:26:25 2013.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_EnemyControl_open (lua_State* tolua_S);

#include "EnemyControl.h"

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_PH__GemUtils__GemColor (lua_State* tolua_S)
{
 PH::GemUtils::GemColor* self = (PH::GemUtils::GemColor*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"PH::IObject");
 tolua_usertype(tolua_S,"PH::GemUtils::GemColor");
 tolua_usertype(tolua_S,"CCSprite");
 tolua_usertype(tolua_S,"boost::enable_shared_from_this<EnemyControl>");
 tolua_usertype(tolua_S,"PH::EnemyControl");
}

/* method: getHealth of class  PH::EnemyControl */
#ifndef TOLUA_DISABLE_tolua_EnemyControl_PH_EnemyControl_getHealth00
static int tolua_EnemyControl_PH_EnemyControl_getHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PH::EnemyControl",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PH::EnemyControl* self = (PH::EnemyControl*)  tolua_tousertype(tolua_S,1,0);
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

/* method: getMaxHealth of class  PH::EnemyControl */
#ifndef TOLUA_DISABLE_tolua_EnemyControl_PH_EnemyControl_getMaxHealth00
static int tolua_EnemyControl_PH_EnemyControl_getMaxHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PH::EnemyControl",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PH::EnemyControl* self = (PH::EnemyControl*)  tolua_tousertype(tolua_S,1,0);
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

/* method: getHeroId of class  PH::EnemyControl */
#ifndef TOLUA_DISABLE_tolua_EnemyControl_PH_EnemyControl_getHeroId00
static int tolua_EnemyControl_PH_EnemyControl_getHeroId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PH::EnemyControl",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PH::EnemyControl* self = (PH::EnemyControl*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getHeroId'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getHeroId();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getHeroId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getColor of class  PH::EnemyControl */
#ifndef TOLUA_DISABLE_tolua_EnemyControl_PH_EnemyControl_getColor00
static int tolua_EnemyControl_PH_EnemyControl_getColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PH::EnemyControl",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PH::EnemyControl* self = (PH::EnemyControl*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getColor'", NULL);
#endif
  {
   PH::GemUtils::GemColor tolua_ret = (PH::GemUtils::GemColor)  self->getColor();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((PH::GemUtils::GemColor)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"PH::GemUtils::GemColor");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(PH::GemUtils::GemColor));
     tolua_pushusertype(tolua_S,tolua_obj,"PH::GemUtils::GemColor");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getSprite of class  PH::EnemyControl */
#ifndef TOLUA_DISABLE_tolua_EnemyControl_PH_EnemyControl_getSprite00
static int tolua_EnemyControl_PH_EnemyControl_getSprite00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PH::EnemyControl",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PH::EnemyControl* self = (PH::EnemyControl*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getSprite'", NULL);
#endif
  {
   CCSprite* tolua_ret = (CCSprite*)  self->getSprite();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"CCSprite");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getSprite'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: __PH of class  PH::EnemyControl */
#ifndef TOLUA_DISABLE_tolua_get_PH__EnemyControl___PH__IObject__
static int tolua_get_PH__EnemyControl___PH__IObject__(lua_State* tolua_S)
{
  PH::EnemyControl* self = (PH::EnemyControl*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable '__PH'",NULL);
#endif
#ifdef __cplusplus
   tolua_pushusertype(tolua_S,(void*)static_cast<PH::IObject*>(self), "PH::IObject");
#else
   tolua_pushusertype(tolua_S,(void*)((PH::IObject*)self), "PH::IObject");
#endif
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_EnemyControl_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_module(tolua_S,"PH",0);
  tolua_beginmodule(tolua_S,"PH");
   tolua_cclass(tolua_S,"EnemyControl","PH::EnemyControl","boost::enable_shared_from_this<EnemyControl>",NULL);
   tolua_beginmodule(tolua_S,"EnemyControl");
    tolua_function(tolua_S,"getHealth",tolua_EnemyControl_PH_EnemyControl_getHealth00);
    tolua_function(tolua_S,"getMaxHealth",tolua_EnemyControl_PH_EnemyControl_getMaxHealth00);
    tolua_function(tolua_S,"getHeroId",tolua_EnemyControl_PH_EnemyControl_getHeroId00);
    tolua_function(tolua_S,"getColor",tolua_EnemyControl_PH_EnemyControl_getColor00);
    tolua_function(tolua_S,"getSprite",tolua_EnemyControl_PH_EnemyControl_getSprite00);
    tolua_variable(tolua_S,"__PH__IObject__",tolua_get_PH__EnemyControl___PH__IObject__,NULL);
   tolua_endmodule(tolua_S);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_EnemyControl (lua_State* tolua_S) {
 return tolua_EnemyControl_open(tolua_S);
};
#endif

