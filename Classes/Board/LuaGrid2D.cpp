/*
** Lua binding: Grid2D
** Generated automatically by tolua++-1.0.92 on Wed Oct 23 16:26:25 2013.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_Grid2D_open (lua_State* tolua_S);

#include "Grid2D.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"PH::Grid2D<PH::Gem*>");
 tolua_usertype(tolua_S,"PH::Gem");
}

/* get function: Width of class  PH::Grid2D<PH::Gem*> */
#ifndef TOLUA_DISABLE_tolua_get_PH__Grid2D_PH__Gem___Width
static int tolua_get_PH__Grid2D_PH__Gem___Width(lua_State* tolua_S)
{
  PH::Grid2D<PH::Gem*>* self = (PH::Grid2D<PH::Gem*>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'Width'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->Width);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: Height of class  PH::Grid2D<PH::Gem*> */
#ifndef TOLUA_DISABLE_tolua_get_PH__Grid2D_PH__Gem___Height
static int tolua_get_PH__Grid2D_PH__Gem___Height(lua_State* tolua_S)
{
  PH::Grid2D<PH::Gem*>* self = (PH::Grid2D<PH::Gem*>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'Height'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->Height);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* method: e of class  PH::Grid2D<PH::Gem*> */
#ifndef TOLUA_DISABLE_tolua_Grid2D_PH_Grid2D_PH__Gem___e00
static int tolua_Grid2D_PH_Grid2D_PH__Gem___e00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PH::Grid2D<PH::Gem*>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PH::Grid2D<PH::Gem*>* self = (PH::Grid2D<PH::Gem*>*)  tolua_tousertype(tolua_S,1,0);
  int x = ((int)  tolua_tonumber(tolua_S,2,0));
  int y = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'e'", NULL);
#endif
  {
   PH::Gem* tolua_ret = (PH::Gem*)  self->e(x,y);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"PH::Gem");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'e'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: e of class  PH::Grid2D<PH::Gem*> */
#ifndef TOLUA_DISABLE_tolua_Grid2D_PH_Grid2D_PH__Gem___e01
static int tolua_Grid2D_PH_Grid2D_PH__Gem___e01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PH::Grid2D<PH::Gem*>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"PH::Gem",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  PH::Grid2D<PH::Gem*>* self = (PH::Grid2D<PH::Gem*>*)  tolua_tousertype(tolua_S,1,0);
  int x = ((int)  tolua_tonumber(tolua_S,2,0));
  int y = ((int)  tolua_tonumber(tolua_S,3,0));
  PH::Gem* val = ((PH::Gem*)  tolua_tousertype(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'e'", NULL);
#endif
  {
   self->e(x,y,val);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Grid2D_PH_Grid2D_PH__Gem___e00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: e of class  PH::Grid2D<PH::Gem*> */
#ifndef TOLUA_DISABLE_tolua_Grid2D_PH_Grid2D_PH__Gem___e02
static int tolua_Grid2D_PH_Grid2D_PH__Gem___e02(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PH::Grid2D<PH::Gem*>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  PH::Grid2D<PH::Gem*>* self = (PH::Grid2D<PH::Gem*>*)  tolua_tousertype(tolua_S,1,0);
  int i = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'e'", NULL);
#endif
  {
   PH::Gem* tolua_ret = (PH::Gem*)  self->e(i);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"PH::Gem");
  }
 }
 return 1;
tolua_lerror:
 return tolua_Grid2D_PH_Grid2D_PH__Gem___e01(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: e of class  PH::Grid2D<PH::Gem*> */
#ifndef TOLUA_DISABLE_tolua_Grid2D_PH_Grid2D_PH__Gem___e03
static int tolua_Grid2D_PH_Grid2D_PH__Gem___e03(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PH::Grid2D<PH::Gem*>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"PH::Gem",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  PH::Grid2D<PH::Gem*>* self = (PH::Grid2D<PH::Gem*>*)  tolua_tousertype(tolua_S,1,0);
  int i = ((int)  tolua_tonumber(tolua_S,2,0));
  PH::Gem* val = ((PH::Gem*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'e'", NULL);
#endif
  {
   self->e(i,val);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Grid2D_PH_Grid2D_PH__Gem___e02(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: size of class  PH::Grid2D<PH::Gem*> */
#ifndef TOLUA_DISABLE_tolua_Grid2D_PH_Grid2D_PH__Gem___size00
static int tolua_Grid2D_PH_Grid2D_PH__Gem___size00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PH::Grid2D<PH::Gem*>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PH::Grid2D<PH::Gem*>* self = (const PH::Grid2D<PH::Gem*>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'size'", NULL);
#endif
  {
   int tolua_ret = (int)  self->size();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'size'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_Grid2D_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_module(tolua_S,"PH",0);
  tolua_beginmodule(tolua_S,"PH");
   tolua_cclass(tolua_S,"Grid2D_PH__Gem__","PH::Grid2D<PH::Gem*>","",NULL);
   tolua_beginmodule(tolua_S,"Grid2D_PH__Gem__");
    tolua_variable(tolua_S,"Width",tolua_get_PH__Grid2D_PH__Gem___Width,NULL);
    tolua_variable(tolua_S,"Height",tolua_get_PH__Grid2D_PH__Gem___Height,NULL);
    tolua_function(tolua_S,"e",tolua_Grid2D_PH_Grid2D_PH__Gem___e00);
    tolua_function(tolua_S,"e",tolua_Grid2D_PH_Grid2D_PH__Gem___e01);
    tolua_function(tolua_S,"e",tolua_Grid2D_PH_Grid2D_PH__Gem___e02);
    tolua_function(tolua_S,"e",tolua_Grid2D_PH_Grid2D_PH__Gem___e03);
    tolua_function(tolua_S,"size",tolua_Grid2D_PH_Grid2D_PH__Gem___size00);
   tolua_endmodule(tolua_S);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_Grid2D (lua_State* tolua_S) {
 return tolua_Grid2D_open(tolua_S);
};
#endif

