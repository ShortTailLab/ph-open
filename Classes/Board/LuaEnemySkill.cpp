/*
** Lua binding: EnemySkill
** Generated automatically by tolua++-1.0.92 on Wed Oct 23 16:26:25 2013.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_EnemySkill_open (lua_State* tolua_S);

#include "EnemySkill.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
}

/* function: EnemyHeal */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyHeal00
static int tolua_EnemySkill_EnemyHeal00(lua_State* tolua_S)
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
   EnemyHeal(val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyHeal'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyPoison */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyPoison00
static int tolua_EnemySkill_EnemyPoison00(lua_State* tolua_S)
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
  int turn = ((int)  tolua_tonumber(tolua_S,1,0));
  float val = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   EnemyPoison(turn,val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyPoison'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyDrainEnergyByColor */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyDrainEnergyByColor00
static int tolua_EnemySkill_EnemyDrainEnergyByColor00(lua_State* tolua_S)
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
  float val = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   EnemyDrainEnergyByColor(color,val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyDrainEnergyByColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyDrainEnergyRandomHero */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyDrainEnergyRandomHero00
static int tolua_EnemySkill_EnemyDrainEnergyRandomHero00(lua_State* tolua_S)
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
  float val = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   EnemyDrainEnergyRandomHero(count,val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyDrainEnergyRandomHero'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyBombGemsByColor */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyBombGemsByColor00
static int tolua_EnemySkill_EnemyBombGemsByColor00(lua_State* tolua_S)
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
  int maxCount = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   EnemyBombGemsByColor(color,maxCount);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyBombGemsByColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyLockGems */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyLockGems00
static int tolua_EnemySkill_EnemyLockGems00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_iscppstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,1,0));
  int count = ((int)  tolua_tonumber(tolua_S,2,0));
  int turn = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   EnemyLockGems(color,count,turn);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyLockGems'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyAttachBombToGems */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyAttachBombToGems00
static int tolua_EnemySkill_EnemyAttachBombToGems00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_iscppstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,1,0));
  int count = ((int)  tolua_tonumber(tolua_S,2,0));
  float damage = ((float)  tolua_tonumber(tolua_S,3,0));
  int turn = ((int)  tolua_tonumber(tolua_S,4,0));
  {
   EnemyAttachBombToGems(color,count,damage,turn);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyAttachBombToGems'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyAttachHandToGems */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyAttachHandToGems00
static int tolua_EnemySkill_EnemyAttachHandToGems00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_iscppstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,1,0));
  int count = ((int)  tolua_tonumber(tolua_S,2,0));
  float damage = ((float)  tolua_tonumber(tolua_S,3,0));
  int turn = ((int)  tolua_tonumber(tolua_S,4,0));
  {
   EnemyAttachHandToGems(color,count,damage,turn);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyAttachHandToGems'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyBlindHeroRandom */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyBlindHeroRandom00
static int tolua_EnemySkill_EnemyBlindHeroRandom00(lua_State* tolua_S)
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
  int turn = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   EnemyBlindHeroRandom(count,turn);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyBlindHeroRandom'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyBlindHeroByColor */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyBlindHeroByColor00
static int tolua_EnemySkill_EnemyBlindHeroByColor00(lua_State* tolua_S)
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
  int turn = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   EnemyBlindHeroByColor(color,turn);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyBlindHeroByColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemySlowByFactor */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemySlowByFactor00
static int tolua_EnemySkill_EnemySlowByFactor00(lua_State* tolua_S)
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
  int turn = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   EnemySlowByFactor(factor,turn);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemySlowByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyReduceHealingByFactor */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyReduceHealingByFactor00
static int tolua_EnemySkill_EnemyReduceHealingByFactor00(lua_State* tolua_S)
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
  int turn = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   EnemyReduceHealingByFactor(factor,turn);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyReduceHealingByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyReflectByFactor */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyReflectByFactor00
static int tolua_EnemySkill_EnemyReflectByFactor00(lua_State* tolua_S)
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
  int turn = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   EnemyReflectByFactor(factor,turn);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyReflectByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyReflectByGems */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyReflectByGems00
static int tolua_EnemySkill_EnemyReflectByGems00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_iscppstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  int count = ((int)  tolua_tonumber(tolua_S,3,0));
  int turn = ((int)  tolua_tonumber(tolua_S,4,0));
  {
   EnemyReflectByGems(color,factor,count,turn);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyReflectByGems'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyInjuryFreeExceptDarts */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyInjuryFreeExceptDarts00
static int tolua_EnemySkill_EnemyInjuryFreeExceptDarts00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_iscppstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,1,0));
  int count = ((int)  tolua_tonumber(tolua_S,2,0));
  int turn = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   EnemyInjuryFreeExceptDarts(color,count,turn);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyInjuryFreeExceptDarts'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemySpreadPlague */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemySpreadPlague00
static int tolua_EnemySkill_EnemySpreadPlague00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_iscppstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,1,0));
  float damage = ((float)  tolua_tonumber(tolua_S,2,0));
  int count = ((int)  tolua_tonumber(tolua_S,3,0));
  int infectionLimitPerGems = ((int)  tolua_tonumber(tolua_S,4,0));
  int globalLimit = ((int)  tolua_tonumber(tolua_S,5,0));
  int turn = ((int)  tolua_tonumber(tolua_S,6,0));
  {
   EnemySpreadPlague(color,damage,count,infectionLimitPerGems,globalLimit,turn);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemySpreadPlague'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyChangeAttributeByColor */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyChangeAttributeByColor00
static int tolua_EnemySkill_EnemyChangeAttributeByColor00(lua_State* tolua_S)
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
  int turn = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   EnemyChangeAttributeByColor(color,turn);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyChangeAttributeByColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemySpeaking */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemySpeaking00
static int tolua_EnemySkill_EnemySpeaking00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_iscppstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::string content = ((std::string)  tolua_tocppstring(tolua_S,1,0));
  {
   EnemySpeaking(content);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemySpeaking'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyHealAllTheEnemy */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyHealAllTheEnemy00
static int tolua_EnemySkill_EnemyHealAllTheEnemy00(lua_State* tolua_S)
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
   EnemyHealAllTheEnemy(val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyHealAllTheEnemy'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyLeechByFactor */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyLeechByFactor00
static int tolua_EnemySkill_EnemyLeechByFactor00(lua_State* tolua_S)
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
  int duration = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   EnemyLeechByFactor(factor,duration);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyLeechByFactor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyDamageByCount */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyDamageByCount00
static int tolua_EnemySkill_EnemyDamageByCount00(lua_State* tolua_S)
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
  float count = ((float)  tolua_tonumber(tolua_S,1,0));
  float damage = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   EnemyDamageByCount(count,damage);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyDamageByCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemySetShield */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemySetShield00
static int tolua_EnemySkill_EnemySetShield00(lua_State* tolua_S)
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
  float damage = ((float)  tolua_tonumber(tolua_S,1,0));
  float factor = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   EnemySetShield(damage,factor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemySetShield'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyPassiveInjuryFree */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyPassiveInjuryFree00
static int tolua_EnemySkill_EnemyPassiveInjuryFree00(lua_State* tolua_S)
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
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  float probality = ((float)  tolua_tonumber(tolua_S,2,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,3,0));
  {
   EnemyPassiveInjuryFree(factor,probality,color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyPassiveInjuryFree'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyPassiveReflect */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyPassiveReflect00
static int tolua_EnemySkill_EnemyPassiveReflect00(lua_State* tolua_S)
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
  float factor = ((float)  tolua_tonumber(tolua_S,1,0));
  float probality = ((float)  tolua_tonumber(tolua_S,2,0));
  std::string color = ((std::string)  tolua_tocppstring(tolua_S,3,0));
  bool takeDamage = ((bool)  tolua_toboolean(tolua_S,4,0));
  {
   EnemyPassiveReflect(factor,probality,color,takeDamage);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyPassiveReflect'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyPassiveHound */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyPassiveHound00
static int tolua_EnemySkill_EnemyPassiveHound00(lua_State* tolua_S)
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
  float damage = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   EnemyPassiveHound(count,damage);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyPassiveHound'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EnemyPassiveReborn */
#ifndef TOLUA_DISABLE_tolua_EnemySkill_EnemyPassiveReborn00
static int tolua_EnemySkill_EnemyPassiveReborn00(lua_State* tolua_S)
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
  float healthFactor = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   EnemyPassiveReborn(count,healthFactor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnemyPassiveReborn'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_EnemySkill_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_function(tolua_S,"EnemyHeal",tolua_EnemySkill_EnemyHeal00);
  tolua_function(tolua_S,"EnemyPoison",tolua_EnemySkill_EnemyPoison00);
  tolua_function(tolua_S,"EnemyDrainEnergyByColor",tolua_EnemySkill_EnemyDrainEnergyByColor00);
  tolua_function(tolua_S,"EnemyDrainEnergyRandomHero",tolua_EnemySkill_EnemyDrainEnergyRandomHero00);
  tolua_function(tolua_S,"EnemyBombGemsByColor",tolua_EnemySkill_EnemyBombGemsByColor00);
  tolua_function(tolua_S,"EnemyLockGems",tolua_EnemySkill_EnemyLockGems00);
  tolua_function(tolua_S,"EnemyAttachBombToGems",tolua_EnemySkill_EnemyAttachBombToGems00);
  tolua_function(tolua_S,"EnemyAttachHandToGems",tolua_EnemySkill_EnemyAttachHandToGems00);
  tolua_function(tolua_S,"EnemyBlindHeroRandom",tolua_EnemySkill_EnemyBlindHeroRandom00);
  tolua_function(tolua_S,"EnemyBlindHeroByColor",tolua_EnemySkill_EnemyBlindHeroByColor00);
  tolua_function(tolua_S,"EnemySlowByFactor",tolua_EnemySkill_EnemySlowByFactor00);
  tolua_function(tolua_S,"EnemyReduceHealingByFactor",tolua_EnemySkill_EnemyReduceHealingByFactor00);
  tolua_function(tolua_S,"EnemyReflectByFactor",tolua_EnemySkill_EnemyReflectByFactor00);
  tolua_function(tolua_S,"EnemyReflectByGems",tolua_EnemySkill_EnemyReflectByGems00);
  tolua_function(tolua_S,"EnemyInjuryFreeExceptDarts",tolua_EnemySkill_EnemyInjuryFreeExceptDarts00);
  tolua_function(tolua_S,"EnemySpreadPlague",tolua_EnemySkill_EnemySpreadPlague00);
  tolua_function(tolua_S,"EnemyChangeAttributeByColor",tolua_EnemySkill_EnemyChangeAttributeByColor00);
  tolua_function(tolua_S,"EnemySpeaking",tolua_EnemySkill_EnemySpeaking00);
  tolua_function(tolua_S,"EnemyHealAllTheEnemy",tolua_EnemySkill_EnemyHealAllTheEnemy00);
  tolua_function(tolua_S,"EnemyLeechByFactor",tolua_EnemySkill_EnemyLeechByFactor00);
  tolua_function(tolua_S,"EnemyDamageByCount",tolua_EnemySkill_EnemyDamageByCount00);
  tolua_function(tolua_S,"EnemySetShield",tolua_EnemySkill_EnemySetShield00);
  tolua_function(tolua_S,"EnemyPassiveInjuryFree",tolua_EnemySkill_EnemyPassiveInjuryFree00);
  tolua_function(tolua_S,"EnemyPassiveReflect",tolua_EnemySkill_EnemyPassiveReflect00);
  tolua_function(tolua_S,"EnemyPassiveHound",tolua_EnemySkill_EnemyPassiveHound00);
  tolua_function(tolua_S,"EnemyPassiveReborn",tolua_EnemySkill_EnemyPassiveReborn00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_EnemySkill (lua_State* tolua_S) {
 return tolua_EnemySkill_open(tolua_S);
};
#endif

