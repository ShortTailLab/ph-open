#include "BattleScene.h"
#include "Board/Common.h"
#include "Board/Board.h"
#include "PlayerData.h"
#include "TouchMaskLayer.h"
#include "TutorialBoard.h"
#include "PVPBoard.h"
#include "SimpleAudioEngine.h"
#include "CCNative.h"
#include <extensions/utils.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>

namespace PH
{
    
using namespace std;
    
std::string Control::getRewardsString() const
{
    return rewards.html;
//    vector<std::string> entries;
//    transform(rewards.helpedUsers.begin(), rewards.helpedUsers.end(), back_inserter(entries), [](map<string, int>::const_reference kv)
//    {
//        return boost::algorithm::trim_copy(kv.first) + "\t\t" + boost::lexical_cast<string>(kv.second);
//    });
//    return boost::algorithm::join(entries, "\n");
}
    
CCAnimation* createAttackAnimFromColor(GemUtils::GemColor c, int numFrames)
{
    CCAnimation* anim = CCAnimation::create();
    
    std::string colorStr = GemUtils::name(c);
    boost::algorithm::to_lower(colorStr);
    
    for(int i=1; i<=numFrames; ++i)
    {
        char name[128];
        sprintf(name, "%s_000%.2d.png", colorStr.c_str(), i);
        CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
        
        assert(frame != NULL && "cannot find frames");
        
        anim->addSpriteFrame(frame);
    }
    
    return anim;
}

CCAnimation * createAnimationWithPattern(std::string pattern, int numFrames)
{
    CCAnimation* anim = CCAnimation::create(); 
    
    for(int i=1; i<=numFrames; ++i)
    {
        char name[128];
        sprintf(name, pattern.c_str(), i);
        CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
        
        assert(frame != NULL && "cannot find frames");
        
        anim->addSpriteFrame(frame);
    }

    
    return anim;
}

void BattleScene::loadBattleResources()
{
    // manually create animation caches
    {
        // preload assets
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("effects/light_dark_attack_packed.plist");
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("effects/water_wood_attack_packed.plist");
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("effects/fire_attack_packed.plist");
    
        CCAnimation* anim;
        anim = createAttackAnimFromColor(GemUtils::Fire, 20);
        anim->setDelayPerUnit(0.05);
        CCAnimationCache::sharedAnimationCache()->addAnimation(anim, GemUtils::attackAnim(GemUtils::Fire));
        
        anim = createAttackAnimFromColor(GemUtils::Water, 23);
        anim->setDelayPerUnit(0.05);
        CCAnimationCache::sharedAnimationCache()->addAnimation(anim, GemUtils::attackAnim(GemUtils::Water));
        
        anim = createAttackAnimFromColor(GemUtils::Wood, 17);
        anim->setDelayPerUnit(0.05);
        CCAnimationCache::sharedAnimationCache()->addAnimation(anim, GemUtils::attackAnim(GemUtils::Wood));
        
        anim = createAttackAnimFromColor(GemUtils::Light, 25);
        anim->setDelayPerUnit(0.05);
        CCAnimationCache::sharedAnimationCache()->addAnimation(anim, GemUtils::attackAnim(GemUtils::Light));
        
        anim = createAttackAnimFromColor(GemUtils::Dark, 19);
        anim->setDelayPerUnit(0.05);
        CCAnimationCache::sharedAnimationCache()->addAnimation(anim, GemUtils::attackAnim(GemUtils::Dark));
    }
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("battleMisc.plist");
    {
        // preload skill asserts
        for (int i=0;i<100;i++)
        {
            string plistName = "skill" + toStr(i) +".plist";
            if (PH::resourcePresentForLoading(plistName))
                CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(plistName.c_str());
            else
                break;
        }
        
        CCAnimation * skill_anim;
        skill_anim = createAnimationWithPattern("expl_%d.png", 9);
        skill_anim->setDelayPerUnit(0.08f);
        CCAnimationCache::sharedAnimationCache()->addAnimation(skill_anim, "skill_explosion");

        skill_anim = createAnimationWithPattern("chain_%d.png", 6);
        skill_anim->setDelayPerUnit(0.08f);
        CCAnimationCache::sharedAnimationCache()->addAnimation(skill_anim, "skill_chain");
        
        skill_anim = createAnimationWithPattern("wenyiguang%d.png", 9);
        skill_anim->setDelayPerUnit(0.08f);
        CCAnimationCache::sharedAnimationCache()->addAnimation(skill_anim, "skill_plague");
        
        skill_anim = createAnimationWithPattern("fuze_%d.png", 10);
        skill_anim->setDelayPerUnit(0.08f);
        CCAnimationCache::sharedAnimationCache()->addAnimation(skill_anim, "skill_bomb_fuze");

        skill_anim = createAnimationWithPattern("broken_shield_%d.png", 9);
        skill_anim->setDelayPerUnit(0.08f);
        CCAnimationCache::sharedAnimationCache()->addAnimation(skill_anim, "skill_broken_shield");
        
        skill_anim = createAnimationWithPattern("reborn_%d.png", 12);
        skill_anim->setDelayPerUnit(0.08f);
        CCAnimationCache::sharedAnimationCache()->addAnimation(skill_anim, "skill_reborn");
        
        skill_anim = createAnimationWithPattern("fadong_0%d.png", 7);
        skill_anim->setDelayPerUnit(0.08f);
        CCAnimationCache::sharedAnimationCache()->addAnimation(skill_anim, "skill_trigger");
    }
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("sound/bg/boss.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("sound/bg/battle.mp3");
}

void unloadBattleResources()
{
    CCAnimationCache::sharedAnimationCache()->purgeSharedAnimationCache();
    
    // unload battle scene only sprite frames
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("effects/fire_attack_packed.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("effects/light_dark_attack_packed.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("effects/water_wood_attack_packed.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("battleMisc.plist");
    for (int i=0;i<100;i++)
    {
        string plistName = "skill" + toStr(i) +".plist";
        if (PH::resourcePresentForLoading(plistName))
            CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile(plistName.c_str());
        else
            break;
    }
    
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}
    
BattleScene::~BattleScene()
{
    // should also unload background music here.
    CCLog("BattleScene destructor invoked\n");
}

// PvE
BattleScene* BattleScene::create(HeroList& team, LevelInfo& level)
{
    BattleScene* scene = new BattleScene();
    
    if(scene && scene->initWithLevel(team, level))
    {
        scene->autorelease();
        return scene;
    }
    CC_SAFE_DELETE(scene);
    return NULL;
}
    
bool BattleScene::initWithLevel(HeroList& team, LevelInfo& level)
{
    // preload assets
    loadBattleResources();
    
    // start background music
    playBG("sound/bg/battle.mp3");
    
    // create board layer
    if(GameConfig::shared()->getTutorialProgress() == TutorialFirstBattle)
    {
        BoardLayer* layer = TutorialBoardLayer::create(BOARD_WIDTH, BOARD_HEIGHT, team, level);
        this->addChild(layer);
    }
    else
    {
        BoardLayer* layer = BoardLayer::create(BOARD_WIDTH, BOARD_HEIGHT, team, level);
        this->addChild(layer);
    }

    return true;
}

// PvP
BattleScene* BattleScene::create(const Player& p1, const Player& p2, const PvPConfig& config)
{
    BattleScene* scene = new BattleScene();

    if(scene && scene->initPvP(p1, p2, config))
    {
        scene->autorelease();
        return scene;
    }
    CC_SAFE_DELETE(scene);
    return NULL;
}

bool BattleScene::initPvP(const Player& p1, const Player& p2, const PvPConfig& config)
{
    // preload assets
    loadBattleResources();

    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("pvp.plist");
    
    // start background music
    playBG("sound/bg/battle.mp3");
    
    PVPBoardLayer* pvp = PVPBoardLayer::create(BOARD_WIDTH,
                                               BOARD_HEIGHT,
                                               p1,
                                               p2,
                                               config);
    this->addChild(pvp);
    return true;
    
}

// PvP Recovery
BattleScene* BattleScene::create(int seed,
                                 int round,
                                 const Player& first,
                                 const Player& second,
                                 const rapidjson::Value& battleState,
                                 float skillTimeout,
                                 float moveTimeout,
                                 float skillTimeLeft,
                                 float moveTimeLeft,
                                 const rapidjson::Value& events)
{
    BattleScene* scene = new BattleScene();
    
    if(scene && scene->initPvP(seed,
                               round,
                               first,
                               second,
                               battleState,
                               skillTimeout,
                               moveTimeout,
                               skillTimeLeft,
                               moveTimeLeft,
                               events))
    {
        scene->autorelease();
        return scene;
    }
    CC_SAFE_DELETE(scene);
    return NULL;
}
    
bool BattleScene::initPvP(int seed,
                          int round,
                          const Player& first,
                          const Player& second,
                          const rapidjson::Value& battleState,
                          float skillTimeout,
                          float moveTimeout,
                          float skillTimeLeft,
                          float moveTimeLeft,
                          const rapidjson::Value& events)
{
    // preload assets
    loadBattleResources();
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("pvp.plist");
    
    // start background music
    playBG("sound/bg/boss");
    
    PVPBoardLayer* pvp = PVPBoardLayer::create(BOARD_WIDTH,
                                               BOARD_HEIGHT,
                                               seed,
                                               round,
                                               first,
                                               second,
                                               battleState,
                                               skillTimeout,
                                               moveTimeout,
                                               skillTimeLeft,
                                               moveTimeLeft,
                                               events);
    this->addChild(pvp);
    return true;
}

Hero* Player::findHero(const std::string& hid)
{
    std::vector<Hero*>::iterator it = std::find_if(storage.begin(),
                                                   storage.end(),
                                                   [&](Hero* h)
                                                   {
                                                       phassert(h != NULL && "find hero locates NULL in storage.");
                                                       return h && h->id == hid;
                                                   });
    return it == storage.end() ? NULL : *it;
}
    
}
