/*
** Lua binding: SkillContext
** Generated automatically by tolua++-1.0.92 on Wed Oct 23 16:26:25 2013.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_SkillContext_open (lua_State* tolua_S);

#include "SkillContext.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
}

/* function: ActiveTransformGemColor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveTransformGemColor00
static int tolua_SkillContext_ActiveTransformGemColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_iscppstring(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::string from = ((std::string)  tolua_tocppstring(tolua_S,1,0));
  std::string to = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  {
   ActiveTransformGemColor(from,to);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveTransformGemColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveTransformAllGem */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveTransformAllGem00
static int tolua_SkillContext_ActiveTransformAllGem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   ActiveTransformAllGem();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveTransformAllGem'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveRandomizeGem */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveRandomizeGem00
static int tolua_SkillContext_ActiveRandomizeGem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   ActiveRandomizeGem();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveRandomizeGem'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveStopMoveTimer */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveStopMoveTimer00
static int tolua_SkillContext_ActiveStopMoveTimer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   ActiveStopMoveTimer();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveStopMoveTimer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveExtendMoveTimer */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveExtendMoveTimer00
static int tolua_SkillContext_ActiveExtendMoveTimer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float secs = ((float)  tolua_tonumber(tolua_S,1,0));
  {
   ActiveExtendMoveTimer(secs);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveExtendMoveTimer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveDisableSweep */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveDisableSweep00
static int tolua_SkillContext_ActiveDisableSweep00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float secs = ((float)  tolua_tonumber(tolua_S,1,0));
  {
   ActiveDisableSweep(secs);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveDisableSweep'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveExtendEnemyWaitTurn */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveExtendEnemyWaitTurn00
static int tolua_SkillContext_ActiveExtendEnemyWaitTurn00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int turn = ((int)  tolua_tonumber(tolua_S,1,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  {
   ActiveExtendEnemyWaitTurn(turn,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveExtendEnemyWaitTurn'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveSortGem */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveSortGem00
static int tolua_SkillContext_ActiveSortGem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   ActiveSortGem();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveSortGem'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveMoveAgain */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveMoveAgain00
static int tolua_SkillContext_ActiveMoveAgain00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int count = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   ActiveMoveAgain(count);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveMoveAgain'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveInstantEnergy */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveInstantEnergy00
static int tolua_SkillContext_ActiveInstantEnergy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  {
   ActiveInstantEnergy(factor,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveInstantEnergy'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveDispelPassiveBuff */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveDispelPassiveBuff00
static int tolua_SkillContext_ActiveDispelPassiveBuff00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   ActiveDispelPassiveBuff();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveDispelPassiveBuff'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveDamage */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveDamage00
static int tolua_SkillContext_ActiveDamage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_iscppstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,1,0));
  float damage = ((float)  tolua_tonumber(tolua_S,2,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,3,0));
  bool ignoreArmor = ((bool)  tolua_toboolean(tolua_S,4,0));
  {
   ActiveDamage(color,damage,isArea,ignoreArmor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveDamage'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveDamageByValue */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveDamageByValue00
static int tolua_SkillContext_ActiveDamageByValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float damage = ((float)  tolua_tonumber(tolua_S,1,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,2,0));
  {
   ActiveDamageByValue(damage,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveDamageByValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveDamageByPlayerHealthFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveDamageByPlayerHealthFactor00
static int tolua_SkillContext_ActiveDamageByPlayerHealthFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,2,0));
  {
   ActiveDamageByPlayerHealthFactor(factor,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveDamageByPlayerHealthFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveDamageByHeroHealthFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveDamageByHeroHealthFactor00
static int tolua_SkillContext_ActiveDamageByHeroHealthFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,2,0));
  {
   ActiveDamageByHeroHealthFactor(factor,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveDamageByHeroHealthFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveDamageByFactorFromColor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveDamageByFactorFromColor00
static int tolua_SkillContext_ActiveDamageByFactorFromColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,2,0));
  {
   ActiveDamageByFactorFromColor(factor,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveDamageByFactorFromColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveDamageByFactorFromTeam */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveDamageByFactorFromTeam00
static int tolua_SkillContext_ActiveDamageByFactorFromTeam00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,2,0));
  {
   ActiveDamageByFactorFromTeam(factor,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveDamageByFactorFromTeam'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveDamageByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveDamageByFactor00
static int tolua_SkillContext_ActiveDamageByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,2,0));
  {
   ActiveDamageByFactor(factor,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveDamageByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveDamageByFactorWithoutArmor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveDamageByFactorWithoutArmor00
static int tolua_SkillContext_ActiveDamageByFactorWithoutArmor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_iscppstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,3,0));
  {
   ActiveDamageByFactorWithoutArmor(color,factor,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveDamageByFactorWithoutArmor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveDamageByEnemyHealthFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveDamageByEnemyHealthFactor00
static int tolua_SkillContext_ActiveDamageByEnemyHealthFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float percent = ((float)  tolua_tonumber(tolua_S,1,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,2,0));
  {
   ActiveDamageByEnemyHealthFactor(percent,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveDamageByEnemyHealthFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveDamageAndHealByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveDamageAndHealByFactor00
static int tolua_SkillContext_ActiveDamageAndHealByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,2,0));
  {
   ActiveDamageAndHealByFactor(factor,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveDamageAndHealByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveDamageByFactorAndEnemyHealth */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveDamageByFactorAndEnemyHealth00
static int tolua_SkillContext_ActiveDamageByFactorAndEnemyHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  float healthRatio = ((float)  tolua_tonumber(tolua_S,2,0));
  float damageFactor = ((float)  tolua_tonumber(tolua_S,3,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,4,0));
  {
   ActiveDamageByFactorAndEnemyHealth(factor,healthRatio,damageFactor,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveDamageByFactorAndEnemyHealth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveDamageBySelfDamageFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveDamageBySelfDamageFactor00
static int tolua_SkillContext_ActiveDamageBySelfDamageFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float healthFactor = ((float)  tolua_tonumber(tolua_S,1,0));
  float attackFactor = ((float)  tolua_tonumber(tolua_S,2,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,3,0));
  {
   ActiveDamageBySelfDamageFactor(healthFactor,attackFactor,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveDamageBySelfDamageFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveDamageByGems */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveDamageByGems00
static int tolua_SkillContext_ActiveDamageByGems00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  std::string gemColor = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  std::string attColor = ((std::string)  tolua_tocppstring(tolua_S,3,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,4,0));
  {
   ActiveDamageByGems(factor,gemColor,attColor,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveDamageByGems'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveMultipleAttacksByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveMultipleAttacksByFactor00
static int tolua_SkillContext_ActiveMultipleAttacksByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int count = ((int)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,3,0));
  {
   ActiveMultipleAttacksByFactor(count,factor,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveMultipleAttacksByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveLeechByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveLeechByFactor00
static int tolua_SkillContext_ActiveLeechByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float attackFactor = ((float)  tolua_tonumber(tolua_S,1,0));
  float leechFactor = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   ActiveLeechByFactor(attackFactor,leechFactor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveLeechByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveHeal */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveHeal00
static int tolua_SkillContext_ActiveHeal00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float val = ((float)  tolua_tonumber(tolua_S,1,0));
  {
   ActiveHeal(val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveHeal'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveHealByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveHealByFactor00
static int tolua_SkillContext_ActiveHealByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float val = ((float)  tolua_tonumber(tolua_S,1,0));
  {
   ActiveHealByFactor(val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveHealByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveHealFull */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveHealFull00
static int tolua_SkillContext_ActiveHealFull00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   ActiveHealFull();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveHealFull'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveBoostAttackByColor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveBoostAttackByColor00
static int tolua_SkillContext_ActiveBoostAttackByColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float val = ((float)  tolua_tonumber(tolua_S,2,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,3,0));
  {
   ActiveBoostAttackByColor(duration,val,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveBoostAttackByColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveBoostAttackAll */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveBoostAttackAll00
static int tolua_SkillContext_ActiveBoostAttackAll00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float val = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   ActiveBoostAttackAll(duration,val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveBoostAttackAll'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveBoostComboFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveBoostComboFactor00
static int tolua_SkillContext_ActiveBoostComboFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   ActiveBoostComboFactor(duration,factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveBoostComboFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveBoostMaxHealth */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveBoostMaxHealth00
static int tolua_SkillContext_ActiveBoostMaxHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   ActiveBoostMaxHealth(duration,factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveBoostMaxHealth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveBoostEnergyByColor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveBoostEnergyByColor00
static int tolua_SkillContext_ActiveBoostEnergyByColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  int val = ((int)  tolua_tonumber(tolua_S,2,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,3,0));
  {
   ActiveBoostEnergyByColor(duration,val,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveBoostEnergyByColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveAmplifyDamageByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveAmplifyDamageByFactor00
static int tolua_SkillContext_ActiveAmplifyDamageByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,3,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,4,0));
  {
   ActiveAmplifyDamageByFactor(duration,factor,color,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveAmplifyDamageByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActivePoisonByValue */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActivePoisonByValue00
static int tolua_SkillContext_ActivePoisonByValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float damage = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   ActivePoisonByValue(duration,damage);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActivePoisonByValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActivePoisonByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActivePoisonByFactor00
static int tolua_SkillContext_ActivePoisonByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   ActivePoisonByFactor(duration,factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActivePoisonByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveBlind */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveBlind00
static int tolua_SkillContext_ActiveBlind00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   ActiveBlind(duration);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveBlind'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveBlindHeroRandom */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveBlindHeroRandom00
static int tolua_SkillContext_ActiveBlindHeroRandom00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int count = ((int)  tolua_tonumber(tolua_S,1,0));
  int duration = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   ActiveBlindHeroRandom(count,duration);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveBlindHeroRandom'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveBlindHeroByColor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveBlindHeroByColor00
static int tolua_SkillContext_ActiveBlindHeroByColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_iscppstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,1,0));
  int duration = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   ActiveBlindHeroByColor(color,duration);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveBlindHeroByColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveSilence */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveSilence00
static int tolua_SkillContext_ActiveSilence00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,2,0));
  {
   ActiveSilence(duration,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveSilence'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveReduceEnemyArmorByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveReduceEnemyArmorByFactor00
static int tolua_SkillContext_ActiveReduceEnemyArmorByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   ActiveReduceEnemyArmorByFactor(duration,factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveReduceEnemyArmorByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveWeakenEnemyAttackByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveWeakenEnemyAttackByFactor00
static int tolua_SkillContext_ActiveWeakenEnemyAttackByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,3,0));
  {
   ActiveWeakenEnemyAttackByFactor(duration,factor,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveWeakenEnemyAttackByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveReflectByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveReflectByFactor00
static int tolua_SkillContext_ActiveReflectByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  int count = ((int)  tolua_tonumber(tolua_S,2,0));
  float factor = ((float)  tolua_tonumber(tolua_S,3,0));
  float probality = ((float)  tolua_tonumber(tolua_S,4,0));
  {
   ActiveReflectByFactor(duration,count,factor,probality);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveReflectByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveReduceDamageByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveReduceDamageByFactor00
static int tolua_SkillContext_ActiveReduceDamageByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   ActiveReduceDamageByFactor(duration,factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveReduceDamageByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveLeechByAttack */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveLeechByAttack00
static int tolua_SkillContext_ActiveLeechByAttack00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float leechFactor = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   ActiveLeechByAttack(duration,leechFactor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveLeechByAttack'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveShield */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveShield00
static int tolua_SkillContext_ActiveShield00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,3,0));
  {
   ActiveShield(duration,factor,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveShield'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveInvincible */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveInvincible00
static int tolua_SkillContext_ActiveInvincible00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float probality = ((float)  tolua_tonumber(tolua_S,2,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,3,0));
  {
   ActiveInvincible(duration,probality,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveInvincible'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveReduceEnergyCost */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveReduceEnergyCost00
static int tolua_SkillContext_ActiveReduceEnergyCost00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int count = ((int)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   ActiveReduceEnergyCost(count,factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveReduceEnergyCost'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveReduceCooldownTime */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveReduceCooldownTime00
static int tolua_SkillContext_ActiveReduceCooldownTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int times = ((int)  tolua_tonumber(tolua_S,1,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  {
   ActiveReduceCooldownTime(times,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveReduceCooldownTime'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveAbsorbDamage */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveAbsorbDamage00
static int tolua_SkillContext_ActiveAbsorbDamage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  int count = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   ActiveAbsorbDamage(duration,count);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveAbsorbDamage'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveTriggerCriticalAttack */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveTriggerCriticalAttack00
static int tolua_SkillContext_ActiveTriggerCriticalAttack00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  float probality = ((float)  tolua_tonumber(tolua_S,3,0));
  {
   ActiveTriggerCriticalAttack(duration,factor,probality);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveTriggerCriticalAttack'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveSuckBloodFromEnemyByRegen */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveSuckBloodFromEnemyByRegen00
static int tolua_SkillContext_ActiveSuckBloodFromEnemyByRegen00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,3,0));
  {
   ActiveSuckBloodFromEnemyByRegen(duration,factor,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveSuckBloodFromEnemyByRegen'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveHealByColor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveHealByColor00
static int tolua_SkillContext_ActiveHealByColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  bool still = ((bool)  tolua_toboolean(tolua_S,3,0));
  {
   ActiveHealByColor(duration,color,still);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveHealByColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveHealByMaxHealth */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveHealByMaxHealth00
static int tolua_SkillContext_ActiveHealByMaxHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   ActiveHealByMaxHealth(duration,factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveHealByMaxHealth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveReduceAttackByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveReduceAttackByFactor00
static int tolua_SkillContext_ActiveReduceAttackByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  int turn = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   ActiveReduceAttackByFactor(factor,color,turn);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveReduceAttackByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveReduceResistance */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveReduceResistance00
static int tolua_SkillContext_ActiveReduceResistance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float value = ((float)  tolua_tonumber(tolua_S,1,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  int turn = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   ActiveReduceResistance(value,color,turn);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveReduceResistance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveReduceDamageTakenByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveReduceDamageTakenByFactor00
static int tolua_SkillContext_ActiveReduceDamageTakenByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  int turn = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   ActiveReduceDamageTakenByFactor(factor,color,turn);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveReduceDamageTakenByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveReduceOpponentDragTime */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveReduceOpponentDragTime00
static int tolua_SkillContext_ActiveReduceOpponentDragTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float secs = ((float)  tolua_tonumber(tolua_S,1,0));
  int turn = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   ActiveReduceOpponentDragTime(secs,turn);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveReduceOpponentDragTime'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveZombine */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveZombine00
static int tolua_SkillContext_ActiveZombine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int count = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   ActiveZombine(count);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveZombine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveFightBack */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveFightBack00
static int tolua_SkillContext_ActiveFightBack00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  float probality = ((float)  tolua_tonumber(tolua_S,3,0));
  {
   ActiveFightBack(duration,factor,probality);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveFightBack'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveForceAreaAttack */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveForceAreaAttack00
static int tolua_SkillContext_ActiveForceAreaAttack00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   ActiveForceAreaAttack(duration);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveForceAreaAttack'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveBloodForBlood */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveBloodForBlood00
static int tolua_SkillContext_ActiveBloodForBlood00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float ratio = ((float)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,3,0));
  {
   ActiveBloodForBlood(ratio,factor,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveBloodForBlood'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveEyeForEye */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveEyeForEye00
static int tolua_SkillContext_ActiveEyeForEye00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float attacker = ((float)  tolua_tonumber(tolua_S,1,0));
  float defender = ((float)  tolua_tonumber(tolua_S,2,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,3,0));
  {
   ActiveEyeForEye(attacker,defender,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveEyeForEye'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveBonusGemWeight */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveBonusGemWeight00
static int tolua_SkillContext_ActiveBonusGemWeight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,3,0));
  {
   ActiveBonusGemWeight(duration,factor,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveBonusGemWeight'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ActiveBoostEnergyByFactorByColor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_ActiveBoostEnergyByFactorByColor00
static int tolua_SkillContext_ActiveBoostEnergyByFactorByColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int duration = ((int)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,3,0));
  {
   ActiveBoostEnergyByFactorByColor(duration,factor,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActiveBoostEnergyByFactorByColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveReflectByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveReflectByFactor00
static int tolua_SkillContext_PassiveReflectByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  float probality = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   PassiveReflectByFactor(factor,probality);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveReflectByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveAttackFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveAttackFactor00
static int tolua_SkillContext_PassiveAttackFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  {
   PassiveAttackFactor(factor,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveAttackFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveAttackFactorByTitle */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveAttackFactorByTitle00
static int tolua_SkillContext_PassiveAttackFactorByTitle00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  std::string title = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  {
   PassiveAttackFactorByTitle(factor,title);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveAttackFactorByTitle'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveRandomAttackFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveRandomAttackFactor00
static int tolua_SkillContext_PassiveRandomAttackFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  float alterFactor = ((float)  tolua_tonumber(tolua_S,2,0));
  float probality = ((float)  tolua_tonumber(tolua_S,3,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,4,0));
  {
   PassiveRandomAttackFactor(factor,alterFactor,probality,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveRandomAttackFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveRegenFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveRegenFactor00
static int tolua_SkillContext_PassiveRegenFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  {
   PassiveRegenFactor(factor,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveRegenFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveDamageReduceFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveDamageReduceFactor00
static int tolua_SkillContext_PassiveDamageReduceFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  {
   PassiveDamageReduceFactor(factor,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveDamageReduceFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveFollowUpAttack */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveFollowUpAttack00
static int tolua_SkillContext_PassiveFollowUpAttack00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,2,0));
  {
   PassiveFollowUpAttack(factor,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveFollowUpAttack'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveSurviveCriticalHitFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveSurviveCriticalHitFactor00
static int tolua_SkillContext_PassiveSurviveCriticalHitFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  {
   PassiveSurviveCriticalHitFactor(factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveSurviveCriticalHitFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveHealByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveHealByFactor00
static int tolua_SkillContext_PassiveHealByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  {
   PassiveHealByFactor(factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveHealByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveSetBonusHealthByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveSetBonusHealthByFactor00
static int tolua_SkillContext_PassiveSetBonusHealthByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  {
   PassiveSetBonusHealthByFactor(factor,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveSetBonusHealthByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveSetBonusHealthByFactorByTitle */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveSetBonusHealthByFactorByTitle00
static int tolua_SkillContext_PassiveSetBonusHealthByFactorByTitle00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  std::string title = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  {
   PassiveSetBonusHealthByFactorByTitle(factor,title);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveSetBonusHealthByFactorByTitle'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveBonusDragTime */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveBonusDragTime00
static int tolua_SkillContext_PassiveBonusDragTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  {
   PassiveBonusDragTime(factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveBonusDragTime'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveSetEnergyFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveSetEnergyFactor00
static int tolua_SkillContext_PassiveSetEnergyFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  {
   PassiveSetEnergyFactor(factor,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveSetEnergyFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveSetMinEnergy */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveSetMinEnergy00
static int tolua_SkillContext_PassiveSetMinEnergy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float minEnergy = ((float)  tolua_tonumber(tolua_S,1,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,2,0));
  {
   PassiveSetMinEnergy(minEnergy,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveSetMinEnergy'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveSetComboFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveSetComboFactor00
static int tolua_SkillContext_PassiveSetComboFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  {
   PassiveSetComboFactor(factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveSetComboFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveReduceDamageOnHealth */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveReduceDamageOnHealth00
static int tolua_SkillContext_PassiveReduceDamageOnHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float healthFactor = ((float)  tolua_tonumber(tolua_S,1,0));
  float reduceFactor = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   PassiveReduceDamageOnHealth(healthFactor,reduceFactor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveReduceDamageOnHealth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveBoostAttackOnHealth */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveBoostAttackOnHealth00
static int tolua_SkillContext_PassiveBoostAttackOnHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float healthFactor = ((float)  tolua_tonumber(tolua_S,1,0));
  float attackFactor = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   PassiveBoostAttackOnHealth(healthFactor,attackFactor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveBoostAttackOnHealth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveBleedingByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveBleedingByFactor00
static int tolua_SkillContext_PassiveBleedingByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  {
   PassiveBleedingByFactor(factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveBleedingByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveBonusCoins */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveBonusCoins00
static int tolua_SkillContext_PassiveBonusCoins00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float percent = ((float)  tolua_tonumber(tolua_S,1,0));
  {
   PassiveBonusCoins(percent);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveBonusCoins'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveBonusExp */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveBonusExp00
static int tolua_SkillContext_PassiveBonusExp00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float percent = ((float)  tolua_tonumber(tolua_S,1,0));
  {
   PassiveBonusExp(percent);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveBonusExp'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveBonusLuck */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveBonusLuck00
static int tolua_SkillContext_PassiveBonusLuck00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float percent = ((float)  tolua_tonumber(tolua_S,1,0));
  {
   PassiveBonusLuck(percent);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveBonusLuck'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveBonusEnemyStartTurn */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveBonusEnemyStartTurn00
static int tolua_SkillContext_PassiveBonusEnemyStartTurn00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int bonus = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   PassiveBonusEnemyStartTurn(bonus);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveBonusEnemyStartTurn'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveSetAreaAttackAndFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveSetAreaAttackAndFactor00
static int tolua_SkillContext_PassiveSetAreaAttackAndFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  {
   PassiveSetAreaAttackAndFactor(factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveSetAreaAttackAndFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveSetGemWeight */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveSetGemWeight00
static int tolua_SkillContext_PassiveSetGemWeight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_iscppstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   PassiveSetGemWeight(color,factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveSetGemWeight'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveSetMinComboCount */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveSetMinComboCount00
static int tolua_SkillContext_PassiveSetMinComboCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int count = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   PassiveSetMinComboCount(count);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveSetMinComboCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveDamageToHealthByFactor */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveDamageToHealthByFactor00
static int tolua_SkillContext_PassiveDamageToHealthByFactor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  {
   PassiveDamageToHealthByFactor(factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveDamageToHealthByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveSetEnergyBonusByGemCount */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveSetEnergyBonusByGemCount00
static int tolua_SkillContext_PassiveSetEnergyBonusByGemCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int count = ((int)  tolua_tonumber(tolua_S,1,0));
  float bonus = ((float)  tolua_tonumber(tolua_S,2,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,3,0));
  {
   PassiveSetEnergyBonusByGemCount(count,bonus,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveSetEnergyBonusByGemCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveNothingTodo */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveNothingTodo00
static int tolua_SkillContext_PassiveNothingTodo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   PassiveNothingTodo();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveNothingTodo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveSetArmorByHealthRatio */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveSetArmorByHealthRatio00
static int tolua_SkillContext_PassiveSetArmorByHealthRatio00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  float healthRatio = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   PassiveSetArmorByHealthRatio(factor,healthRatio);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveSetArmorByHealthRatio'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveSetExtraWaitForEnemy */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveSetExtraWaitForEnemy00
static int tolua_SkillContext_PassiveSetExtraWaitForEnemy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int round = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   PassiveSetExtraWaitForEnemy(round);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveSetExtraWaitForEnemy'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveSetBonusGemInAreaAttackCheck */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveSetBonusGemInAreaAttackCheck00
static int tolua_SkillContext_PassiveSetBonusGemInAreaAttackCheck00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int count = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   PassiveSetBonusGemInAreaAttackCheck(count);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveSetBonusGemInAreaAttackCheck'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveFollowUpAttackWithoutResistence */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveFollowUpAttackWithoutResistence00
static int tolua_SkillContext_PassiveFollowUpAttackWithoutResistence00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float damage = ((float)  tolua_tonumber(tolua_S,1,0));
  bool isArea = ((bool)  tolua_toboolean(tolua_S,2,0));
  {
   PassiveFollowUpAttackWithoutResistence(damage,isArea);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveFollowUpAttackWithoutResistence'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveSetInvicible */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveSetInvicible00
static int tolua_SkillContext_PassiveSetInvicible00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  {
   PassiveSetInvicible(factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveSetInvicible'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveShield */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveShield00
static int tolua_SkillContext_PassiveShield00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int last = ((int)  tolua_tonumber(tolua_S,1,0));
  int gap = ((int)  tolua_tonumber(tolua_S,2,0));
  int comboCount = ((int)  tolua_tonumber(tolua_S,3,0));
  int shieldCount = ((int)  tolua_tonumber(tolua_S,4,0));
  float reflectFactor = ((float)  tolua_tonumber(tolua_S,5,0));
  {
   PassiveShield(last,gap,comboCount,shieldCount,reflectFactor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveShield'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PassiveReduceOpponentDragTime */
#ifndef TOLUA_DISABLE_tolua_SkillContext_PassiveReduceOpponentDragTime00
static int tolua_SkillContext_PassiveReduceOpponentDragTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float secs = ((float)  tolua_tonumber(tolua_S,1,0));
  {
   PassiveReduceOpponentDragTime(secs);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PassiveReduceOpponentDragTime'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_SkillContext_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_function(tolua_S,"ActiveTransformGemColor",tolua_SkillContext_ActiveTransformGemColor00);
  tolua_function(tolua_S,"ActiveTransformAllGem",tolua_SkillContext_ActiveTransformAllGem00);
  tolua_function(tolua_S,"ActiveRandomizeGem",tolua_SkillContext_ActiveRandomizeGem00);
  tolua_function(tolua_S,"ActiveStopMoveTimer",tolua_SkillContext_ActiveStopMoveTimer00);
  tolua_function(tolua_S,"ActiveExtendMoveTimer",tolua_SkillContext_ActiveExtendMoveTimer00);
  tolua_function(tolua_S,"ActiveDisableSweep",tolua_SkillContext_ActiveDisableSweep00);
  tolua_function(tolua_S,"ActiveExtendEnemyWaitTurn",tolua_SkillContext_ActiveExtendEnemyWaitTurn00);
  tolua_function(tolua_S,"ActiveSortGem",tolua_SkillContext_ActiveSortGem00);
  tolua_function(tolua_S,"ActiveMoveAgain",tolua_SkillContext_ActiveMoveAgain00);
  tolua_function(tolua_S,"ActiveInstantEnergy",tolua_SkillContext_ActiveInstantEnergy00);
  tolua_function(tolua_S,"ActiveDispelPassiveBuff",tolua_SkillContext_ActiveDispelPassiveBuff00);
  tolua_function(tolua_S,"ActiveDamage",tolua_SkillContext_ActiveDamage00);
  tolua_function(tolua_S,"ActiveDamageByValue",tolua_SkillContext_ActiveDamageByValue00);
  tolua_function(tolua_S,"ActiveDamageByPlayerHealthFactor",tolua_SkillContext_ActiveDamageByPlayerHealthFactor00);
  tolua_function(tolua_S,"ActiveDamageByHeroHealthFactor",tolua_SkillContext_ActiveDamageByHeroHealthFactor00);
  tolua_function(tolua_S,"ActiveDamageByFactorFromColor",tolua_SkillContext_ActiveDamageByFactorFromColor00);
  tolua_function(tolua_S,"ActiveDamageByFactorFromTeam",tolua_SkillContext_ActiveDamageByFactorFromTeam00);
  tolua_function(tolua_S,"ActiveDamageByFactor",tolua_SkillContext_ActiveDamageByFactor00);
  tolua_function(tolua_S,"ActiveDamageByFactorWithoutArmor",tolua_SkillContext_ActiveDamageByFactorWithoutArmor00);
  tolua_function(tolua_S,"ActiveDamageByEnemyHealthFactor",tolua_SkillContext_ActiveDamageByEnemyHealthFactor00);
  tolua_function(tolua_S,"ActiveDamageAndHealByFactor",tolua_SkillContext_ActiveDamageAndHealByFactor00);
  tolua_function(tolua_S,"ActiveDamageByFactorAndEnemyHealth",tolua_SkillContext_ActiveDamageByFactorAndEnemyHealth00);
  tolua_function(tolua_S,"ActiveDamageBySelfDamageFactor",tolua_SkillContext_ActiveDamageBySelfDamageFactor00);
  tolua_function(tolua_S,"ActiveDamageByGems",tolua_SkillContext_ActiveDamageByGems00);
  tolua_function(tolua_S,"ActiveMultipleAttacksByFactor",tolua_SkillContext_ActiveMultipleAttacksByFactor00);
  tolua_function(tolua_S,"ActiveLeechByFactor",tolua_SkillContext_ActiveLeechByFactor00);
  tolua_function(tolua_S,"ActiveHeal",tolua_SkillContext_ActiveHeal00);
  tolua_function(tolua_S,"ActiveHealByFactor",tolua_SkillContext_ActiveHealByFactor00);
  tolua_function(tolua_S,"ActiveHealFull",tolua_SkillContext_ActiveHealFull00);
  tolua_function(tolua_S,"ActiveBoostAttackByColor",tolua_SkillContext_ActiveBoostAttackByColor00);
  tolua_function(tolua_S,"ActiveBoostAttackAll",tolua_SkillContext_ActiveBoostAttackAll00);
  tolua_function(tolua_S,"ActiveBoostComboFactor",tolua_SkillContext_ActiveBoostComboFactor00);
  tolua_function(tolua_S,"ActiveBoostMaxHealth",tolua_SkillContext_ActiveBoostMaxHealth00);
  tolua_function(tolua_S,"ActiveBoostEnergyByColor",tolua_SkillContext_ActiveBoostEnergyByColor00);
  tolua_function(tolua_S,"ActiveAmplifyDamageByFactor",tolua_SkillContext_ActiveAmplifyDamageByFactor00);
  tolua_function(tolua_S,"ActivePoisonByValue",tolua_SkillContext_ActivePoisonByValue00);
  tolua_function(tolua_S,"ActivePoisonByFactor",tolua_SkillContext_ActivePoisonByFactor00);
  tolua_function(tolua_S,"ActiveBlind",tolua_SkillContext_ActiveBlind00);
  tolua_function(tolua_S,"ActiveBlindHeroRandom",tolua_SkillContext_ActiveBlindHeroRandom00);
  tolua_function(tolua_S,"ActiveBlindHeroByColor",tolua_SkillContext_ActiveBlindHeroByColor00);
  tolua_function(tolua_S,"ActiveSilence",tolua_SkillContext_ActiveSilence00);
  tolua_function(tolua_S,"ActiveReduceEnemyArmorByFactor",tolua_SkillContext_ActiveReduceEnemyArmorByFactor00);
  tolua_function(tolua_S,"ActiveWeakenEnemyAttackByFactor",tolua_SkillContext_ActiveWeakenEnemyAttackByFactor00);
  tolua_function(tolua_S,"ActiveReflectByFactor",tolua_SkillContext_ActiveReflectByFactor00);
  tolua_function(tolua_S,"ActiveReduceDamageByFactor",tolua_SkillContext_ActiveReduceDamageByFactor00);
  tolua_function(tolua_S,"ActiveLeechByAttack",tolua_SkillContext_ActiveLeechByAttack00);
  tolua_function(tolua_S,"ActiveShield",tolua_SkillContext_ActiveShield00);
  tolua_function(tolua_S,"ActiveInvincible",tolua_SkillContext_ActiveInvincible00);
  tolua_function(tolua_S,"ActiveReduceEnergyCost",tolua_SkillContext_ActiveReduceEnergyCost00);
  tolua_function(tolua_S,"ActiveReduceCooldownTime",tolua_SkillContext_ActiveReduceCooldownTime00);
  tolua_function(tolua_S,"ActiveAbsorbDamage",tolua_SkillContext_ActiveAbsorbDamage00);
  tolua_function(tolua_S,"ActiveTriggerCriticalAttack",tolua_SkillContext_ActiveTriggerCriticalAttack00);
  tolua_function(tolua_S,"ActiveSuckBloodFromEnemyByRegen",tolua_SkillContext_ActiveSuckBloodFromEnemyByRegen00);
  tolua_function(tolua_S,"ActiveHealByColor",tolua_SkillContext_ActiveHealByColor00);
  tolua_function(tolua_S,"ActiveHealByMaxHealth",tolua_SkillContext_ActiveHealByMaxHealth00);
  tolua_function(tolua_S,"ActiveReduceAttackByFactor",tolua_SkillContext_ActiveReduceAttackByFactor00);
  tolua_function(tolua_S,"ActiveReduceResistance",tolua_SkillContext_ActiveReduceResistance00);
  tolua_function(tolua_S,"ActiveReduceDamageTakenByFactor",tolua_SkillContext_ActiveReduceDamageTakenByFactor00);
  tolua_function(tolua_S,"ActiveReduceOpponentDragTime",tolua_SkillContext_ActiveReduceOpponentDragTime00);
  tolua_function(tolua_S,"ActiveZombine",tolua_SkillContext_ActiveZombine00);
  tolua_function(tolua_S,"ActiveFightBack",tolua_SkillContext_ActiveFightBack00);
  tolua_function(tolua_S,"ActiveForceAreaAttack",tolua_SkillContext_ActiveForceAreaAttack00);
  tolua_function(tolua_S,"ActiveBloodForBlood",tolua_SkillContext_ActiveBloodForBlood00);
  tolua_function(tolua_S,"ActiveEyeForEye",tolua_SkillContext_ActiveEyeForEye00);
  tolua_function(tolua_S,"ActiveBonusGemWeight",tolua_SkillContext_ActiveBonusGemWeight00);
  tolua_function(tolua_S,"ActiveBoostEnergyByFactorByColor",tolua_SkillContext_ActiveBoostEnergyByFactorByColor00);
  tolua_function(tolua_S,"PassiveReflectByFactor",tolua_SkillContext_PassiveReflectByFactor00);
  tolua_function(tolua_S,"PassiveAttackFactor",tolua_SkillContext_PassiveAttackFactor00);
  tolua_function(tolua_S,"PassiveAttackFactorByTitle",tolua_SkillContext_PassiveAttackFactorByTitle00);
  tolua_function(tolua_S,"PassiveRandomAttackFactor",tolua_SkillContext_PassiveRandomAttackFactor00);
  tolua_function(tolua_S,"PassiveRegenFactor",tolua_SkillContext_PassiveRegenFactor00);
  tolua_function(tolua_S,"PassiveDamageReduceFactor",tolua_SkillContext_PassiveDamageReduceFactor00);
  tolua_function(tolua_S,"PassiveFollowUpAttack",tolua_SkillContext_PassiveFollowUpAttack00);
  tolua_function(tolua_S,"PassiveSurviveCriticalHitFactor",tolua_SkillContext_PassiveSurviveCriticalHitFactor00);
  tolua_function(tolua_S,"PassiveHealByFactor",tolua_SkillContext_PassiveHealByFactor00);
  tolua_function(tolua_S,"PassiveSetBonusHealthByFactor",tolua_SkillContext_PassiveSetBonusHealthByFactor00);
  tolua_function(tolua_S,"PassiveSetBonusHealthByFactorByTitle",tolua_SkillContext_PassiveSetBonusHealthByFactorByTitle00);
  tolua_function(tolua_S,"PassiveBonusDragTime",tolua_SkillContext_PassiveBonusDragTime00);
  tolua_function(tolua_S,"PassiveSetEnergyFactor",tolua_SkillContext_PassiveSetEnergyFactor00);
  tolua_function(tolua_S,"PassiveSetMinEnergy",tolua_SkillContext_PassiveSetMinEnergy00);
  tolua_function(tolua_S,"PassiveSetComboFactor",tolua_SkillContext_PassiveSetComboFactor00);
  tolua_function(tolua_S,"PassiveReduceDamageOnHealth",tolua_SkillContext_PassiveReduceDamageOnHealth00);
  tolua_function(tolua_S,"PassiveBoostAttackOnHealth",tolua_SkillContext_PassiveBoostAttackOnHealth00);
  tolua_function(tolua_S,"PassiveBleedingByFactor",tolua_SkillContext_PassiveBleedingByFactor00);
  tolua_function(tolua_S,"PassiveBonusCoins",tolua_SkillContext_PassiveBonusCoins00);
  tolua_function(tolua_S,"PassiveBonusExp",tolua_SkillContext_PassiveBonusExp00);
  tolua_function(tolua_S,"PassiveBonusLuck",tolua_SkillContext_PassiveBonusLuck00);
  tolua_function(tolua_S,"PassiveBonusEnemyStartTurn",tolua_SkillContext_PassiveBonusEnemyStartTurn00);
  tolua_function(tolua_S,"PassiveSetAreaAttackAndFactor",tolua_SkillContext_PassiveSetAreaAttackAndFactor00);
  tolua_function(tolua_S,"PassiveSetGemWeight",tolua_SkillContext_PassiveSetGemWeight00);
  tolua_function(tolua_S,"PassiveSetMinComboCount",tolua_SkillContext_PassiveSetMinComboCount00);
  tolua_function(tolua_S,"PassiveDamageToHealthByFactor",tolua_SkillContext_PassiveDamageToHealthByFactor00);
  tolua_function(tolua_S,"PassiveSetEnergyBonusByGemCount",tolua_SkillContext_PassiveSetEnergyBonusByGemCount00);
  tolua_function(tolua_S,"PassiveNothingTodo",tolua_SkillContext_PassiveNothingTodo00);
  tolua_function(tolua_S,"PassiveSetArmorByHealthRatio",tolua_SkillContext_PassiveSetArmorByHealthRatio00);
  tolua_function(tolua_S,"PassiveSetExtraWaitForEnemy",tolua_SkillContext_PassiveSetExtraWaitForEnemy00);
  tolua_function(tolua_S,"PassiveSetBonusGemInAreaAttackCheck",tolua_SkillContext_PassiveSetBonusGemInAreaAttackCheck00);
  tolua_function(tolua_S,"PassiveFollowUpAttackWithoutResistence",tolua_SkillContext_PassiveFollowUpAttackWithoutResistence00);
  tolua_function(tolua_S,"PassiveSetInvicible",tolua_SkillContext_PassiveSetInvicible00);
  tolua_function(tolua_S,"PassiveShield",tolua_SkillContext_PassiveShield00);
  tolua_function(tolua_S,"PassiveReduceOpponentDragTime",tolua_SkillContext_PassiveReduceOpponentDragTime00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_SkillContext (lua_State* tolua_S) {
 return tolua_SkillContext_open(tolua_S);
};
#endif

