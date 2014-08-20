//
//  Header.h
//  HelloCpp
//
//  Created by Zeyang Li on 10/24/12.
//
//

#pragma once

int luaopen_Grid2D(lua_State* s);
int luaopen_Gem(lua_State* s);
int luaopen_EnemyControl(lua_State* s);
int luaopen_PlayerControl(lua_State* s);
int luaopen_HeroControl(lua_State* s);
int luaopen_EnemySkill(lua_State* s);
int luaopen_SkillContext(lua_State* tolua_S);
