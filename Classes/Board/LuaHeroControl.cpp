/*
** Lua binding: HeroControl
** Generated automatically by tolua++-1.0.92 on Wed Oct 23 16:26:25 2013.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_HeroControl_open (lua_State* tolua_S);

#include "HeroControl.h"

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
 tolua_usertype(tolua_S,"PH::GemUtils::GemColor");
 tolua_usertype(tolua_S,"PH::HeroControl");
}

/* method: getHeroId of class  PH::HeroControl */
#ifndef TOLUA_DISABLE_tolua_HeroControl_PH_HeroControl_getHeroId00
static int tolua_HeroControl_PH_HeroControl_getHeroId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PH::HeroControl",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PH::HeroControl* self = (PH::HeroControl*)  tolua_tousertype(tolua_S,1,0);
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

/* method: getColor of class  PH::HeroControl */
#ifndef TOLUA_DISABLE_tolua_HeroControl_PH_HeroControl_getColor00
static int tolua_HeroControl_PH_HeroControl_getColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PH::HeroControl",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PH::HeroControl* self = (PH::HeroControl*)  tolua_tousertype(tolua_S,1,0);
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

/* method: getEnergy of class  PH::HeroControl */
#ifndef TOLUA_DISABLE_tolua_HeroControl_PH_HeroControl_getEnergy00
static int tolua_HeroControl_PH_HeroControl_getEnergy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PH::HeroControl",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PH::HeroControl* self = (PH::HeroControl*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getEnergy'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getEnergy();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getEnergy'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_HeroControl_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_module(tolua_S,"PH",0);
  tolua_beginmodule(tolua_S,"PH");
   tolua_cclass(tolua_S,"HeroControl","PH::HeroControl","",NULL);
   tolua_beginmodule(tolua_S,"HeroControl");
    tolua_function(tolua_S,"getHeroId",tolua_HeroControl_PH_HeroControl_getHeroId00);
    tolua_function(tolua_S,"getColor",tolua_HeroControl_PH_HeroControl_getColor00);
    tolua_function(tolua_S,"getEnergy",tolua_HeroControl_PH_HeroControl_getEnergy00);
   tolua_endmodule(tolua_S);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_HeroControl (lua_State* tolua_S) {
 return tolua_HeroControl_open(tolua_S);
};
#endif

