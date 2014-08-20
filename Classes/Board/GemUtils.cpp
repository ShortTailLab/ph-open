//
//  GemUtils.cpp
//  HelloCpp
//
//  Created by 毛 靖凯 on 13-9-2.
//
//

#include "GemUtils.h"
#include "Utils.h"
#include <boost/algorithm/string.hpp>
#include <extensions/utils.h>

using namespace PH;

static void handleUnknownColor()
{
    printf("Unknown color detected\n");
    assert(false && "unknown color");
}

std::string GemUtils::getNameWithPatternAndColor(std::string pattern,
                                                 GemUtils::GemColor color)
{
    char name[128];
    sprintf(name, pattern.c_str(), GemUtils::colorToStr(color));
    return std::string(name);
}

std::string GemUtils::nameCN(GemColor c)
{
    switch(c)
    {
        case Fire: return "火";
        case Water: return "水";
        case Wood: return "木";
        case Dark: return "暗";
        case Light: return "光";
        case Health: return "生命";
        case AllColor:
        case GemColorMax:
            handleUnknownColor();
            break;
    }
    throw "Unknown gem color";
}

const char* GemUtils::res(GemColor c)
{
    switch(c)
    {
        case Fire: return "gem_fire.png";
        case Water: return "gem_water.png";
        case Wood: return "gem_wood.png";
        case Dark: return "gem_dark.png";
        case Light: return "gem_light.png";
        case Health: return "gem_health.png";
        case AllColor:
        case GemColorMax:
            handleUnknownColor();
            break;
    }
    
    throw "Unknown gem color";
}

const char* GemUtils::stageColorRes(GemColor c)
{
    switch(c)
    {
        case Fire: return "zhanchangshuxing_huo.png";
        case Water: return "zhanchangshuxing_shui.png";
        case Wood: return "zhanchangshuxing_mu.png";
        case Dark: return "zhanchangshuxing_an.png";
        case Light: return "zhanchangshuxing_guang.png";
        case AllColor: return "zhanchangshuxing_za.png";
            
            // unsupported
        case Health:
        case GemColorMax:
            handleUnknownColor();
            break;
    }
    
    throw "Unknown stage color";
}

const char* GemUtils::frame(GemColor c)
{
    switch(c)
    {
        case Fire: return "frame_fire.png";
        case Water: return "frame_water.png";
        case Wood: return "frame_wood.png";
        case Dark: return "frame_dark.png";
        case Light: return "frame_light.png";
        case Health:
        case AllColor:
        case GemColorMax:
            handleUnknownColor();
            break;
    }
    
    throw "Unknown Gem Color";
}

const char* GemUtils::healthBox(GemColor c)
{
    switch(c)
    {
        case Fire: return "ui/fire_health.png";
        case Water: return "ui/water_health.png";
        case Wood: return "ui/wood_health.png";
        case Dark: return "ui/dark_health.png";
        case Light: return "ui/light_health.png";
        case Health:
        case AllColor:
        case GemColorMax:
            handleUnknownColor();
            break;
    }
    
    throw "Unknown Gem Color";
}

const char * GemUtils::healthBox(GemColor c, bool isLong){
    if(!isLong) return GemUtils::healthBox(c);
    switch(c)
    {
        case Fire:   return "ui/fire_health_long.png";
        case Water:  return "ui/water_health_long.png";
        case Wood:   return "ui/wood_health_long.png";
        case Dark:   return "ui/dark_health_long.png";
        case Light:  return "ui/light_health_long.png";
        case Health:
        case AllColor:
        case GemColorMax:
            handleUnknownColor();
            break;
    }
    
    throw "Unknown Gem Color";
}

const char* GemUtils::numberFont(GemColor c)
{
    switch(c)
    {
        case Fire: return "bmfont/Molot_30_red_gradient.fnt";
        case Water: return "bmfont/Molot_30_blue_gradient.fnt";
        case Wood: return "bmfont/Molot_30_green_gradient.fnt";
        case Dark: return "bmfont/Molot_30_purple_gradient.fnt";
        case Light: return "bmfont/Molot_30_yellow_gradient.fnt";
        case Health: return "bmfont/Molot_30_pink_gradient.fnt";
        case AllColor:
        case GemColorMax:
            handleUnknownColor();
    }
    
    throw "Unknown Gem Color";
}

const char* GemUtils::resistanceFont(GemColor c)
{
    switch(c)
    {
        case Fire: return "bmfont/Sumo_red_22.fnt";
        case Water: return "bmfont/Sumo_blue_22.fnt";
        case Wood: return "bmfont/Sumo_green_22.fnt";
        case Dark: return "bmfont/Sumo_purple_22.fnt";
        case Light: return "bmfont/Sumo_yellow_22.fnt";
        case Health:
        case AllColor:
        case GemColorMax:
            handleUnknownColor();
            break;
    }
    
    throw "Unknown Gem Color";
}

const char* GemUtils::healthFill(GemColor c)
{
    switch(c)
    {
        case Fire: return "ui/fire_health_fill.png";
        case Water: return "ui/water_health_fill.png";
        case Wood: return "ui/wood_health_fill.png";
        case Dark: return "ui/dark_health_fill.png";
        case Light: return "ui/light_health_fill.png";
        case Health:
        case AllColor:
        case GemColorMax:
            handleUnknownColor();
    }
    
    throw "Unknown Gem Color";
}

const char* GemUtils::healthFill(GemColor c, bool isLong) {
    if(!isLong) return healthFill(c);
    switch(c)
    {
        case Fire:   return "ui/fire_health_fill_long.png";
        case Water:  return "ui/water_health_fill_long.png";
        case Wood:   return "ui/wood_health_fill_long.png";
        case Dark:   return "ui/dark_health_fill_long.png";
        case Light:  return "ui/light_health_fill_long.png";
        case Health:
        case AllColor:
        case GemColorMax:
            handleUnknownColor();
    }
    
    throw "Unknown Gem Color";
}

const char* GemUtils::name(GemColor c)
{
    switch(c)
    {
        case Fire: return "Fire";
        case Water: return "Water";
        case Wood: return "Wood";
        case Dark: return "Dark";
        case Light: return "Light";
        case Health: return "Health";
        case GemColorMax: return "Gem Color Max";
        case AllColor: return "AllColor";
    }
    
    return "Unknown Gem Color";
}

CCSpriteFrame* GemUtils::energy(GemColor c, int i)
{
    std::string n = name(c);
    boost::to_lower(n);
    
    std::string path = n + "_energy_" + toStr(i) + ".png";
    
    return CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(path.c_str());
}

const char* GemUtils::attackSound(GemColor c)
{
    switch(c)
    {
        case Fire: return "sound/v/fire_attack.mp3";
        case Water: return "sound/v/water_attack.mp3";
        case Wood: return "sound/v/wood_attack.mp3";
        case Dark: return "sound/v/dark_attack.mp3";
        case Light: return "sound/v/light_attack.mp3";
        case Health:
        case AllColor:
        case GemColorMax:
            handleUnknownColor();
            break;
    }
    return NULL;
}

const char* GemUtils::attackAnim(GemColor c)
{
    switch(c)
    {
        case Fire: return "fire_attack_anim";
        case Water: return "water_attack_anim";
        case Wood: return "wood_attack_anim";
        case Dark: return "dark_attack_anim";
        case Light: return "light_attack_anim";
        case Health:
        case AllColor:
        case GemColorMax:
            handleUnknownColor();
            break;
    }
    return NULL;
}

GemUtils::GemColor GemUtils::strToColor(std::string str)
{
    boost::algorithm::to_lower(str);
    
    if(str == "fire")         return Fire;
    else if (str == "water")  return Water;
    else if (str == "wood")   return Wood;
    else if (str == "dark")   return Dark;
    else if (str == "light")  return Light;
    else if (str == "health") return Health;
    else if (str == "allcolor") return AllColor;
    
    phassert(false && "strToColor failed on unknown string");
    return GemColorMax;
}

const char * GemUtils::colorToStr(PH::GemUtils::GemColor c)
{
    switch(c)
    {
        case Fire: return "fire";
        case Water: return "water";
        case Wood: return "wood";
        case Dark: return "dark";
        case Light: return "light";
        case Health: return "health";
        case AllColor:
        case GemColorMax:
            handleUnknownColor();
            break;
    }
    return NULL;
}

ccColor3B GemUtils::colorToCCColor(PH::GemUtils::GemColor c)
{
    switch(c)
    {
        case Fire: return ccc3(250,10,20);
        case Water: return ccc3(45, 175, 204);
        case Wood: return ccc3(48, 205, 58);
        case Dark: return ccc3(150,30,250);
        case Light: return ccc3(220,200,20);
        case Health: return ccc3(240,0,250);
        case AllColor:
        case GemColorMax:
            handleUnknownColor();
            break;
    }
    return ccc3(0,0,0);
}

GemUtils::GemColor GemUtils::weak(PH::GemUtils::GemColor l)
{
    switch(l)
    {
        case Fire: return Wood;
        case Water: return Fire;
        case Wood: return Water;
        case Light: return Dark;
        case Dark: return Light;
        default:
            handleUnknownColor();
            break;
    }
    return GemColorMax;
}