#!/bin/bash
TL="../../tools/tolua++-1.0.93/bin/tolua++"
$TL Gem.h > LuaGem.cpp
$TL Grid2D.h > LuaGrid2D.cpp
$TL PlayerControl.h > LuaPlayerControl.cpp
$TL EnemyControl.h > LuaEnemyControl.cpp
$TL HeroControl.h > LuaHeroControl.cpp
$TL EnemySkill.h > LuaEnemySkill.cpp
$TL SkillContext.h > LuaSkillContext.cpp
$TL SerializationUtils.h > LuaSerializationUtils.cpp
#$TL Board.h > LuaBoard.cpp
#$TL Task.h > LuaTask.cpp
#$TL HeroTable.h > LuaHeroTable.cpp
