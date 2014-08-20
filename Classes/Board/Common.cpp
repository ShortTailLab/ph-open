//
//  Common.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 10/30/12.
//
//

#include "Common.h"
#include "extensions/utils.h"
#include "SimpleAudioEngine.h"
#include "PlayerData.h"

namespace PH
{
    USING_NS_CC;

    const int InitialHeroIds[6] = {64062, 64122, 64112, 64022, 64012, 64072};
    const int SuperHeroIds[5] = {66015, 66045, 66055, 66075, 66095};
    const int GodHeroIds[5] = {66025, 66035, 66065, 66085, 66105};
    const int SkyHeroIds[20] = {65212, 65232, 65252, 65272, 65292, 65213, 65233, 65253, 65273, 65293,
                               65214, 65234, 65254, 65274, 65294, 65215, 65235, 65255, 65275, 65295};
    bool isInitialHero(int hid)
    {
        for(int i=0; i<6; i++)
            if(InitialHeroIds[i] == hid)
                return true;
        return false;
    }
    bool isSuperHero(int hid)
    {
        for(int id : SuperHeroIds)
            if(id == hid)
                return true;
        return false;
    }
    bool isGodHero(int hid)
    {
        for(int id : GodHeroIds)
            if(id == hid)
                return true;
        return false;
    }
    
    bool isSkyHero(int hid)
    {
        for(int id : SkyHeroIds)
            if(id == hid)
                return true;
        return false;
    }
    
    void ring(const char* file, bool loop)
    {
        if(GameConfig::shared()->isSFXEnabled())
            CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(file);
    }
    
    void playBG(const char* file)
    {
        if(GameConfig::shared()->isBGMEnabled())
            CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(file, true);
    }
    
    void stopBgMusic()
    {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    }
    
    static const char* HintTable[] =
    {
        "每次移动宝石后算为一回合。",
        "每天玩游戏可获得铜钱奖励，记得要天天上线拿奖励噢。总计登录奖励可以累积，累积登录到一定数量后会得到额外惊喜！",
        "同色宝石，横排或者竖排三个或以上相连时可以消除，五个宝石会发动全体攻击噢，多尝试下吧！",
        "战场内我方侠客头像发光时，就可以发动技能了。好好利用侠客技能渡过难关吧！",
        "援军侠客的掌门技能也会被触发，还能获得更多的铜钱奖励。",
        "侠客级别提升慢后，可以使用转生道具升级到更高级的形态！",
        "战斗时，点击敌方侠客，可以优先攻击该侠客。",
        "无法通关时可能需要重新考虑队伍的配置和平衡噢！",
        "同时消除多组宝石，侠客的攻击力会大幅提升！",
        "强力侠客除了拥有主动技能，还拥有被动掌门技.",
        "元宝的作用很多，可以用来招募侠客、回復体力、扩充席位.",
        "宝石和侠客均有五种属性。\n火、水、木属性相互克制，\n光和暗相互克制。\n相克属性可以造成额外伤害喔！",
        "宝石的移动有时间限制，注意宝石上出现的倒计时。",
        "等级提升后，体力，威望，好友上限等相关属性也会增加。",
        "如何才能打出连击？宝石移动时要使不同属性的宝石连续消除。",
        "消除粉色宝石，能够回复一定数量的HP。",
        "同一颜色的宝石消除五个或以上，可以发动群体攻击。敌方多的时候非常有用！",
        "有些侠客的技能可以降低敌人的防御力，更快的消灭敌人！",
        "侠客要升级到最高形态？要记得经常强化和转生喔。",
        "《群侠转》会举行各种限时活动，敬请留意！",
        "强化侠客时，小心不要消耗掉稀有的转生道具哦噢。",
        "进入战场会消耗体力值，不同的战场消耗的体力值不同，每天还有限时的战场活动。",
        "使用同属性的侠客进行强化，能获得更多的经验值。",
        "只有在掌门或者援军位置上的侠客，才能发动掌门技能。",
        "玩家升级后，可以提升体力上限值",
    };
    
    std::string randHint()
    {
        int size = sizeof(HintTable) / sizeof(const char*);
        if(size == 0)
            return "還沒有提示噢";
        
        int i = rand() % size;
        
        return HintTable[i];
    }
    
    //--------------------
    // GameConfig
    //--------------------
    
#define CONFIG_SFX_KEY "SFX_enabled"
#define CONFIG_BGM_KEY "BGM_enabled"
#define CONFIG_ANDROID_HD_KEY  "Android_HD_enabled"
#define CONFIG_TUTORIAL_PROGRESS "tutorialProgress"
    
    GameConfig* GameConfig::shared()
    {
        static GameConfig* config = NULL;
        if(config == NULL)
        {
            config = new GameConfig();

            CCUserDefault* user = CCUserDefault::sharedUserDefault();
            config->tutorialMode = (TutorialProgress)user->getIntegerForKey(CONFIG_TUTORIAL_PROGRESS, TutorialSkip);
            config->mEnableSFX = user->getBoolForKey(CONFIG_SFX_KEY, true);
            config->mEnableBGM = user->getBoolForKey(CONFIG_BGM_KEY, true);
            config->mEnableAndroidHD = user->getBoolForKey(CONFIG_ANDROID_HD_KEY, false);
            
            config->stageTab = -1;
            config->stageId = -1;
            
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            config->useTouchSmooth = true;
#else
            config->useTouchSmooth = false;
#endif
        }
        return config;
    }
    
    void GameConfig::load()
    {
        std::string playerUID = PlayerData::getInstance()->player->uid;
        
        CCUserDefault* user = CCUserDefault::sharedUserDefault();

        showInstanceStage = user->getBoolForKey((playerUID + "_instance_stage").c_str(),
                                                false);
        showSoulExchange = user->getBoolForKey((playerUID + "_soul_exchange").c_str(),
                                               false);
        showShuffleRelation = user->getBoolForKey((playerUID + "_shuffle_relation").c_str(), false);
        firstTimeNeili = user->getBoolForKey((playerUID + "_first_time_neili").c_str(), true);
        firstTimeShowInstance = user->getBoolForKey((playerUID + "_first_time_instance").c_str(), true);
        firstTimeZhuansheng = user->getBoolForKey((playerUID + "_first_time_zhuansheng").c_str(), true);
        
        openInstanceStep = user->getIntegerForKey((playerUID + "_open_instance").c_str(), 0);
        openJueDing10 = user->getBoolForKey((playerUID + "_open_jueding10").c_str(), false);
        
        showDragHint = user->getBoolForKey((playerUID + "_show_drag_hint").c_str(), true);
        showAttrHint = user->getBoolForKey((playerUID + "_show_attr_hint").c_str(), true);
    }
    
    void GameConfig::save()
    {
        std::string playerUID = PlayerData::getInstance()->player->uid;

        CCUserDefault* user = CCUserDefault::sharedUserDefault();
    
        user->setBoolForKey((playerUID + "_instance_stage").c_str(),
                            showInstanceStage);
        user->setBoolForKey((playerUID + "_soul_exchange").c_str(),
                            showSoulExchange);
        user->setBoolForKey((playerUID + "_shuffle_relation").c_str(),
                            showShuffleRelation);
        user->setBoolForKey((playerUID + "_first_time_neili").c_str(),
                            firstTimeNeili);
        user->setBoolForKey((playerUID + "_first_time_instance").c_str(),
                            firstTimeShowInstance);
        user->setBoolForKey((playerUID + "_first_time_zhuansheng").c_str(),
                            firstTimeZhuansheng);
        
        user->setIntegerForKey((playerUID + "_open_instance").c_str(), openInstanceStep);
        user->setBoolForKey((playerUID + "_open_jueding10").c_str(), openJueDing10);
        
        user->setBoolForKey((playerUID + "_show_drag_hint").c_str(), showDragHint);
        user->setBoolForKey((playerUID + "_show_attr_hint").c_str(), showAttrHint);
        
        user->flush();
    }
    
    void GameConfig::setSFXEnabled(bool val)
    {
        mEnableSFX = val;
        CCUserDefault* user = CCUserDefault::sharedUserDefault();
        user->setBoolForKey(CONFIG_SFX_KEY, mEnableSFX);
        user->flush();
    }
    
    void GameConfig::setBGMEnabled(bool val)
    {
        mEnableBGM = val;
        CCUserDefault* user = CCUserDefault::sharedUserDefault();
        user->setBoolForKey(CONFIG_BGM_KEY, mEnableBGM);
        user->flush();
    }
    
    void GameConfig::setAndroidHDEnabled(bool val)
    {
        mEnableAndroidHD = val;
        CCUserDefault* user = CCUserDefault::sharedUserDefault();
        user->setBoolForKey(CONFIG_ANDROID_HD_KEY, mEnableAndroidHD);
        user->flush();
    }
    
    void GameConfig::setTutorialProgress(PH::TutorialProgress progress)
    {
        tutorialMode = progress;
        CCUserDefault* user = CCUserDefault::sharedUserDefault();
        user->setIntegerForKey(CONFIG_TUTORIAL_PROGRESS, tutorialMode);
        user->flush();
    }
}