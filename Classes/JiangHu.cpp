#include "JiangHu.h"

#include "DebugMisc.h"
#include "GameScene.h"
#include "PlayerData.h"
#include "Fonts.h"
#include "ZqlScrollView.h"
#include "PageScrollView.h"
#include "ControlExtensions.h"
#include "Board/HeroTable.h"
#include "network/PuzzleRPC.h"
#include <extensions/utils.h>
#include "Board/Utils.h"
#include <time.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "HeroProfileLayer.h"
#include "Store.h"
#include "ObjectPool.h"
#include "UIPresets.h"
#include "CCCallLambda.h"

#include "PvPUtils.h"
#include "HeroIcon.h"
#include "CCNative.h"
#include "Common.h"


namespace PH
{
    USING_NS_CC;
    using namespace std;
    using namespace boost::posix_time;

    bool RelationsEffect::init(ScrollRelations* view, JuanZhou* jz)
    {
        if(!ContentLayer::init())
            return false;
        
        this->view = view;
        this->jz = jz;
        heroes = jz->heroVec;
        icons = jz->heroImgVec;
        items = view->relationItems;
        
        return true;
    }
    
    void RelationsEffect::showEffect()
    {
        CCArray* actions = CCArray::create();
        actions->addObject(CCCallLambda::create([=](){
            view->setTouchEnabled(false);
            jz->setTouchEnabled(false);
        }));
        for(int j = 0; j < items.size(); j++)
            actions->addObject(CCSequence::create(CCCallLambda::create([=]()
                                                                       {
                                                                           ((HeroRelationItem*)items[j])->onPress(true);
                                                                           if(j >= 2)
                                                                               view->scrollView->setMoveTargetId(j-1);
                                                                       }),
                                                  createEachItemEffect(items[j]),
                                                  CCCallLambda::create([=]()
                                                                       {
                                                                           ((HeroRelationItem*)items[j])->onPress(false);
                                                                       }),
                                                  NULL));
        actions->addObject(CCCallLambda::create([=](){
            view->setTouchEnabled(true);
            jz->setTouchEnabled(true);
        }));
        jz->runAction(CCSequence::create(actions));
    }
    
    bool RelationsEffect::isOwnRelation(const Hero* hero, const HeroRelation& relation)
    {
        for(const HeroRelation& h : hero->relations)
            if(h == relation)
                return true;
        return false;
    }
    
    CCAction* RelationsEffect::createEachItemEffect(CCSprite* item)
    {
        const HeroRelation& relation = ((HeroRelationItem*)item)->relation;
        CCArray* actions = CCArray::create();
        
        actions->addObject(CCTargetedAction::create(item,
                                                   CCSequence::create(CCScaleTo::create(0.2f, 1.1f),
                                                                      CCScaleTo::create(0.2f, 1.0f),
                                                                      CCScaleTo::create(0.2f, 1.1f),
                                                                      CCScaleTo::create(0.2f, 1.0f),
                                                                      NULL)));
        
        for(int i = 0; i<heroes.size(); i++)
            if(heroes[i] && isOwnRelation(heroes[i], relation))
                actions->addObject(createEachIconEffect(i, relation));
        
        return CCSpawn::create(actions);
    }
    
    CCAction* RelationsEffect::createEachIconEffect(int iconNum, const HeroRelation& relation)
    {
        HeroIcon* icon = icons[iconNum];
        const Hero* hero = icon->getHero();
        CCArray* effects = CCArray::create();
        
        CCSprite* mask = GetSprite("gameui/duiwu_2.png");
        mask->setPosition(icon->getPosition());
        mask->setVisible(false);
        addChild(mask);
        
        CCFiniteTimeAction* maskAction = CCSequence::create(CCShow::create(),
                                                  CCFadeOut::create(1),
                                                  CCCallLambda::create([=](){this->removeChild(mask,true);}),
                                                  NULL
                                                  );
        
        effects->addObject(CCTargetedAction::create(mask, maskAction));
        
        for(const boost::tuple<RelationEffects, double>& rEffect : relation.effects)
        {
            string name;
            string fontName;
            int org;
            if(boost::get<0>(rEffect) == RelationEffects::HP)
            {
                name = "HP+";
                org = hero->health;
                fontName = "bmfont/PW_Green_18.fnt";
            }
            else if(boost::get<0>(rEffect) == RelationEffects::STRENGTH)
            {
                name = "攻+";
                org = hero->attack;
                fontName = "bmfont/PW_Orange_18.fnt";   
            }
            else
            {
                name = "恢+";
                org = hero->regen;
                fontName = "bmfont/PW_Purple_18.fnt";
            }
            int value = org*boost::get<1>(rEffect);
            int effectIndex = getEffectIndex(iconNum, boost::get<0>(rEffect));
            if(effectsOnIcons.count(effectIndex))
            {
                CCLabelBMFont* numLabel = effectsOnIcons[effectIndex];
                int from = toInt(numLabel->getString());
                numLabel->setString(toStr(from+value).c_str());
                effects->addObject(CCTargetedAction::create(numLabel, CCCountBMTextNumber::create(from, from+value, 0.5f)));
                
            }
            else
            {
                effects->addObject(createNumAction(name, fontName, 0, value, getEffectPosition(icon), effectIndex));
            }
            
        }
        return CCSpawn::create(effects);
    }
    
    int RelationsEffect::getEffectIndex(int iconNum, const RelationEffects& effect)
    {
        return iconNum*3+effect;
    }
    
    CCPoint RelationsEffect::getEffectPosition(HeroIcon* icon)
    {
        if(effectsNumOnEachIcon.count(icon) == 0)
            effectsNumOnEachIcon[icon] = 0;
        return ccp(icon->getPosition().x-45,
                   icon->getPosition().y-35+20*(effectsNumOnEachIcon[icon]++));
    }
    
    CCAction* RelationsEffect::createNumAction(const string& name,
                                               const string& fontName,
                                               int from, int to,
                                               const CCPoint& pos,
                                               int index)
    {
        CCLabelBMFont* nameLabel;
        CCLabelBMFont* valueLabel;
        
        addLabelBMFont(this, nameLabel, name.c_str(), fontName.c_str(), ccp(pos.x, pos.y), ccp(0,0.5));
        addLabelBMFont(this, valueLabel, toStr(to).c_str(), fontName.c_str(),
                       ccp(nameLabel->getPosition().x + nameLabel->getContentSize().width*contentScale() - 5,
                           nameLabel->getPosition().y),
                       ccp(0,0.5));
        
        nameLabel->setVisible(false);
        nameLabel->setScale(contentScale());
        valueLabel->setVisible(false);
        valueLabel->setScale(contentScale());
        
        effectsOnIcons[index] = valueLabel;
        
        return CCSequence::create(CCCallLambda::create([=](){
            nameLabel->setVisible(true);
            valueLabel->setVisible(true);
        }),
                                  CCTargetedAction::create(valueLabel, CCCountBMTextNumber::create(from, to, 0.5f)),
                                  NULL);
    }
    
    
bool TeamConfirmJuanZhou::init(const HeroList& team, Level& level)
{
    if(!JuanZhou::init())
        return false;
    
    assert(team.size() == 6 && "make sure that it contains helper");
    
    viewPort = CCSize(20 , 485);
    confirmTeam = team;
    relationsView = NULL;
    effectLayer = NULL;
    
    CCSprite *bg = GetSprite("gameui/duiwu_1_3.png");
    bg->setAnchorPoint(ccp(0.5 , 1));
    this->addChild(bg);
    
    switcher = BianDuiSwitcher::create();
    switcher->setPosition(ccp(-275, -42));
    switcher->setCallBack([=](int i)
    {
        if(PlayerData::getInstance()->getActiveTeam() != i)
        {
            stopAllActions();
            PlayerData::getInstance()->setActiveTeam(i);
            this->refresh();
            this->initHeroRelations();
        }
    });
    addChild(switcher);
    heroNum = 6;
    heroVec = std::vector<Hero*>(heroNum, (Hero*)NULL);
    heroImgVec = std::vector<HeroIcon*>(heroNum, (HeroIcon*)NULL);
    
    for(int i = 0 ; i < heroNum ; i++)
    {
        if(i == 0)
            locations.push_back(CCRect(-282, -275 , 100 , 100 ));
        else if(i < heroNum-1)
            locations.push_back(CCRect(-279+91*i , -278 , 100 , 100 ));
        else
            locations.push_back(CCRect(183, -280 , 100 , 100 ));
    }
    
    for(int i = 0; i < confirmTeam.size(); i++)
        if(confirmTeam[i])
            addHero(confirmTeam[i], i);
    
    teamData.update(confirmTeam);
    addLabelBMFont(this, hpLabel, toStr(teamData.hp).c_str(),
                   "bmfont/Sumo_22_yellow.fnt", ccp(-163, -83), ccp(0.5, 0.5));
    hpLabel->setScale(contentScale());
    addLabelBMFont(this, attackLabel, toStr(teamData.attack[GemUtils::AllColor]).c_str(),
                   "bmfont/Sumo_22_yellow.fnt", ccp(31, -83), ccp(0.5, 0.5));
    attackLabel->setScale(contentScale());
    addLabelBMFont(this, regenLabel, toStr(teamData.regen).c_str(),
                   "bmfont/Sumo_22_yellow.fnt", ccp(228, -83), ccp(0.5, 0.5));
    regenLabel->setScale(contentScale());

    addLabelTTF(this, skillLabel, "", FONT_CN, 28, ccp(-120,-127), ccp(0, 0.5), ccc3(108, 46, 0));
    
    skillDesLabel = LabelX::create("", FONT_CN, 28, LabelX::FontEffect::SHADOW, ccp(2, -2));
    skillDesLabel->setPosition(ccp(-265,-162));
    skillDesLabel->setAnchorPoint(ccp(0, 0.5));
    this->addChild(skillDesLabel);
    
    addLabelTTF(this, helperSkillLabel,AST(team[5]->profile->passiveSkillId).name.c_str(),
                FONT_CN, 28, ccp(-120,-305), ccp(0, 0.5), ccc3(108, 46, 0));
    
    helperSkillDesLabel = LabelX::create(AST(team[5]->profile->passiveSkillId).desc.c_str(),
                                         FONT_CN, 28, LabelX::FontEffect::SHADOW, ccp(2, -2));
    helperSkillDesLabel->setPosition(ccp(-265,-341));
    helperSkillDesLabel->setAnchorPoint(ccp(0, 0.5));
    this->addChild(helperSkillDesLabel);
    
    refresh();
    
    setTouchEnabled(true);
    
    if(GameConfig::shared()->getTutorialProgress() == TutorialFirstBattle)
    {
        setTouchEnabled(false);
        switcher->setTouchEnabled(false);
    }
        
    return true;
}
    
bool TeamConfirmJuanZhou::addHero(PH::Hero *hero , int id ,bool isUpdate)
{
    JuanZhou::addHero(hero, id, isUpdate);
    heroImgVec[id]->setScale(0.9);
    return true;
}
    
void TeamConfirmJuanZhou::openDone()
{
    initHeroRelations();
}
    
void TeamConfirmJuanZhou::initHeroRelations()
{
    if(relationsView)
    {
        relationsView->removeFromParentAndCleanup(true);
        relationsView = NULL;
    }
    if(effectLayer)
    {
        effectLayer->removeFromParentAndCleanup(true);
        effectLayer = NULL;
    }   
    
    teamRelations = calcRelationsInTeam(confirmTeam);
    if(!teamRelations.empty())
    {
        relationsView = ScrollRelations::create(teamRelations, CCRectMake(-83, 0, 438, 68));
        relationsView->setPosition(ccp(0, -439));
        addChild(relationsView,101);
        
        effectLayer = RelationsEffect::create(relationsView, this);
        effectLayer->showEffect();
        addChild(effectLayer, 100);
    }
}
    
void TeamConfirmJuanZhou::refresh()
{
    HeroList myTeam = PlayerData::getInstance()->getTeam();
    for(int i = 0; i < myTeam.size(); i++)
        confirmTeam[i] = myTeam[i];
    teamData.update(confirmTeam);
    
    clear();
    for(int i = 0; i < confirmTeam.size(); i++)
        if(confirmTeam[i])
            addHero(confirmTeam[i], i);
    
    hpLabel->setString(toStr(teamData.hp).c_str());
    attackLabel->setString(toStr(teamData.attack[GemUtils::AllColor]).c_str());
    regenLabel->setString(toStr(teamData.regen).c_str());
    
    if(confirmTeam[0])
    {
        skillLabel->setString(AST(confirmTeam[0]->profile->passiveSkillId).name.c_str());
        skillDesLabel->setString(AST(confirmTeam[0]->profile->passiveSkillId).desc.c_str());
    }
    else
    {
        skillLabel->setString("无");
        skillDesLabel->setString("无");
    }
}
    
void TeamConfirmJuanZhou::onLongClick(CCTouch* touch)
{
    //check if touch on the heroIcon
    for (HeroIcon *icon : heroImgVec)
    {
        if(!icon)
            continue;
        CCPoint localPoint = icon->getParent()->convertTouchToNodeSpace(touch);
        if(icon->boundingBox().containsPoint(localPoint))
        {
            auto* layer = HeroProfileLayer::create(icon->getHero(), false);
            GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
            break;
        }
    }
}
    
//////////////////////////////////////////
//TeamConfirmLayer
//////////////////////////////////////////
bool TeamConfirmLayer::init(PlayerPtr player , Level& level)
{
    if (!ContentLayer::init())
        return false;
    
    targetPlayer = player;
    targetLevel = level;
    
    menu = CCMenu::create();
    CCMenuItem *item_back = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                                     GetSprite("gameui/back_t.png"),
                                                     this,
                                                     menu_selector(TeamConfirmLayer::onBack));
    
    CCMenuItem *item_title = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                      GetSprite("gameui/hongchengdi_t_tongyong.png"));
    
    LabelX* name = LabelX::create("确认队伍", FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp( item_title->getContentSize().width*0.5f , item_title->getContentSize().height*0.5f));
    item_title->addChild(name);
    
    item_back->setAnchorPoint(ccp(0.5 , 1));
    item_title->setAnchorPoint(ccp(0.5 , 1));
    menu->addChild(item_back);
    menu->addChild(item_title);
    
    CCPoint pos(-70, 18);
    if(isSharedAvailable())
    {
        pos = ccp(0, 18);
        CCMenuItem *itemRight = CCMenuItemSprite::create(createBtn("分享", ButtonType::NORMAL_BLUE, TITLE_YELLOW),
                                                         createBtn("分享", ButtonType::NORMAL_BLUE, TITLE_YELLOW),
                                                         this,
                                                         menu_selector(TeamConfirmLayer::onShare));
        itemRight->setAnchorPoint(ccp(0.5 , 1));
        menu->addChild(itemRight);
    }
    
    menu->alignItemsHorizontallyWithPadding(8);
    menu->setPosition(pos);
    addChild(menu , 10);
    
    CCMenu* startMenu = CCMenu::create();
    CCMenuItem* btn = CCMenuItemSprite::create(GetSprite("gameui/4zidi.png"),
                                               GetSprite("gameui/4zidi.png"),
                                               this,
                                               menu_selector(TeamConfirmLayer::onStartGame));
    LabelX* btn_name = LabelX::create("立即开战", FONT_CN, 34);
    btn_name->setColor(ccWHITE);
    btn_name->setAnchorPoint(ccp(0.5, 0.5));
    btn_name->setPosition(ccp(btn->getContentSize().width*0.5f , btn->getContentSize().height*0.5f));

    btn->addChild(btn_name);
    
    startMenu->addChild(btn);
    startMenu->setPosition(ccp(0, -620));
    addChild(startMenu);
    
    {
        CCSprite* titleBg = GetSprite("gameui/zhanchangbiaoti.png");
        titleBg->setPositionY(-80);
        addChild(titleBg);
        
        CCSize bgSize = titleBg->getContentSize();

        
        PlayerData* playerData = PlayerData::getInstance();
        std::vector<StagePtr> stageList = playerData->normalStageList;
        // merge stage lists
        stageList.insert(stageList.end(),
                         playerData->instanceStageList.begin(),
                         playerData->instanceStageList.end());
        
        std::string stageName = "";
        std::string levelName = "";
        
        for(auto& stage : stageList)
        {
            const Level* level = stage->findLevelById(targetLevel.id);
            if(level != NULL)
            {
                stageName = stage->name;
                levelName = level->name;
                break;
            }
        }

        CCLabelTTF* titleLabel;
        std::string title = stageName + "  " + "之" + "  " + levelName;
        addLabelTTF(titleBg, titleLabel,
                    title.c_str(), FONT_CN, 36,
                    ccp(bgSize.width*0.5f, bgSize.height*0.5f), ccp(0.5, 0.5));
        titleLabel->enableShadow(ccp(3, -3), 1.0f, 0.0f);
    }
    
    HeroList team = PlayerData::getInstance()->getTeam();
    team.push_back(player->team[0]);
    
    TeamConfirmJuanZhou* juanzhou = TeamConfirmJuanZhou::create(team, level);
    
    CCSprite *zhou1 = GetSprite("gameui/duiwu_1_1.png");
    CCSprite *zhou2 = GetSprite("gameui/duiwu_1_1.png");
    zhou2->setFlipX(true);
    
    jz = JuanZhouLayer::create();
    jz->addSkin(juanzhou , zhou1 , zhou2);
    jz->setPosition(ccp(0 , -95));
    addChild(jz);
    
    return true;
}
    
void TeamConfirmLayer::onShare()
{
     SystemInform::alertSharePanel(runningScene(),
                                   toStr("嘿嘿，我要上场了，@群侠转 最强力队伍，什么老毒物、东方大神都不是我的对手！来战！") +
                                   "https://itunes.apple.com/cn/app/qun-xia-zhuan/id669428248");
}
    
void TeamConfirmLayer::onStartGame()
{
    if(PlayerData::getInstance()->getTeam()[0] == NULL)
    {
        SystemInform::alertMessage(GameScene::getInstance(), "掌门不能为空！", "提示", [](){});
        return;
    }
    
    if(PlayerData::getInstance()->teamData.lp > PlayerData::getInstance()->player->maxLP)
    {
        SystemInform::alertConfirmPanel(GameScene::getInstance(),
                                        "当前队伍的总威望超过上限无法进入战斗，请检查编队，升级VIP可增加威望上限！", "提示",
                                        [](){}, [=](){
                                            GameScene::getInstance()->pushLayerWithDefaultAnimation(VIPLayer::create());
                                        }, false, "确定", "VIP");
        return;
    }
    
    if(GameConfig::shared()->getTutorialProgress() == TutorialFirstBattle)
    {
        ArrowMask::shared()->clear();
    }
    
    SystemInform *inform = SystemInform::alertLoading(GameScene::getInstance());
     
     PuzzleRPC::getInstance()->startGame(PlayerData::getInstance()->getToken(),
                                         targetLevel.id,
                                         targetPlayer->uid,
                                         targetPlayer->team[0]->id,
                                         [=](std::auto_ptr<LevelInfo> match,
                                             std::auto_ptr<Error> e)
     {
         inform->cancel();
         
         if(e.get())
         {
             std::cout << "start game, " << e->toStr() << std::endl;
             if(e->code == ErrorCode::GameNonExist)
                 SystemInform::alertMessage(GameScene::getInstance(),
                                            "该关卡时间已过",
                                            "警告",
                                            [=](){GameScene::getInstance()->selectTab(1);});
             else
                 e->handleErrorInform();
             return;
         }
         
         if(GameConfig::shared()->getTutorialProgress() == TutorialFirstBattle)
             ArrowMask::shared()->clear();
         
         auto player = PlayerData::getInstance()->player;
         PlayerData::getInstance()->setSpirit(match->spirit, player->maxSpirit);
         PlayerData::getInstance()->setCoins(player->coins + targetPlayer->asHelperReward);
         
         // keep track of the ally player so we could add him as a friend later
         PlayerData::getInstance()->lastAllyPlayer = targetPlayer;
         
         LevelInfo levelInfo = *match;
         
         HeroList team = PlayerData::getInstance()->getTeam();
         team.push_back(targetPlayer->team[0]);
         
         CCAssert(team.size() == 6, "team size mismatch, causing team[5] to be nonsupport hero");
         
         GameScene* s = GameScene::getInstance();

         s->stopReloginSchedule();
         s->unscheduleTabMsg();
         s->clearUI();

         CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("gameui.plist");
         CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("gameui1.plist");
         CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("gameui2.plist");
         CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("gameui3.plist");
         CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("gameuiBase.plist");
         CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("gameuiQiuAnim.plist");
         CCTextureCache::sharedTextureCache()->removeUnusedTextures();

         // dispatch twice to clean remaining resources
         SystemInform* alert = SystemInform::alertLoading(GameScene::getInstance());

         runOnMainThread([=]()
         {
             runOnMainThread([=]()
             {
                 alert->cancel();
                 
                 GameScene::getInstance()->clearBG();
             
                 HeroList teamCopy = team;
                 LevelInfo levelCopy = levelInfo;

                 CCTextureCache::sharedTextureCache()->removeUnusedTextures();

                 BattleScene* battleScene = BattleScene::create(teamCopy, levelCopy);
                 CCDirector::sharedDirector()->replaceScene(battleScene);
             });
         });
     });
}

void TeamConfirmLayer::onEnter()
{
    ContentLayer::onEnter();
    
    if(GameConfig::shared()->getTutorialProgress() == TutorialFirstBattle)
    {
        
                                 ArrowMask::shared()->setMask(CCRect(205, 148, 225, 66));
                                 ArrowMask::shared()->setArrow(ccp(176, 178));
    }
}

void TeamConfirmLayer::onEnterAnimationDone()
{
    jz->startAnim();
}

void TeamConfirmLayer::onBack(cocos2d::CCObject *target)
{
    GameScene::getInstance()->popLayer();
}


bool HelpItem::init(PlayerPtr& player , const Level& level, const HelpersSortType& type)
{
	if(!ContentLayer::init())
        return false;
    
    ContentLayer::setTouchEnabled(true);
	targetPlayer = player;
	targetLevel = level;
    heroImg = NULL;
    sigh = NULL;
    
	const char* path = player->isFriend ? "gameui/youjun.png" : "gameui/yiyun.png";
	bg = GetSprite(path);
	bg->setAnchorPoint(ccp(0.5 , 1));
	bg->setPosition(ccp(0 , 0));
	addChild(bg);

	targetHero = targetPlayer->team[0];
	if(targetHero) 
    {
		heroImg = HeroIcon::create(targetHero, false , true);
		heroImg->setAnchorPoint(ccp(0,1));
		heroImg->setPosition(ccp(-271, -9));
		addChild(heroImg);
        if(type == HelpersSortType::HP_Sort)
            ((HeroIcon*)heroImg)->setBottomMsg(HeroIcon::LabelType::HP);
        else if(type == HelpersSortType::Attack_Sort)
            ((HeroIcon*)heroImg)->setBottomMsg(HeroIcon::LabelType::Attack);
        else if(type == HelpersSortType::Regen_Sort)
            ((HeroIcon*)heroImg)->setBottomMsg(HeroIcon::LabelType::Regen);
        else if(type == HelpersSortType::Title_Sort)
            ((HeroIcon*)heroImg)->setBottomMsg(HeroIcon::LabelType::Type);
        
        initRelation();
	}
    
    string value = trimNameCN(player->name);
    
    CCLabelTTF *nameLabel = LabelX::create(value.c_str() , FONT_CN , 28, LabelX::FontEffect::SHADOW, ccp(2, -2));
    nameLabel->setColor(ccWHITE);
    nameLabel->setPosition(ccp(-160, -34));
    nameLabel->setAnchorPoint(ccp(0, 0.5));
    addChild(nameLabel);
    
    CCLabelTTF *levelLabel = LabelX::create(toStr(player->level).c_str() , FONT_CN , 22, LabelX::FontEffect::SHADOW, ccp(2, -2));
    levelLabel->setColor(ccWHITE);
    levelLabel->setPosition(ccp(230, -33));
    levelLabel->setAnchorPoint(ccp(0.5, 0.5));
    addChild(levelLabel);

    CCLabelBMFont* coinsLabel;
    addLabelBMFont(this, coinsLabel, toStr(player->asHelperReward).c_str(), "bmfont/Sumo_22_yellow.fnt",ccp(58, -83), ccp(0.5,0.5));
    coinsLabel->setScale(contentScale());

	btn = GetSprite("gameui/chengdi_t_tongyong_x.png");
	btn->setPosition(ccp(214 , -79));
    
    CCLabelTTF *btn_label = LabelX::create("选择" , FONT_CN , 26, LabelX::FontEffect::SHADOW, ccp(3, -3));
    btn_label->setColor(ccWHITE);
    btn_label->setPosition(ccp(btn->getContentSize().width*0.5f, btn->getContentSize().height*0.5f));
    btn->addChild(btn_label);
    
	addChild(btn);

	setContentSize(CCSize(556 , 140));
	return true;
}
    
void HelpItem::initRelation()
{
    targetHero = targetPlayer->team[0];
    vector<Hero*> team = PlayerData::getInstance()->getTeam();
    team.push_back(targetHero);
    std::vector<HeroRelation> relations = calcRelationsInTeam(team);
    
    for(const HeroRelation& relation : relations)
        for(int id : relation.withHeroes)
            for(int id2 = id; id2 != -1; id2 = HPT(id2).transformTargetHeroId)
                if(id2 == targetHero->heroId)
                {
                    if(!sigh)
                    {
                        sigh = GetSprite("gameui/yuan_yuanjun.png");
                        sigh->setPosition(ccp(-273, -12));
                        sigh->setAnchorPoint(ccp(1,1));
                        addChild(sigh);
                    }
                    return;
                }

    if(sigh)
    {
        sigh->removeFromParentAndCleanup(true);
        sigh = NULL;
    }
}

void HelpItem::setTouchEnabled(bool value)
{
	ContentLayer::setTouchEnabled(value);
	//menu->setTouchEnabled(value);
}
    
void HelpItem::onSingleClick(cocos2d::CCTouch *touch)
{
    CCPoint point = convertTouchToNodeSpace(touch);
    CCSize size = getContentSize();
    CCSize btnSize = btn->getContentSize();
    
    CCRect rect = CCRectMake(-size.width*0.5f, -size.height, size.width, size.height);
    CCRect btnRect = btn->boundingBox();
    btnRect.size.width += 4;
    btnRect.size.height += 4;
    
    if (heroImg && heroImg->boundingBox().containsPoint(point))
    {
        if(GameConfig::shared()->getTutorialProgress() == TutorialFirstBattle)
        {
            // do not allow hero detail in tutorial mode.
        }
        else
        {
            auto layer = HeroProfileLayer::create(targetHero , false , true);
            layer->setSelectCall([=](){GameScene::getInstance()->popLayer();onConfirm(NULL);});
            GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
        }
    }
    //hack ,to fix the bugs that click the heroicon but enter battle scene with a wrong hero
    else if(rect.containsPoint(point) && ((CCNode*)getUserData())->getParent())
    {
        onConfirm(NULL);
    }
}
    
void HelpItem::onLongClick(CCTouch* touch)
{
    CCPoint localPoint = convertTouchToNodeSpace(touch);
    
    if (heroImg && heroImg->boundingBox().containsPoint(localPoint))
    {
        if(GameConfig::shared()->getTutorialProgress() == TutorialFirstBattle)
        {
            // do not allow hero detail in tutorial mode.
        }
        else
        {
            auto layer = HeroProfileLayer::create(targetHero , false , true);
            layer->setSelectCall([=](){GameScene::getInstance()->popLayer();onConfirm(NULL);});
            GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
        }
    }
}

void HelpItem::onConfirm(CCObject *target)
{
    if(GameConfig::shared()->getTutorialProgress() == TutorialFirstBattle)
        ArrowMask::shared()->clear();
    
    GameScene::getInstance()->pushLayerWithDefaultAnimation(TeamConfirmLayer::create(targetPlayer,targetLevel));
    
    ring("sound/v/button_click.mp3");
    
}


bool HelpersLayer::init(const Level& level)
{
	if(!ContentLayer::init())
        return false;

    LOGD("Helpers layer init\n");
    CCNative::reportMemoryUsage();
    
	scrollView = NULL;
	targetLevel = level;

	menu = CCMenu::create();

	CCMenuItem *item_back = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                                    GetSprite("gameui/back_t.png"),
                                                    this,
                                                    menu_selector(HelpersLayer::onBack));
    
	CCMenuItem *item_title = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                     GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                     this,
                                                     menu_selector(HelpersLayer::onChangeSortType));

    LabelX* name = LabelX::create("援军", FONT_CN, 36, LabelX::FontEffect::SHADOW, ccp(3, -3));
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp(90 , 33));
    item_title->addChild(name);
    
    title = LabelX::create("默认排序" , FONT_CN , 20, LabelX::FontEffect::SHADOW, ccp(2, -2));
    title->setColor(ccWHITE);
    title->setPosition(ccp( 170 , item_title->getContentSize().height*0.5f));
    title->setAnchorPoint(ccp(0, 0.5));
    item_title->addChild(title);
    
    CCSprite* xian = GetSprite("gameui/hongchengdi_huang_tongyong.png");
    xian->setPosition(ccp(160 , item_title->getContentSize().height*0.5f));
    item_title->addChild(xian);
    
    item_back->setAnchorPoint(ccp(0.5 , 1));
    item_title->setAnchorPoint(ccp(0.5 , 1));
	menu->addChild(item_back);
	menu->addChild(item_title);
	
    menu->alignItemsHorizontallyWithPadding(8);
	menu->setPosition(ccp(-70 , 18));
	addChild(menu , 10);
    
	SystemInform *inform = SystemInform::alertLoading(GameScene::getInstance());

    PuzzleRPC::getInstance()->helpers(PlayerData::getInstance()->getToken(),
                                      targetLevel.id,
                                      [=](std::vector<Player*> players,
                                          std::auto_ptr<Error> e)
    {
        inform->cancel();
        
		if(e.get())
        {
			LOGD("!!! get friends error: %s\n", e->toStr().c_str());
			e->handleErrorInform();
            if(GameConfig::shared()->getTutorialProgress() == TutorialFirstBattle)
            {
                ArrowMask::shared()->setMask(CCRect(0, 140, 640, 685));
            }
			return;
		}

        for(Player* p : players)
            helpers.push_back(PlayerPtr(p));

		makeItems(getHelpersBySortType());
        title->setString(getSortStr().c_str());
        
        if(GameConfig::shared()->getTutorialProgress() == TutorialFirstBattle)
        {
            scrollView->setScrollEnabled(false);
            
             ArrowMask::shared()->setMask(CCRect(33, 620, 570, 123));
             ArrowMask::shared()->setArrow(ccp(320, 610), ArrowDirection::Up);
            
        }
    });
    
    if(GameConfig::shared()->getTutorialProgress() == TutorialFirstBattle)
    {
        item_back->setEnabled(false);
        item_title->setEnabled(false);
        ArrowMask::shared()->setMask(CCRect(0, 140, 640, 685));
    }
    
	return true;
}
    
void HelpersLayer::checkIfStageEnd()
{
    if(targetStage.visibleEnd != not_a_date_time)
    {
        auto dur = targetStage.visibleEnd - second_clock::universal_time();
        if( !dur.is_negative() )
        {
            schedule(schedule_selector(HelpersLayer::onStageEnd) , 1);
        }
    }
}
    
void HelpersLayer::onStageEnd(float dt)
{
    auto dur = targetStage.visibleEnd - second_clock::universal_time();
    if(dur.is_negative())
    {
        unschedule(schedule_selector(HelpersLayer::onStageEnd));
        onBack(NULL);
    }
}

void HelpersLayer::setTouchEnabled(bool value)
{
	ContentLayer::setTouchEnabled(value);
	menu->setTouchEnabled(value);
	for(ContentLayer *item : items)
		item->setTouchEnabled(value);
}
    
void HelpersLayer::onEnter()
{
    ContentLayer::onEnter();
    for(HelpItem* item : items)
        item->initRelation();
}


void HelpersLayer::onSingleClick(CCTouch * touch)
{
#if PH_DEBUG_BUILD
    if(!scrollView) return;
    CCPoint localPoint = scrollView->convertTouchToNodeSpace(touch);
    Hero* hero = NULL;
    if(scrollView->boundingBox().containsPoint(localPoint))
        hero = board->getHero(touch);

    if(hero){
        PlayerPtr targetPlayer = this->helpers[0];

        delete targetPlayer->team[0];
        targetPlayer->team[0] = targetPlayer->storage[0] = new Hero(hero->id, hero->heroId, hero->level, hero->exp);

        GameScene::getInstance()->pushLayerWithDefaultAnimation(TeamConfirmLayer::create(targetPlayer,targetLevel));

        ring("sound/v/button_click.mp3");
    }
#endif
}


void HelpersLayer::makeItems(const std::vector<PlayerPtr>& friends)
{
	if(scrollView)
    {
		this->removeChild(scrollView, true);
		items.clear();
	}

	scrollView = ZqlScrollView::create();

	for(PlayerPtr player : friends)
    {
        HelpItem *item = HelpItem::create(player , targetLevel, PlayerData::getInstance()->getHelpersSortType());
        item->setUserData(this);
		items.push_back(item);
		scrollView->addLayer(item);
	}
    
#if PH_DEBUG_BUILD
    board = HeroBoard::create(PlayerData::getInstance()->getStorage());
    scrollView->addLayer(board);
#endif
    
	scrollView->setViewSize(CCSize(winSize.width , 800) ,  CCRect(0, -100, winSize.width, 600));
	scrollView->setPosition(ccp(0, 40));
    
	this->addChild(scrollView);
}

void HelpersLayer::onBack(cocos2d::CCObject *target)
{
	GameScene::getInstance()->popLayer();
    ring("sound/v/button_click.mp3");
}
    
void HelpersLayer::onChangeSortType(cocos2d::CCObject *target)
{
    PlayerData::getInstance()->switchHelpersSortType();
    makeItems(getHelpersBySortType());
    title->setString(getSortStr().c_str());
}
    
std::string HelpersLayer::getSortStr()
{
    switch (PlayerData::getInstance()->getHelpersSortType()) {
        case Normal_Sort:
            return "默认排序";
        case Color_Sort:
            return "属性排序";
        case HP_Sort:
            return "血量排序";
        case Attack_Sort:
            return "攻击排序";
        case Regen_Sort:
            return "回复排序";
        case Title_Sort:
            return "流派排序";
        default:
            return "默认排序";
            break;
    }
}
    
std::vector<PlayerPtr> HelpersLayer::getHelpersBySortType()
{
    std::vector<PlayerPtr> tempHelpers = helpers;
    switch (PlayerData::getInstance()->getHelpersSortType()) {
        case Normal_Sort:
            break;
        case HP_Sort:
            std::sort(tempHelpers.begin() , tempHelpers.end() , [](PlayerPtr p1 , PlayerPtr p2){
                return p1->team[0]->health != p2->team[0]->health ? p1->team[0]->health > p2->team[0]->health : p1->team[0]->profile->tableId < p2->team[0]->profile->tableId;
            });
            break;
        case Color_Sort:
            std::sort(tempHelpers.begin() , tempHelpers.end() , [](PlayerPtr p1 , PlayerPtr p2){
                return p1->team[0]->profile->color != p2->team[0]->profile->color ? p1->team[0]->profile->color < p2->team[0]->profile->color : p1->team[0]->profile->tableId < p2->team[0]->profile->tableId;
            });
            break;
        case Attack_Sort:
            std::sort(tempHelpers.begin() , tempHelpers.end() , [](PlayerPtr p1 , PlayerPtr p2){
                return p1->team[0]->attack != p2->team[0]->attack ? p1->team[0]->attack > p2->team[0]->attack : p1->team[0]->profile->tableId < p2->team[0]->profile->tableId;
            });
            break;
        case Regen_Sort:
            std::sort(tempHelpers.begin() , tempHelpers.end() , [](PlayerPtr p1 , PlayerPtr p2){
                return p1->team[0]->regen != p2->team[0]->regen ? p1->team[0]->regen > p2->team[0]->regen : p1->team[0]->profile->tableId < p2->team[0]->profile->tableId;
            });
            break;
        case Title_Sort:
            std::sort(tempHelpers.begin() , tempHelpers.end() , [](PlayerPtr p1 , PlayerPtr p2){
                int index1 = heroTitleMap[p1->team[0]->profile->title];
                int index2 = heroTitleMap[p2->team[0]->profile->title];
                return index1 != index2 ?  index1 < index2 : p1->team[0]->profile->tableId < p2->team[0]->profile->tableId;
            });
            break;
            
        default:
            break;
    }
    return tempHelpers;
}

//-------------------
// JiangHuMenuItemBG
//-------------------
    
void JiangHuMenuItemBg::setLevelsVisible(bool visible)
{
    for (auto node : levelsNode)
        node->setVisible(visible);
}

bool JiangHuMenuItemBg::init(const Stage& stage, bool progressive)
{
    if(!ContentLayer::init())
        return false;

    this->stage = stage;
    hintBoard = NULL;
    
    levelNum = stage.levelList.size();
    if (progressive)
    {
        for (int i=0;i<stage.levelList.size();i++)
            if (!stage.levelList[i].cleared)
            {
                levelNum = i + 1;
                break;
            }
    }
    
    int eachItemHeight = 82;
    
    std::string tail = stage.stage >999 & stage.stage < 2000 ? "_hong" : "";
    CCScale9Sprite* bg = Get9SpriteFromFrame(("zhanchang_3"+tail+".png").c_str(), 15, 77, 370, 1);
    
	bg->setAnchorPoint(ccp(0.5 , 1));
	addChild(bg, -2);
    
    levelsNode.clear();  
    CCSpriteBatchNode* batch = CCSpriteBatchNode::create("gameui.png", levelNum);
	for(int i=0;i<levelNum;i++)
    {
        CCPoint itemPos = stage.dailyInstanceProgress == -1 ? ccp(0, -80 - (levelNum-1-i)*eachItemHeight) :
                                                              ccp(0, -80 - i*eachItemHeight);
        const Level level = stage.levelList[i];
        string itemPath = "";
        CCSprite* item;
        if(level.levelLimit > PlayerData::getInstance()->player->level ||
                (stage.dailyInstanceProgress != -1 && i >= stage.dailyInstanceProgress))
        {
            item = GetSprite("gameui/zhanchang_2_h.png");
            item->setAnchorPoint(ccp(0.5, 1));
            item->setPosition(itemPos);
            addChild(item);
            items.push_back(item);
            if(stage.dailyInstanceProgress == -1)
            {
                itemPos = this->convertToNodeSpace(item->convertToWorldSpace(ccp(290, 25)));
                LabelX* des = LabelX::create("开放等级", FONT_CN, 20, LabelX::SHADOW, ccp(2, -2));
                des->setPosition(ccp(itemPos.x-3, itemPos.y+22));
                des->setColor(ccRED);
                addChild(des);
                LabelX* levelLimit = LabelX::create(toStr(level.levelLimit).c_str(), FONT_NUM, 18, LabelX::FontEffect::SHADOW, ccp(2, -2));
                levelLimit->setPosition(itemPos);
                levelLimit->setColor(ccRED);
                addChild(levelLimit);
                levelsNode.push_back(levelLimit);
            }
        }
        else
        {
            item = GetSprite("gameui/zhanchang_2"+tail+".png");
            item->setAnchorPoint(ccp(0.5, 1));
            item->setPosition(itemPos);
            
            batch->addChild(item);
            items.push_back(item);
            
            CCLabelTTF *spirit, *round; 
            
            itemPos = this->convertToNodeSpace(item->convertToWorldSpace(ccp(305, 25)));
            round = LabelX::create(toStr(level.roundCount).c_str(), FONT_NUM, 18, LabelX::FontEffect::SHADOW, ccp(2, -2));
            round->setPosition(itemPos);
            round->setColor(TITLE_YELLOW);
            addChild(round);
            
            itemPos = this->convertToNodeSpace(item->convertToWorldSpace(ccp(305, 50)));
            spirit = LabelX::create(toStr(level.cost).c_str(), FONT_NUM, 18, LabelX::FontEffect::SHADOW, ccp(2, -2));
            spirit->setPosition(itemPos);
            spirit->setColor(TITLE_YELLOW);
            addChild(spirit);
            
            itemPos = this->convertToNodeSpace(item->convertToWorldSpace(ccp(50, item->getContentSize().height)));
            CCSprite *sigh = NULL;
            if(!level.cleared)
                sigh = GetSprite("gameui/new.png");
            else if(stage.hasReward)
            {
                sigh = GetSprite("gameui/pingfen"+toStr(level.cleared)+".png");
                sigh->setScale(0.28);
                itemPos = ccp(itemPos.x, itemPos.y-10);
            }
            else
                sigh = GetSprite("gameui/clear.png");
            sigh->setPosition(itemPos);
            addChild(sigh, 10);
            
            levelsNode.push_back(spirit);
            levelsNode.push_back(round);
            
            if(level.dropsHint.size() > 0)
            {
                itemPos = item->convertToWorldSpace(ccp(345, item->getContentSize().height*0.5));
                ButtonSprite* btn = ButtonSprite::create("gameui/zcxq_anniu.png", kCCMenuHandlerPriority, false);
                btn->setOnClickListener([=](){
                    JiangHuMenuItem* parent = (JiangHuMenuItem*)this->getParent();
                    if(!parent->isOpen)
                        return;
                    if(!parent->hintBoard)
                        parent->createHintBoard(level, ccp(itemPos.x-190, itemPos.y-30));
                    else
                        parent->cancelHintBoard();
                });
                
                btn->setPosition(itemPos);
                addChild(btn);
            }
        }
        //level name
        CCSize size = item->getContentSize();
        {
            itemRect.push_back(CCRectMake(item->getPositionX()-size.width*0.5f, item->getPositionY()-size.height, size.width, size.height));
            
            itemPos = this->convertToNodeSpace(item->convertToWorldSpace(ccp(size.width*0.5 - 45, size.height*0.5+2)));
            
            LabelX* label = LabelX::create(level.name.c_str(), FONT_CN, 36, LabelX::FontEffect::SHADOW, ccp(2, -2));
            label->setPosition(itemPos);
            label->setAnchorPoint(ccp(0.5, 0.5));
            //label->setColor(TITLE_YELLOW);
            
            addChild(label);
            levelsNode.push_back(label);
        }
        
        
    }
    addChild(batch, -1);
    levelsNode.push_back(batch);
    
    bg->setContentSize(CCSizeMake(400 ,80 + eachItemHeight*levelNum+10));
    int adjust = 10;
	setContentSize( CCSizeMake(bg->getContentSize().width , bg->getContentSize().height+adjust));
    
    viewPort = CCSize(winSize.width , 60);
    setTouchEnabled(false);
    return true;
}

int JiangHuMenuItemBg::getTouchId(CCPoint localPoint)
{
	for(int i = 0 ; i < itemRect.size() ; i++)
	{
		if(itemRect[i].containsPoint(localPoint))
            return i;
	}
	return -1;
}
    
void JiangHuMenuItemBg::onSingleClick(cocos2d::CCTouch *touch)
{
    CCPoint localPoint = convertTouchToNodeSpace(touch);
    
	int id = getTouchId(localPoint);
    if(id != -1)
		onTouch(id);
}
    
void JiangHuMenuItemBg::onLongClick(cocos2d::CCTouch *touch)
{
    
}

void JiangHuMenuItemBg::onTouch(int id)
{
    Level l = stage.levelList[id];
    if(stage.dailyInstanceProgress != -1 && id >= stage.dailyInstanceProgress)
        SystemInform::alertMessage(GameScene::getInstance(), "关卡尚未开放！", "提示", [](){});
    else if(l.levelLimit > PlayerData::getInstance()->player->level)
    {
        SystemInform::alertMessage(GameScene::getInstance(), "你的等级不足！", "提示", [](){});
    }
    else if(stage.dailyQuota == -1 || stage.dailyQuota > 0)
    {
        HelpersLayer* layer = HelpersLayer::create(stage.levelList[id]);
        layer->targetStage = stage;
        layer->checkIfStageEnd();
        GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
        ring("sound/v/button_click.mp3");
        
        if(GameConfig::shared()->getTutorialProgress())
            ArrowMask::shared()->clear();
        
        return;
    }
    else
        SystemInform::alertConfirmPanel(runningScene(), "该关卡已达到最大次数，升级VIP可获得更多的活动次数", "提示",
                                        [](){},
                                        [](){GameScene::getInstance()->pushLayerWithDefaultAnimation(VIPLayer::create());},
                                        false,
                                        "确定",
                                        "VIP");

}


//make the visible region
void JiangHuMenuItemBg::visit()
{
    CCDirector *director = CCDirector::sharedDirector();

	float n_width = viewPort.width ;
    float n_height = viewPort.height ;

	CCPoint pos = getPosition();
	CCPoint pos2 = ccp(pos.x - n_width*0.5f , pos.y - n_height);
    
    // apply world transform
	CCPoint temp = getParent()->convertToWorldSpace(pos2);

    float x = temp.x;
    float y = temp.y;
    
    glEnable(GL_SCISSOR_TEST);
	director->getOpenGLView()->setScissorInPoints(x, y, n_width, n_height);
    CCLayer::visit();
    glDisable(GL_SCISSOR_TEST);

}
    
//--------------------------
// JiangHuMenuItem
//--------------------------
bool JiangHuMenuItem::init(const Stage& stage, bool progressive)
{
    if(!ContentLayer::init())
        return false;
     
    padding = 70;
    isOpen = false;
	isClick = false;
    this->stage = stage;
    this->progressive = progressive;
    eventLabel = NULL;
    bg = NULL;
    sigh = NULL;
    rewardBtn = NULL;
    hintBoard = NULL;
    ContentLayer::setTouchEnabled(true);
    
    addLabelBMFont(this , timeLabel ,"" , "bmfont/PW_White_24.fnt"  , ccp(200 , 10) , ccp(1 , 0.5));
    timeLabel->setScale(contentScale());
    timeLabel->setZOrder(10);
    
    //this can't added on zhou2 so it will adjust pos in update function 
    addLabelBMFont(this, quoteTimesLabel ,"" , "bmfont/PW_Yellow_24.fnt", ccp(200 , -100) , ccp(1 , 0.5));
    quoteTimesLabel->setScale(contentScale());
    quoteTimesLabel->setZOrder(10);
    
    updateStage(stage);

    {
        std::string path = GemUtils::stageColorRes(stage.stageColor);
        gemSprite = GetSprite(path.c_str());
        gemSprite->setPosition(ccp(173, -28));
        gemSprite->setAnchorPoint(ccp(0.5, 0.5));
        this->addChild(gemSprite, 4);
    }
    
    //CCSpriteBatchNode* menuBatch = CCSpriteBatchNode::create("gameui.png", 4);
    {
        std::string tail = stage.stage >999 & stage.stage < 2000 ? "_hong" : "";
        marginLeft = GetSprite("gameui/zhanchang_2_2"+tail+".png");
        marginLeft->setAnchorPoint(ccp(1, 1));
        marginLeft->setPosition(ccp(-200, 0));
        this->addChild(marginLeft);
        
        marginRight = GetSprite("gameui/zhanchang_2_2"+tail+".png");
        marginRight->setAnchorPoint(ccp(0, 1));
        marginRight->setPosition(ccp(199, 0));
        marginRight->setFlipX(true);
        this->addChild(marginRight);
        
        zhou1 = GetSprite("gameui/zhanchang_1_1"+tail+".png");
        zhou1->setAnchorPoint(ccp(0.5 , 0.5));
        zhou1->setPosition(ccp(0, 0));
        this->addChild(zhou1);
        
        zhou2 = GetSprite("gameui/zhanchang_1_2"+tail+".png");
        zhou2->setAnchorPoint(ccp(0.5 , 0.5));
        zhou2->setPosition(ccp(0, -bg->viewPort.height));
        this->addChild(zhou2);
    }
    //this->addChild(menuBatch);
 
    
    title = LabelX::create(this->stage.name.c_str(), FONT_CN, 42, LabelX::FontEffect::SHADOW, ccp(3, -3));
    title->setColor(ccWHITE);
	title->setAnchorPoint(ccp(0.5 , 1));
    title->setPosition(ccp(-20, -3));
	this->addChild(title);
    
	makeTouchRegion();
    
    eventTimeCount(0.0);
    schedule(schedule_selector(JiangHuMenuItem::eventTimeCount) , 1);
    this->scheduleUpdate();
    

    return true;
}

void JiangHuMenuItem::onExit()
{
    ContentLayer::onExit();
    if(hintBoard)
        cancelHintBoard();
//    GameScene::recycle(this);
}
    
//this method used to controll some changeable UI 
void JiangHuMenuItem::updateStage(const Stage& _stage)
{
    if(bg)
    {
        for(int i = 0 ; i < stage.levelList.size() ; i++)
            if(stage.levelList[i].cleared != _stage.levelList[i].cleared ||
               stage.dailyInstanceProgress != _stage.dailyInstanceProgress)
            {
                bg->removeFromParentAndCleanup(true);
                bg = NULL;
                break;
            }
    }
    
    this->stage = _stage;
    
    if(!bg)
    {
        bg = JiangHuMenuItemBg::create(stage, progressive);
        this->addChild(bg,-10);
        setContentSize(CCSize(bg->viewPort.width , bg->viewPort.height+padding));
        //setIsOpen(isOpen);
    }
    bg->stage = this->stage;
    
    if(hintBoard)
        cancelHintBoard();
    
    if(rewardBtn)
    {
        rewardBtn->removeFromParentAndCleanup(true);
        rewardBtn = NULL;
    }
    
    if(stage.hasReward)
    {
        if(stage.rewardAvailable)
        {
            rewardBtn = ButtonSprite::create(createRewardSpr(stage), createRewardSpr(stage));
            rewardBtn->setOnClickListener([=](){
                if(!stage.isAllPass())
                {
                    std::string msg = "全S通关可打开礼包！";
                    if(PlayerData::getInstance()->isDailyInstanceByStage(_stage.id))
                    {
                        msg = "通关震天七言可获得绝世大礼包！";
                    }
                    SystemInform::alertMessage(runningScene(), msg.c_str(), "提示", [](){});
                }
                else
                    PuzzleRPC::getInstance()->getFullhouseReward(PlayerData::getInstance()->getToken(), stage.id,
                             [=](std::auto_ptr<Player> player,
                                 ChestRewards rewards,
                                 std::auto_ptr<Error> e)
                             {
                                 if(e.get())
                                 {
                                     e->handleErrorInform();
                                     return;
                                 }
                                 
                                 PlayerData* data = PlayerData::getInstance();
                                 data->updatePlayer(player.get());
                                 int normalSize = data->normalStageList.size();
                                 int size = normalSize + data->instanceStageList.size();
                                 for(int i = 0; i < size; i++)
                                    if(i < normalSize && data->normalStageList[i]->id == stage.id)
                                        data->normalStageList[i]->rewardAvailable = false;
                                    else if(i >= normalSize && data->instanceStageList[i-normalSize]->id == stage.id)
                                        data->instanceStageList[i-normalSize]->rewardAvailable = false;
                                 
                                 stage.rewardAvailable = false;
                                 this->updateStage(stage);
                                 
                                 SystemInform::alertRewardTips(runningScene(),
                                                              "恭喜获得评分礼包",
                                                              rewards, [](){});
                             });
            });
            rewardBtn->setPosition(ccp(-190 , 0));
            if(stage.isAllPass())
            {
                rewardBtn->getSprite()->removeAllChildrenWithCleanup(true);
                rewardBtn->getPressedSprite()->removeAllChildrenWithCleanup(true);
                rewardBtn->runAction(CCRepeatForever::create(CCSequence::create(CCScaleTo::create(0.3, 1.2),
                                                                                CCScaleTo::create(0.3, 1),
                                                                                NULL)));
            }
        }
        else
        {
            rewardBtn = ButtonSprite::create("gameui/clear.png", "gameui/clear.png", kCCMenuHandlerPriority, false);
            rewardBtn->setPosition(ccp(-190 , 15));
        }
        
        addChild(rewardBtn, 20);
    }
    else
    {
        bool isNew = false;
        for(const Level& l : _stage.levelList)
            if(!l.cleared)
            {
                isNew = true;
                break;
            }
        
        if(isNew && !sigh)
        {
            sigh = GetSprite("gameui/new.png");
            sigh->setPosition(ccp(-190 , 10));
            addChild(sigh,10);
        }
        else if(!isNew && sigh)
        {
            sigh->removeFromParentAndCleanup(true);
            sigh = NULL;
        }
    }
    
    if(stage.dailyQuota > 0)
    {
        quoteTimesLabel->setFntFile("bmfont/PW_Yellow_24.fnt");
        quoteTimesLabel->setString(("剩余次数:" + toStr(stage.dailyQuota)).c_str());
    }
    else if(stage.dailyQuota == 0)
    {
        quoteTimesLabel->setFntFile("bmfont/PW_24_red.fnt");
        quoteTimesLabel->setString("今日已无次数");
    }
    else
        quoteTimesLabel->setString(" ");

	setIsOpen(false);

}
    
void JiangHuMenuItem::createHintBoard(const Level& level, CCPoint pos)
{
    
    if(hintBoard)
        cancelHintBoard();
        
    std::vector<int> hints = level.dropsHint;
    int size = hints.size();
    int col = 4;
    int row = size/5+1;
    int height = 0;
    
    hintBoard = ContentLayer::create();
    
    for(int i = 0; i < size; i++)
    {
        int hint = hints[i];
        CCSprite* icon;
        if(hint == LoginRewardType::Reward_Exp)
        {
            icon = GetSprite("gameui/jiangli_bg.png");
            icon->setScale(0.5);
            CCSprite* s = GetSprite("gameui/jingyantubiao.png");
            s->setPosition(ccp(icon->getContentSize().width*0.5f, icon->getContentSize().height*0.5f));
            icon->addChild(s);
        }
        else if(hint == LoginRewardType::Reward_Coins)
        {
            icon = GetSprite("gameui/jiangli_bg.png");
            icon->setScale(0.5);
            CCSprite* s = GetSprite("gameui/yiduitongqian.png");
            s->setPosition(ccp(icon->getContentSize().width*0.5f, icon->getContentSize().height*0.5f));
            icon->addChild(s);
        }
        else
        {
            icon = HeroIcon::createWithProfile(&HPT(hints[i]));
            icon->setScale(0.6);
            icons.push_back((HeroIcon*)icon);
        }
        icon->setAnchorPoint(ccp(0, 1));
        icon->setPosition(ccp(-135+i%col*70, 20+row*70-70*(i/col)));
        hintBoard->addChild(icon);
    }
    CCLabelTTF* drop;
    drop = addLabelTTF(hintBoard, "几率掉落：", FONT_CN, 22, ccp(-140, 40+row*70), ccp(0, 0.5), ccc3(0xff, 0xd2, 0x00));
    
    height += 40+row*70 + drop->getContentSize().height;
    
    if(!stage.hint.empty())
    {
        CCLabelTTF* hint;
        hint = addLabelTTF(hintBoard, stage.hint.c_str(), FONT_CN, 22, ccp(-140, height), ccp(0, 0.5), ccc3(0xff, 0xd2, 0x00));
        height += hint->getContentSize().height;
    }
    
    addLabelTTF(hintBoard, level.name.c_str(), FONT_CN, 26, ccp(0, height+5), ccp(0.5, 0.5), ccc3(0xff, 0xd2, 0x00));
    
    CCScale9Sprite* bg = Get9SpriteFromFrame("zcxq_bg.png", 13, 55, 295, 1);
    bg->setContentSize(CCSizeMake(324, height+35));
    bg->setAnchorPoint(ccp(0.5, 0));
    hintBoard->addChild(bg, -1);
    
    hintBoard->setPosition(convertToWorldSpace(pos));
    GameScene::getInstance()->addChild(hintBoard, 100);
}
    
void JiangHuMenuItem::cancelHintBoard()
{
    if(hintBoard)
    {
        hintBoard->removeFromParentAndCleanup(true);
        hintBoard = NULL;
        icons.clear();
    }
    
}
    
CCSprite* JiangHuMenuItem::createRewardSpr(const Stage& stage)
{
    int total = stage.levelList.size();
    int clearCount = 0;
    for(const Level& level : stage.levelList)
        if(level.cleared == 1)
            clearCount++;
    
    int color = stage.isAllPass() ? 255 : 100;
    CCSprite* bg = GetSprite("gameui/slibao.png");
    bg->setColor(ccc3(color, color, color));
    
    CCSprite* board = GetSprite("gameui/slibao_bg.png");
    addLabelBMFont(board,  (toStr(clearCount)+"/"+toStr(total)).c_str(),
                   "bmfont/Sumo_22_yellow.fnt", ccp(75, 24))->setScale(contentScale());
    board->setPosition(ccp(20, 30));
    bg->addChild(board);
    
    return bg;
}
    
void JiangHuMenuItem::setTouchEnabled(bool value)
{
    ContentLayer::setTouchEnabled(value);
    bg->setTouchEnabled(value);
}

void JiangHuMenuItem::onTitleClick()
{
    ring("sound/v/scroll_unfold.mp3");
    
    itemOpenSwitch();
    if(isOpen)
        ((MenuScrollView*)getParent()->getParent())->moveTo(getTag());
    jianghu->itemOpenHandle(this);
    
    
    saveJiangHuState();
}
    
void JiangHuMenuItem::saveJiangHuState()
{
    GameConfig::shared()->stageTab = (int)jianghu->type;
    GameConfig::shared()->stageId = stage.stage;
}

void JiangHuMenuItem::itemOpenSwitch()
{
	setIsOpen(!isOpen);
}

void JiangHuMenuItem::setIsOpen(bool value)
{
	isOpen = value;
	bg->setTouchEnabled(isOpen);
    if (value)
        bg->setLevelsVisible(true);
}

void JiangHuMenuItem::makeTouchRegion()
{
	CCSize size = zhou1->getContentSize();
	CCSize size2 = zhou2->getContentSize();
	zhou1Rect = CCRectMake(zhou1->getPositionX()-size.width*0.5f , zhou1->getPositionY()-size.height*0.5f-20 , size.width , size.height+20);
	zhou2Rect = CCRectMake(zhou2->getPositionX()-size2.width*0.5f , zhou2->getPositionY()-size2.height*0.5f , size2.width , 30);
}
    
void JiangHuMenuItem::stopAllSchedule(bool value)
{
    if(value)
    {
        unschedule(schedule_selector(JiangHuMenuItem::eventTimeCount));
        this->unscheduleUpdate();
    }
    else
    {
        schedule(schedule_selector(JiangHuMenuItem::eventTimeCount) , 1);
        this->scheduleUpdate();
    }
}

void JiangHuMenuItem::checkEvent()
{
    unschedule(schedule_selector(JiangHuMenuItem::eventTimeCount));
    schedule(schedule_selector(JiangHuMenuItem::eventTimeCount) , 1);
    
    eventTimeCount(0.0);
}

void JiangHuMenuItem::eventTimeCount(float dt)
{
    ptime now = boost::posix_time::second_clock::universal_time() +
                PlayerData::getInstance()->delta_local_server;
    //when it has event.
    if(stage.startTimeStamp != 0 && stage.endTimeStamp != 0)
    {
        ptime start = from_time_t(stage.startTimeStamp);
        ptime end = from_time_t(stage.endTimeStamp);
        
        auto dur_start_cur = start - now;
        auto dur_end_cur = end - now;
        
        timeLabel->setString("");
        std::string des = "";
        switch (stage.event)
        {
            case StageEvent::NoEvent:
                des = "";
                break;
            case StageEvent::CoinsTwice:
                des = "銅錢翻倍";
                break;
            case StageEvent::DropOneDotFive:
                des = "掉率1.5倍";
                break;
            case StageEvent::DropTwice:
                des = "掉率翻倍";
                break;
            case StageEvent::SpiritHalf:
                des = "體力減半";
                break;
            default:
                des = "";
                break;
        }
        
        if(!des.empty())
        {
            if(!eventTime)
            {
                addLabelBMFont(this, eventTime, "",
                               "bmfont/Sumo_24_white.fnt",
                               ccp(200 , 10),
                               ccp(1.0f, 0.5));
                eventTime->setScale(contentScale());
            }
            if(!dur_start_cur.is_negative())
            {
                //if eventLabel has not been initialized,init it. otherwise,update the time of it
                
                char ss[100];
                if (dur_start_cur.hours() > 0)
                    sprintf(ss, " 還有%02d:%02d:%02d開啓", dur_start_cur.hours(), dur_start_cur.minutes(), dur_start_cur.seconds());
                else
                    sprintf(ss, " 還有%02d:%02d開啓", dur_start_cur.minutes(), dur_start_cur.seconds());
                eventTime->setString((des+ss).c_str());
            }
            else if(dur_start_cur.is_negative() && !dur_end_cur.is_negative())
            {
                char ss[100];
                if (dur_end_cur.hours() > 0)
                    sprintf(ss, " 剩餘%02d:%02d:%02d", dur_end_cur.hours(), dur_end_cur.minutes(), dur_end_cur.seconds());
                else
                    sprintf(ss, " 剩餘%02d:%02d", dur_end_cur.minutes(), dur_end_cur.seconds());
                eventTime->setString((des+ss).c_str());
                
            }
            else
                eventTime->setString("");
        }
        
  
        
    }
    //if no event check if has visible time
    else
    {
        endEvent(0.0);
        
        if (stage.visibleEnd == not_a_date_time)
        {
            timeLabel->setString("");
            unschedule(schedule_selector(JiangHuMenuItem::eventTimeCount));
        }
        else if(stage.visibleEnd >= now)
        {
            auto dur_vend_now = stage.visibleEnd - now;
            char ss[100];
            if (dur_vend_now.hours() >= 24)
                sprintf(ss, "剩余%d天结束", int(dur_vend_now.hours()/24));
            else if(dur_vend_now.hours() >= 1)
                sprintf(ss, "剩余%d小时结束", dur_vend_now.hours());
            else if(dur_vend_now.minutes() >= 1)
                sprintf(ss, "剩余%d分钟结束", dur_vend_now.minutes());
            else
                sprintf(ss, "剩余%d秒结束", dur_vend_now.seconds());
            timeLabel->setString(ss);
        }
        else
        {
            timeLabel->setString("");
            jianghu->initStage();
            unschedule(schedule_selector(JiangHuMenuItem::eventTimeCount));
        }
    }
        
}
    
void JiangHuMenuItem::endEvent(float dt)
{
    if(eventLabel)
    {
        eventLabel->removeChild(eventTime, true);
        removeChild(eventLabel, true);
        eventTime = NULL;
        eventLabel = NULL;
    }
}

void JiangHuMenuItem::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	isClick = true;
    
    if(hintBoard)
    {
        for(HeroIcon* icon : icons)
        {
            CCTouch* touch = (CCTouch*)pTouches->anyObject();
            CCPoint localPoint = icon->getParent()->convertTouchToNodeSpace(touch);
            if(icon->boundingBox().containsPoint(localPoint))
            {
                HeroProfileLayer* layer = HeroProfileLayer::create(icon->getHero()->profile);
                GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
                break;
            }
        }
        
        cancelHintBoard();
    }
}
void JiangHuMenuItem::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch *touch = (CCTouch*)pTouches->anyObject();
	CCPoint currentPoint = (CCPoint)touch->getLocation();
	CCPoint prePoint = (CCPoint)touch->getPreviousLocation();
	float l = ccpDistance(currentPoint , prePoint);
	if(l>5)
        isClick = false;
}

void JiangHuMenuItem::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch *touch = (CCTouch*)pTouches->anyObject();
	CCPoint point = convertTouchToNodeSpace(touch);
    //for there are some conditions that item was removed from parent and touchended still be called.
    if(!getParent())
        return;
    MenuScrollView *sv = (MenuScrollView*)getParent()->getParent();
    CCPoint svLocalPoint = sv->convertTouchToNodeSpace(touch);
	if(isClick && sv->boundingBox().containsPoint(svLocalPoint)
       && (zhou2Rect.containsPoint(point) || zhou1Rect.containsPoint(point) ) && isTouchEnabled() )
    {
		onTitleClick();
		isClick = false;
	}
}


void JiangHuMenuItem::update(float dt)
{
    if(isOpen && easing(-bg->getContentSize().height))
    {
        makeTouchRegion();
    }
    else if (!isOpen && easing(-68))
    {
        makeTouchRegion();
        bg->setLevelsVisible(false);
    }
    bg->viewPort.height = abs(zhou2->getPositionY());
    float scale = bg->viewPort.height / marginLeft->getContentSize().height;
    marginLeft->setScaleY(scale);
    marginRight->setScaleY(scale);
    
    setContentSize(CCSize(bg->viewPort.width , bg->viewPort.height+padding));
}

  //to make the easing movement of scollview
bool JiangHuMenuItem::easing(float tY)
{
	float tempY = zhou2->getPositionY();
	zhou2->setPositionY( tempY + (tY - tempY) * 0.3);
    quoteTimesLabel->setPositionY(zhou2->getPositionY());

	if(abs(tY - zhou2->getPositionY())<0.1 ){
		zhou2->setPositionY(tY);
        quoteTimesLabel->setPositionY(zhou2->getPositionY());
		return true;
	}
	return false;
}

    
bool JiangHuMenuItemBlack::init(const Stage& stage, int kind)
{
    if(!ContentLayer::init())
        return false;
    
    this->stage = stage;
    
    CCSprite* bg = GetSprite("gameui/caidan_1_ty2.png");
    bg->setAnchorPoint(ccp(0.5 , 1));
    bg->setPositionY(30);
    addChild(bg);
    
    title = LabelX::create(this->stage.name.c_str(), FONT_CN, 42, LabelX::FontEffect::SHADOW, ccp(3, -3));
    title->setColor(ccWHITE);
	title->setAnchorPoint(ccp(0.5 , 1));
    title->setPosition(ccp(-20, -3));
	this->addChild(title);
    
    addLabelBMFont(this, timeLabel, "" , "bmfont/PW_Red_24.fnt"  , ccp(200 , 10) , ccp(1 , 0.5));
    timeLabel->setScale(contentScale());
    
    if(kind == 0)
        this->scheduleUpdate();
    else if(kind == 1)
    {
        int l = (stage.levelList.end()-1)->levelLimit;
        timeLabel->setString((toStr(l)+"级可进入").c_str());
    }

    setContentSize(CCSizeMake(winSize.width, 138));

    
    return true;
}

void JiangHuMenuItemBlack::update(float dt)
{
    ptime now = boost::posix_time::second_clock::universal_time() +
                PlayerData::getInstance()->delta_local_server;
    
    auto dur_vend_now = stage.visibleStart - now;
    char ss[100];
    if (dur_vend_now.hours() >= 24)
        sprintf(ss, "剩余%d天开启", int(dur_vend_now.hours()/24));
    else if(dur_vend_now.hours() >= 1)
        sprintf(ss, "剩余%d小时开启", dur_vend_now.hours());
    else if(dur_vend_now.minutes() >= 1)
        sprintf(ss, "剩余%d分钟开启", dur_vend_now.minutes());
    else
        sprintf(ss, "剩余%d秒开启", dur_vend_now.seconds());
    timeLabel->setString(ss);
    
    if(now > stage.visibleStart)
        jianghu->initStage();
}

//---------------------------------------
// JiangHu layer
//---------------------------------------

bool JiangHu::init(JiangHuType _type)
{   
	if( !ContentLayer::init() ){
		return false;
	}

	currOpenItem = NULL;
    scrollView = NULL;
    type = _type;
    
    string title = type == JiangHuType::ZhanYi ? "战役" : "活动";
    createTitleMenu(this, title, "", menu_selector(JiangHu::onBack));
    
    initStage();
    
    GameConfig::shared()->stageTab = (int)_type;

	return true;
}
    
void JiangHu::onEnter()
{
    ContentLayer::onEnter();
    PlayerData::getInstance()->registerStageCallBack([this]()
                                                     {
                                                         initStage();
                                                     });
    if(GameConfig::shared()->getTutorialProgress() == TutorialFirstBattle)
    {
        ((CCMenu*)getChildByTag(10000))->setTouchEnabled(false);
        ArrowMask::shared()->setMask(CCRect(0, 140, 640, 685));
    }
    else if(type == JiangHuType::HuoDong && GameConfig::shared()->firstTimeShowInstance)
    {
        ArrowMask::shared()->clear();
        GameConfig::shared()->firstTimeShowInstance = false;
        GameConfig::shared()->save();
        
        AlertMessage::create("", "活动类型：活动分为日常活动和节日活动！日常活动每天可进入的次数有限，\t而且等级越高可以参加的活动越多哦！掌门请加油升级吧！");
    }
}
    
void JiangHu::onEnterAnimationDone()
{
    ContentLayer::onEnterAnimationDone();
    
    if(GameConfig::shared()->getTutorialProgress() == TutorialFirstBattle)
    {
        items[0]->onTitleClick();
        ArrowMask::shared()->setArrow(ccp(100, 616));
    }
    // if player level exceeds 10 and we never showed instance stage tutorials, do it.

    //else if(
}
    
void JiangHu::onExit()
{
    ContentLayer::onExit();
    PlayerData::getInstance()->unRegisterStageCallBack();
}
    
void JiangHu::onBack()
{
    GameScene::getInstance()->popLayer();
}

void JiangHu::setTouchEnabled(bool value)
{
	ContentLayer::setTouchEnabled(value);
	if(currOpenItem)
        currOpenItem->setTouchEnabled(value);
}

void JiangHu::tutorialCall(TutorialControl* control)
{
    //ontrol->checkJiangHu(this);
}
    
void JiangHu::makeMenu(std::vector<boost::shared_ptr<Stage>>& stages, bool progressive)
{
	if(scrollView)
    {
         this->removeChild(scrollView, true);
        for(JiangHuMenuItem* item : items)
        {
            ObjectPool<JiangHuMenuItem>::getInstance()->collectObj(item);
        }
    }
    
	scrollView = MenuScrollView::create();
    items.clear();
    
    // find first stage that palyer has not yet cleared
    int stageProgress = stages.size()-1;    
    ptime now = second_clock::universal_time() +
                    PlayerData::getInstance()->delta_local_server;
    if (progressive)
    {
        for (stageProgress = 0; stageProgress < stages.size(); stageProgress++)
        {
            auto& levels = stages[stageProgress]->levelList;
            if (find_if(levels.begin(), levels.end(), [](const Level& l){ return !l.cleared; }) != levels.end())
                break;
        }
    }
    else
    {
        std::sort(stages.begin(), stages.end(),
                  [=](boost::shared_ptr<Stage> s1, boost::shared_ptr<Stage> s2){
                      return getStagePriority(s1) != getStagePriority(s2) ?
                                  getStagePriority(s1) > getStagePriority(s2) :
                                  s1->stage < s2->stage;
                  });
    }
    stageProgress = min(stageProgress, (int)stages.size()-1);

    
    vector<JiangHuMenuItemBlack*> blackItems;
	for(int i=stageProgress; i>=0; i--)
    {
//        LOGD("stage[%d] visibleStart:%s, visibleEnd:%s",
//             stages[i].stage,
//             to_simple_string(stages[i].visibleStart).c_str(),
//             to_simple_string(stages[i].visibleEnd).c_str());
        bool isLevelsOpen = isLevelsAllOpen(*stages[i]);
        if (stages[i]->visibleStart == not_a_date_time ||
            (now>stages[i]->visibleStart && now<stages[i]->visibleEnd))
        {
            if(isLevelsOpen)
            {
                JiangHuMenuItem* item = ObjectPool<JiangHuMenuItem>::getInstance()->getObject(*stages[i], progressive);
                item->setTag(items.size());
                item->jianghu = this;
                scrollView->addLayer(item);
                items.push_back(item);
            }
            else
            {
                JiangHuMenuItemBlack* item = JiangHuMenuItemBlack::create(*stages[i], 1);
                item->jianghu = this;
                blackItems.push_back(item);
                auto dur = now - time_from_string("2010-01-01 00:00:00.000");
                item->setTag(dur.total_seconds());
            }

        }
        else if( now < stages[i]->visibleStart)
        {
            JiangHuMenuItemBlack* item = JiangHuMenuItemBlack::create(*stages[i]);
            item->jianghu = this;
            blackItems.push_back(item);
            auto dur = stages[i]->visibleStart-now;
            item->setTag(dur.total_seconds());
        }
    }
    std::sort(blackItems.begin(), blackItems.end(),
              [=](JiangHuMenuItemBlack* item1, JiangHuMenuItemBlack* item2)
                {
                    return item1->getTag() < item2->getTag();
                });
    
    for(JiangHuMenuItemBlack* item : blackItems)
        scrollView->addLayer(item);
    
	scrollView->setPosition(ccp(0, 30));
	scrollView->setViewSize(CCSize(winSize.width, 700), CCRect(0, -110, winSize.width, 600));
	this->addChildWithAnim(scrollView, true);
}
    
int JiangHu::getStagePriority(boost::shared_ptr<Stage>& s)
{
    if(s->dailyQuota!=0 && s->stage>=1000 && s->stage<2000)
        return 1;
    else if(s->dailyQuota!=0)
        return 2;
    else if(s->stage>=1000 && s->stage<2000)
        return 3;
    else
        return 4;
}

void JiangHu::itemOpenHandle(JiangHuMenuItem *item)
{
	currOpenItem = item;
	for(JiangHuMenuItem *i : items){
		if(i != item)i->setIsOpen(false);
	}
}

void JiangHu::changeStage(CCObject* object)
{
    ring("sound/v/button_click.mp3");
    
    /*if(object && itemSelected == object)
        return;
    
    if(object)
        itemSelected = (CCMenuItem*)object;
    selectItemExclusive(itemSelected);
    
    if(itemSelected == zhanyiItem)
    {
        makeMenu(PlayerData::getInstance()->normalStageList, true);
    }
    else if(itemSelected == jingjiItem)
    {
        makeMenu(PlayerData::getInstance()->instanceStageList, false);
    }*/
}
    
void JiangHu::initStage()
{
    if(type == JiangHuType::ZhanYi)
        makeMenu(PlayerData::getInstance()->normalStageList, true);
    else
    {
        makeMenu(PlayerData::getInstance()->instanceStageList, false);
    }
}

void JiangHu::cleanup()
{
    CCNode::cleanup();
    for(JiangHuMenuItem* item : items)
    {
        ObjectPool<JiangHuMenuItem>::getInstance()->collectObj(item);
    }
}
    
bool JiangHu::isLevelsAllOpen(const Stage& stage)
{
    bool value = false;
    for(const Level& l : stage.levelList)
        if(l.levelLimit <= PlayerData::getInstance()->player->level)
            value = true;
    return value;
}

}
