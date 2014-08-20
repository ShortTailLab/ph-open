//
//  WashRelation.cpp
//  HelloCpp
//
//  Created by zql on 13-5-2.
//
//

#include "DebugMisc.h"
#include "WashRelation.h"
#include "PuzzleRPC.h"
#include "GameScene.h"
#include "UIPresets.h"
#include "HeroProfileLayer.h"
#include "CCCallLambda.h"
#include "ZqlScrollView.h"
#include "PageScrollView.h"
#include "HeroIcon.h"
#include "Common.h"

/////////////////////////////////////////////////////////////////////
//WashRelation
/////////////////////////////////////////////////////////////////////

namespace PH
{
    
    bool RelationItem::init(PH::HeroRelation &_relation)
    {
        if(!CCSprite::initWithFile("gameui/yuanfen_hong_xiao.png"))
            return false;
        
        relation = _relation;
        
        CCSize size = getContentSize();
        mask = GetSprite("gameui/yuanfen_lv_xiao.png");
        mask->setTag(10);
        mask->setVisible(false);
        mask->setPosition(ccp(size.width*0.5f, size.height*0.5f));
        addChild(mask);
        label = CCLabelTTF::create(relation.name.c_str(),
                                               FONT_CN,
                                               32,
                                               CCSize(80, 80),
                                               kCCTextAlignmentCenter);
        
        label->setPosition(ccp(size.width*0.5f, size.height*0.5f-5));
        label->setAnchorPoint(ccp(0.5f,0.5f));
        addChild(label);
        return true;
    }
    
    void RelationItem::setOpacity(GLubyte value)
    {
        CCSprite::setOpacity(value);
        mask->setOpacity(value);
        label->setOpacity(value);
    }
    
    void RelationItem::lock(bool value)
    {
        mask->setVisible(value);
    }
    
    
bool RelationJuanZhou::init(Hero* hero)
{
    if(!JuanZhou::init())
        return false;
    
    viewPort = CCSize(20 , 204);
    heroNum = 1;
    
    CCSprite *bg = GetSprite("gameui/xiyuanfen_1_3.png");
    bg->setAnchorPoint(ccp(0.5 , 1));
    addChild(bg);
    
    itemContainer = CCLayer::create();
    addChild(itemContainer);
    
    locations.push_back(CCRectMake(-257 , -151 , 100 , 100));
    heroVec = std::vector<Hero*>(1 , (Hero*)NULL);
    heroImgVec = std::vector<HeroIcon*>(1, (HeroIcon*)NULL);
    
    for(int i =0; i<3;i++)
        itemPos.push_back(ccp(-64+136*i, -156));
    
    addHero(hero, 0);
    
    listRelations();
    
    setTouchEnabled(true);
    
    return true;
}
    
void RelationJuanZhou::listRelations()
{
    itemContainer->removeAllChildrenWithCleanup(true);
    relItems.clear();
    btns.clear();
    
    for(int i = 0; i < 3; i++)
    {
        if(i < heroVec[0]->relations.size())
        {
            RelationItem* item = RelationItem::create((heroVec[0]->relations[i]));
            item->setPosition(ccp(-64+136*i, -78));
            relItems.push_back(item);
            itemContainer->addChild(item);
            
            CCSprite* btn = createBtn("锁定", ButtonType::SMALL_RED);
            CCSprite* btnMask = createBtn("解锁", ButtonType::SMALL_BLUE);
            btnMask->setPosition(ccp(btn->getContentSize().width*0.5f, btn->getContentSize().height*0.5f));
            btnMask->setTag(10);
            btnMask->setVisible(false);
            btn->addChild(btnMask);
            btn->setPosition(itemPos[i]);
            btns.push_back(btn);
            itemContainer->addChild(btn);
        }
        else
        {
            CCSprite* item = GetSprite("gameui/xiyuanfen_1_4.png");
            item->setPosition(ccp(-64+136*i, -100));
            itemContainer->addChild(item);
        }
    }
}
    
void RelationJuanZhou::lock(int i, bool value)
{
    relItems[i]->lock(value);
    lockRelItem(btns[i], value);
}
    
void RelationJuanZhou::onSingleClick(CCTouch* touch)
{
    CCPoint localPoint = convertTouchToNodeSpace(touch);
    for(int i = 0; i < btns.size(); i++)
        if(btns[i]->boundingBox().containsPoint(localPoint))
        {
            attachLayer->lock(i);
        }
    
    for(int j = 0; j < relItems.size(); j++)
        if(relItems[j]->boundingBox().containsPoint(localPoint))
            attachLayer->locateBoard(relItems[j]->relation);
    
    if(heroImgVec[0]->boundingBox().containsPoint(localPoint))
        GameScene::getInstance()->pushLayerWithDefaultAnimation(HeroProfileLayer::create(heroVec[0]));
}
    
void RelationJuanZhou::onLongClick(CCTouch* touch)
{
    CCPoint localPoint = convertTouchToNodeSpace(touch);
    if(heroImgVec[0]->boundingBox().containsPoint(localPoint))
        GameScene::getInstance()->pushLayerWithDefaultAnimation(HeroProfileLayer::create(heroVec[0]));
}
    
CCSprite* RelationJuanZhou::createRelItem(HeroRelation& r)
{
    CCSprite* item = GetSprite("gameui/yuanfen_lv_xiao.png");
    CCSize size = item->getContentSize();
    CCSprite *mask = GetSprite("gameui/yuanfen_hong_xiao.png");
    mask->setTag(10);
    mask->setVisible(false);
    mask->setPosition(ccp(size.width*0.5f, size.height*0.5f));
    item->addChild(mask);
    CCLabelTTF* label = CCLabelTTF::create(r.name.c_str(),
                                           FONT_CN,
                                           32,
                                           CCSize(80, 80),
                                           kCCTextAlignmentCenter);
    
    label->setPosition(ccp(size.width*0.5f, size.height*0.5f));
    label->setAnchorPoint(ccp(0.5f,0.5f));
    item->addChild(label);
    
    return item;
}

void RelationJuanZhou::lockRelItem(CCSprite* target, bool value)
{
    if(target->getChildByTag(10))
        target->getChildByTag(10)->setVisible(value);
}
    
    
bool RelationBoard::init(HeroRelation& _relation)
{
    if(!CCSprite::init())
        return false;
    relation = _relation;
    int MaxTextWidth = 255;
    
    mask = NULL;
    sigh = NULL;
    bg = GetSprite("gameui/yuanfen_hong_neirong.png");
    addChild(bg,-2);
    
    mask = GetSprite("gameui/yuanfen_lv_neirong.png");
    lockSigh = GetSprite("gameui/suo.png");
    lockSigh->setPosition(ccp(262,270));
    mask->addChild(lockSigh);
    mask->setVisible(false);
    addChild(mask,-1);
    
    size = bg->getContentSize();
    
    addLabelTTF(this, name, relation.name.c_str() , FONT_CN, 40, ccp(0,size.height*0.5-42), ccp(0.5,0.5));
    
    std::string nameMsg;
    for(int id : relation.withHeroes)
        nameMsg += (HPT(id).nameCN + " ");
    
    heroLabel = CCLabelTTF::create(nameMsg.c_str(),
                                               FONT_CN,
                                               26,
                                               CCSize(MaxTextWidth, 0),
                                               kCCTextAlignmentCenter);
    heroLabel->setAnchorPoint(ccp(0.5, 0.5));
    heroLabel->setPosition(ccp(0,30));
    addChild(heroLabel);
    
    std::string effectStr = "";
    for(boost::tuple<RelationEffects, double> t : relation.effects)
    {
        boost::get<0>(t) == RelationEffects::HP ? effectStr += "血量增加" :
        (boost::get<0>(t) == RelationEffects::STRENGTH ? effectStr += "攻击增加" : effectStr += "恢复增加");
        
        effectStr += (toStr(int(boost::get<1>(t)*100)) + "%\n");
    }
    
    effectLabel = CCLabelTTF::create(effectStr.c_str(),
                                                 FONT_CN,
                                                 24,
                                                 CCSize(MaxTextWidth, 0),
                                                 kCCTextAlignmentCenter);
    effectLabel->setPosition(ccp(0,-83));
    addChild(effectLabel);
    
    return true;
}
    
void RelationBoard::setOpacity(GLubyte value)
{
    CCSprite::setOpacity(value);
    bg->setOpacity(value);
    mask->setOpacity(value);
    heroLabel->setOpacity(value);
    effectLabel->setOpacity(value);
    name->setOpacity(value);
    lockSigh->setOpacity(value);
    if(sigh)
        sigh->setOpacity(value);
}
    
void RelationBoard::setColor(const ccColor3B& value)
{
    CCSprite::setColor(value);
    bg->setColor(value);
    mask->setColor(value);
    heroLabel->setColor(value);
    effectLabel->setColor(value);
    name->setColor(value);
    lockSigh->setColor(value);
    if(sigh)
        sigh->setColor(value);
}
    
void RelationBoard::lock(bool value)
{
    mask->setVisible(value);
}
    
void RelationBoard::occupied(bool value)
{
    if(value && !sigh)
    {
        sigh = GetSprite("gameui/yuanfen_yiyou.png");
        sigh->setPosition(ccp(-100,size.height*0.5-45));
        addChild(sigh);
    }
    else if(!value && sigh)
    {
        sigh->removeFromParent();
        sigh = NULL;
    }
}
    
    
    
bool WashPoolLayer::init(PH::Hero *hero)
{
    if(!ContentLayer::init())
        return false;
    
    isShowBottom = false;
    target = hero;
    lockMap = std::vector<bool>(3 , false);
    priceLabel = pillsLabel = NULL;
    price = NULL;
    isPayByGold = true;
    
    CCSprite* bg = GetSprite("gameui/10lian_bg.png");
    bg->setAnchorPoint(ccp(0.5,1));
    bg->setPositionY(160);
    addChild(bg);
    
    CCMenu *menu = CCMenu::create();
    
	CCMenuItem *backItem = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"), GetSprite("gameui/back_t.png"),
                                                    this , menu_selector(WashPoolLayer::onBack));
	CCMenuItem *titleItem = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"), GetSprite("gameui/hongchengdi_t_tongyong.png"));
    LabelX* name = LabelX::create("缘分", FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp( 141 , 34));
    titleItem->addChild(name);
    
	menu->addChild(backItem);
	menu->addChild(titleItem);
	
    backItem->setAnchorPoint(ccp(0.5 , 1));
    titleItem->setAnchorPoint(ccp(0.5 , 1));
	menu->alignItemsHorizontallyWithPadding(8);
	menu->setPosition(ccp(-70 , 18));
	addChild(menu,10);
    
    {
        CCSprite* guo = GetSprite("gameui/guo.png");
        guo->setPositionY(-480);
        addChild(guo);
        
        CCSprite* light_back = GetSprite("gameui/yuanfen_guang_al.png");
        light_back->setPositionY(-355);
        addChild(light_back,100);
        
        light_back->runAction(CCRepeatForever::create(CCSequence::create(CCFadeTo::create(1, 100),
                                                                         CCFadeTo::create(1, 255),
                                                                         NULL)
                                                      )
                              );
        
        boil = CCParticleSystemQuad::create("particle/boil_water.plist");
        boil->setPosition(ccp(0 , -380));
        boil->unscheduleUpdate();
        this->addChild(boil);
        
        light_front = GetSprite("gameui/yuanfen_guang_bf.png");
        light_front->setAnchorPoint(ccp(0.5,0));
        light_front->setPositionY(-430);
        light_front->setVisible(false);
        addChild(light_front,101);
    }

    
    {
        showPrice();
        
        confirmBtn = createBtn("洗缘", ButtonType::NORMAL_RED);
        confirmBtn->setPositionY(-540);
        addChild(confirmBtn);
        
    }
    
    juanzhou = RelationJuanZhou::create(hero);
    juanzhou->attachLayer = this;
	CCSprite *zhou1 = GetSprite("gameui/suosou_1_1.png");
	CCSprite *zhou2 = GetSprite("gameui/suosou_1_1.png");
    zhou2->setFlipX(true);
	jz = JuanZhouLayer::create();
	jz->addSkin(juanzhou , zhou1 , zhou2);
	jz->setPositionY(-560);
	this->addChild(jz);
    
    return true;
}
    
void WashPoolLayer::showPrice()
{
    if(PlayerData::getInstance()->freeRelationTimes>0)
    {
        price = CCSprite::create();
        price->setPositionY(-460);
        addChild(price);
        addLabelBMFont(price, (toStr(PlayerData::getInstance()->freeRelationTimes)+"次免費").c_str(), "bmfont/Sumo_38_orange.fnt", ccp(0, 0))->setScale(contentScale());
        priceLabel = NULL;
    }
    else
    {
        price = GetSprite("gameui/jiage_chendi.png");
        price->setPositionY(-460);
        addChild(price, 100);
        
        
        CCSprite* gasBg = GetSprite("gameui/xuanzhong.png");
        CCSprite* gas = GetSprite("gameui/yuanfensan.png");
        gas->setPosition(ccp(gasBg->getContentSize().width*0.5, gasBg->getContentSize().height*0.5));
        gasBg->addChild(gas);
        CCSprite* dot = GetSprite("gameui/shezhi_xuanzhong.png");
        dot->setPosition(ccp(-21, gasBg->getContentSize().height*0.5));
        gasBg->addChild(dot);
        relPillBtn = ButtonSprite::create(gasBg, GetSprite("gameui/yuanfensan.png"),
                                          kCCMenuHandlerPriority, false);
        relPillBtn->setOnClickListener([=](){
            bool isGoldEnough = PlayerData::getInstance()->player->golds >= calPrice();
            bool isPillEnough = PlayerData::getInstance()->getNumOfHero(HeroIdOfType::Hero_RelPill) >= calPrice()/50;
            if(!isPillEnough)
            {
                if(isGoldEnough)
                    SystemInform::alertMessage(runningScene(), "洗缘散数量不足，请使用元宝重置", "提示", [=](){choosePayWay(true);});
                else
                    SystemInform::alertConfirmPanel(runningScene(), "你的道具和元宝不足, 是否充值？", "提示",
                                                    [](){GameScene::getInstance()->pushLayerWithDefaultAnimation(ChongZhiLayer::create());},
                                                    [](){});
                return;
            }
            choosePayWay(false);
        });
        relPillBtn->setPosition(ccp(84, 45));
        price->addChild(relPillBtn);
        pillsLabel = addLabelBMFont(relPillBtn, "", "bmfont/Sumo_22_yellow.fnt", ccp(70, 15), ccp(1, 0.5));
        pillsLabel->setScale(contentScale());
        
        CCSprite* goldBg = GetSprite("gameui/xuanzhong.png");
        CCSprite* gold = GetSprite("gameui/yuanbao_dan.png");
        gold->setPosition(ccp(goldBg->getContentSize().width*0.5f, goldBg->getContentSize().height*0.5f));
        goldBg->addChild(gold);
        CCSprite* dot2 = GetSprite("gameui/shezhi_xuanzhong.png");
        dot2->setPosition(ccp(-19, gasBg->getContentSize().height*0.5));
        goldBg->addChild(dot2);
        goldBtn = ButtonSprite::create(goldBg, GetSprite("gameui/yuanbao_dan.png"),
                                       kCCMenuHandlerPriority, false);
        goldBtn->setOnClickListener([=](){
            bool isGoldEnough = PlayerData::getInstance()->player->golds >= calPrice();
            bool isPillEnough = PlayerData::getInstance()->getNumOfHero(HeroIdOfType::Hero_RelPill) >= calPrice()/50;
            if(!isGoldEnough)
            {
                if(isPillEnough)
                    SystemInform::alertMessage(runningScene(), "元宝数量不足，请使用洗缘散重置", "提示", [=](){choosePayWay(false);});
                else
                    SystemInform::alertConfirmPanel(runningScene(), "你的道具和元宝不足, 是否充值？", "提示",
                                                    [](){GameScene::getInstance()->pushLayerWithDefaultAnimation(ChongZhiLayer::create());},
                                                    [](){});
                return;
            }
            choosePayWay(true);
        });
        goldBtn->setPosition(ccp(204, 45));
        price->addChild(goldBtn);
        priceLabel = addLabelBMFont(goldBtn, "", "bmfont/Sumo_22_yellow.fnt", ccp(50, 15), ccp(0.5, 0.5));
        priceLabel->setScale(contentScale());
        updatePrice();
        choosePayWay(false);
    }
    
    
}
    
void WashPoolLayer::choosePayWay(bool isGold)
{
    isPayByGold = isGold;
    if(isGold)
    {
        goldBtn->onRelease();
        relPillBtn->onPressed();
        
    }
    else
    {
        goldBtn->onPressed();
        relPillBtn->onRelease();
    }
    
}
    
void WashPoolLayer::updatePrice()
{
    if(priceLabel)
    {
        priceLabel->setString(toStr(calPrice()).c_str());
        int total = PlayerData::getInstance()->getNumOfHero(HeroIdOfType::Hero_RelPill);
        pillsLabel->setString((toStr(total)+"/"+toStr(calPrice()/50)).c_str());
        if(total >= calPrice()/50)
            pillsLabel->setFntFile("bmfont/Sumo_22_yellow.fnt");
        else
            pillsLabel->setFntFile("bmfont/Sumo_22_red.fnt");
    }
}
    
void WashPoolLayer::onEnterAnimationDone()
{
    jz->startAnim();
    boil->scheduleUpdate();
}
    
void WashPoolLayer::onSingleClick(CCTouch* touch)
{
    CCPoint point = convertTouchToNodeSpace(touch);
    
    if(confirmBtn->boundingBox().containsPoint(point))
        onConfirm();
}
    
void WashPoolLayer::locateBoard(HeroRelation& r)
{
    for(int i = 0; i < boards.size(); i++)
        if(r == ((RelationBoard*)boards[i])->relation)
            scrollView->moveTo(i);
}
    
void WashPoolLayer::lock(int i)
{
    assert(i < target->relations.size() && i >= 0);
    if(lockNum() >= target->relations.size()-1 && !lockMap[i])
        return;
        
    lockMap[i] = !lockMap[i];
    
    juanzhou->lock(i, lockMap[i]);
    lockRelationBoard(target->relations[i], lockMap[i]);
    updatePrice();
}
    
void WashPoolLayer::lockRelationBoard(HeroRelation& r,bool value)
{
    for(CCNode* node : boards)
        if(((RelationBoard*)node)->relation == r)
            ((RelationBoard*)node)->lock(value);
}
    
void WashPoolLayer::showOwnedRelations()
{
    std::vector<HeroRelation> relations = target->relations;
    for(CCNode* node : boards)
    {
        std::vector<HeroRelation>::iterator it = find(relations.begin(),
                                                      relations.end(),
                                                      ((RelationBoard*)node)->relation);
        ((RelationBoard*)node)->occupied(it != relations.end());
    }
}
    
int WashPoolLayer::calPrice()
{
    int p = 50;
    int num = lockNum();
    p += num==0?0:(num==1?50:150);
    return p;
    
}
    
int WashPoolLayer::lockNum()
{
    int num = 0;
    for(bool value : lockMap)
        if(value)num++;
    return num;
}
    
void WashPoolLayer::makeRelations(std::vector<HeroRelation>& relations)
{
    if(scrollView)
    {
        removeChild(scrollView,true);
        boards.clear();
        lockMap[0] = lockMap[1] = lockMap[2]= false;
    }
    
    for(int i = 0; i < relations.size(); i++)
    {
        boards.push_back(RelationBoard::create(relations[i]));
    }
    

    scrollView = RoundPageScrollView::create(boards,220);
    scrollView->setPositionY(-40);
    scrollView->setVisible(false);
    scrollView->runAction(CCSequence::create(CCShow::create(), CCFadeIn::create(1), NULL));
    addChild(scrollView);
    
    showOwnedRelations();
}
    
void WashPoolLayer::washAnima()
{
    juanzhou->setTouchEnabled(false);
    setTouchEnabled(false);
    scrollView->close();
    boil->setSpeed(100);
    boil->setGravity(ccp(0,80));
    price->removeFromParentAndCleanup(true);
    priceLabel = NULL;
    confirmBtn->setVisible(false);
    
    scrollView->runAction(CCSequence::create(CCMoveBy::create(0.5, ccp(0,-280)),
                                             CCDelayTime::create(0.5),
                                             createLightAction(),
                                             createRItemOutAction(),
                                             CCCallLambda::create([=]()
                                             {
                                                 scrollView->open();
                                                 for(CCNode* node : boards)
                                                     ((RelationBoard*)node)->lock(false);
                                                 juanzhou->listRelations();
                                                 boil->setSpeed(10);
                                                 boil->setGravity(ccp(0,30));
                                             }),
                                             CCMoveBy::create(0.5, ccp(0,280)),
                                             CCCallLambda::create([=](){
        this->setTouchEnabled(true);
        juanzhou->setTouchEnabled(true);
        confirmBtn->setVisible(true);
        this->showOwnedRelations();
        showPrice();
    }),
                                             NULL));
}

CCAction* WashPoolLayer::createLightAction()
{
    return CCTargetedAction::create(light_front,
                                    CCSequence::create(CCShow::create(),
                                                       CCFadeIn::create(0.6),
                                                       CCFadeOut::create(0.4),
                                                       CCHide::create(),
                                                       NULL));
}

CCAction* WashPoolLayer::createRItemOutAction() 
{
    CCArray* actions = CCArray::create();
    for(int i = 0; i < target->relations.size(); i++)
    {
        HeroRelation r = target->relations[i];
        for(CCNode* b : boards)
            if(((RelationBoard*)b)->relation == r)
            {
                RelationItem* board = RelationItem::create(r);
                board->setPositionY(-400);
                board->setScale(0.2);
                board->setVisible(false);
                addChild(board);
                
                actions->addObject(
                            CCTargetedAction::create(board,
                                             CCSequence::create(CCDelayTime::create(0.3*i),
                                                                CCShow::create(),
                                                                CCSpawn::create(
                                                                        CCFadeIn::create(0.5),
                                                                        CCMoveTo::create(0.5, ccp(-160+160*i,-200)),
                                                                        CCScaleTo::create(0.5, 1),
                                                                        NULL),
                                                                CCDelayTime::create(0.6),
                                                                CCMoveTo::create(0.3, getItemPos(i)),
                                                                CCCallLambda::create([=](){this->removeChild(board,true);}),
                                                                NULL)));
            }
    }
    
    return CCSpawn::create(actions);
}
    
CCPoint WashPoolLayer::getItemPos(int i)
{
    return this->convertToNodeSpace(juanzhou->convertToWorldSpace(juanzhou->itemPos[i])); 
}
    
void WashPoolLayer::onConfirm()
{
    auto toConfirm = [=](){
        SystemInform* inform = SystemInform::alertLoading(GameScene::getInstance());
        
        std::vector<std::string> lockRe;
        for(int i = 0; i < lockMap.size(); i++)
            if(lockMap[i])
                lockRe.push_back(target->relations[i].id);
        
        int prevGold = PlayerData::getInstance()->getPlayer()->golds;
        RelationChangeType type =PlayerData::getInstance()->freeRelationTimes>0 ? RelationChangeType::Free :
                                    (isPayByGold ? RelationChangeType::Golds : RelationChangeType::Potions);
        
        PuzzleRPC::getInstance()->changeRelations(PlayerData::getInstance()->getToken(),
                                                  target->id,
                                                  lockRe,
                                                  type,
                                                  [=](std::auto_ptr<Hero> hero,
                                                      std::auto_ptr<Error> e)
        {
            inform->cancel();
            if(e.get())
            {
                e->handleErrorInform();
                return;
            }
            
            PlayerData* data = PlayerData::getInstance();
            data->updateHero(hero.get());
            if(data->freeRelationTimes>0)
                data->freeRelationTimes--;

            int golds = data->getPlayer()->golds;
            if(type == RelationChangeType::Golds)
            {
                data->setGolds(golds-calPrice());
            }
            else if(type == RelationChangeType::Potions)
            {
                data->removeNumOfHero(HeroIdOfType::Hero_RelPill, calPrice()/50);
            }
          
            lockMap = std::vector<bool>(3 , false);
                
            washAnima();
        });

    };
    bool isGoldEnough = PlayerData::getInstance()->player->golds >= calPrice();
    bool isPillEnough = PlayerData::getInstance()->getNumOfHero(HeroIdOfType::Hero_RelPill) >= calPrice()/50;
    if(isPayByGold && !isGoldEnough)
    {
        SystemInform::alertMessage(runningScene(), "元宝数量不足！", "提示", [=](){choosePayWay(false);});
        return;
    }
    if(!isPayByGold && !isPillEnough)
    {
        SystemInform::alertMessage(runningScene(), "洗缘散数量不足！", "提示", [=](){choosePayWay(false);});
        return;
    }
    SystemInform::alertConfirmPanel(GameScene::getInstance(),
                                    "确定要洗缘分吗？",
                                    "提示",
                                    toConfirm,
                                    [](){});
}

void WashPoolLayer::onBack()
{
    GameScene::getInstance()->popLayer();
}
    
    
bool WashRelHeroBoard::init()
{
    displayPlaceholder = false;
    if(!HeroBoard::init(PlayerData::getInstance()->getStorage(), false))
        return false;
    
    return true;
}

void WashRelHeroBoard::refresh(const std::vector<PH::Hero *>& heroVec)
{
    std::vector<PH::Hero*> heroesRemoveMats;
    for(Hero* h : heroVec)
        if(h->profile->tableId >= 10000)
            heroesRemoveMats.push_back(h);
    
    HeroBoard::refresh(heroesRemoveMats);
    
    for (auto h : heroesRemoveMats)
    {
        HeroIcon *icon = getHeroImg(h);
        icon->setEnabled(!(h->profile->isMat() || h->profile->star < 3));
    }
}
    
void WashRelHeroBoard::reOrder(const std::vector<PH::Hero *>& heroVec)
{
    std::vector<PH::Hero*> heroesRemoveMats;
    for(Hero* h : heroVec)
        if(h->profile->tableId >= 10000)
            heroesRemoveMats.push_back(h);
    
    HeroBoard::reOrder(heroesRemoveMats);
}
    

bool WashRelationLayer::init()
{
    if(!ContentLayer::init())
        return false;
    
    CCMenu *menu = CCMenu::create();
    
	CCMenuItem *backItem = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"), GetSprite("gameui/back_t.png"),
                                                    this , menu_selector(WashRelationLayer::onBack));
    backItem->setAnchorPoint(ccp(0.5 , 1));
    menu->addChild(backItem);
    
    {
        CCMenuItem* sortButton = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                          GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                          this,
                                                          menu_selector(WashRelationLayer::onTitleClick));
        sortButton->setAnchorPoint(ccp(0.5 , 1));
        
        LabelX* name = LabelX::create("缘分", FONT_CN, 36);
        name->setColor(ccWHITE);
        name->setAnchorPoint(ccp(0.5, 0.5));
        name->setPosition(ccp(90 , sortButton->getContentSize().height*0.5f+2));
        sortButton->addChild(name);
        
        title = LabelX::create(PlayerData::getInstance()->getSortTypeMsg(), FONT_CN, 20, LabelX::FontEffect::SHADOW, ccp(2, -2));
        title->setColor(ccWHITE);
        title->setPosition(ccp( 170 , sortButton->getContentSize().height*0.5f));
        title->setAnchorPoint(ccp(0, 0.5));
        sortButton->addChild(title);
        
        CCSprite *xian = GetSprite("gameui/hongchengdi_huang_tongyong.png");
        xian->setPosition(ccp(160 , sortButton->getContentSize().height*0.5f));
        sortButton->addChild(xian);
        menu->addChild(sortButton);
    }
	
	menu->alignItemsHorizontallyWithPadding(8);
	menu->setPosition(ccp(-70 , 18));
	addChild(menu,10);
    
    
    scrollView = ZqlScrollView::create();
	scrollView->setViewSize(CCSize(winSize.width, 900),
                            CCRect(0, -110, winSize.width, 610));
    
    
    
	board = WashRelHeroBoard::create();
	scrollView->addLayer(board);
	scrollView->setPositionY(70);
	this->addChild(scrollView);
    
    
    if(GameConfig::shared()->getTutorialProgress() == TutorialShuffleRelation)
    {
        AlertMessage::create("", "缘分重置：选择你想要重置缘分的侠客！花费元宝可以随机重置一次哦！\t只有三星或者以上的侠客才可以重置缘分。");
        GameConfig::shared()->setTutorialProgress(TutorialSkip);
    }
    return true;
}
    
void WashRelationLayer::onTitleClick()
{
    PlayerData::getInstance()->switchSortType();
	board->reOrder(PlayerData::getInstance()->getStorage());
	title->setString(PlayerData::getInstance()->getSortTypeMsg());
    
    ring("sound/v/button_click.mp3");
}

void WashRelationLayer::onEnter()
{
    ContentLayer::onEnter();
    
    board->refresh(PlayerData::getInstance()->getStorage());
}
    
void WashRelationLayer::onSingleClick(CCTouch* touch)
{
    CCPoint localPoint = scrollView->convertTouchToNodeSpace(touch);
    Hero* hero = NULL;
    if(scrollView->boundingBox().containsPoint(localPoint))
        hero = board->getHero(touch);
    
    if(hero && hero->profile->star >= 3)
    {
        SystemInform* inform = SystemInform::alertLoading(GameScene::getInstance());
        PuzzleRPC::getInstance()->getRelations(PlayerData::getInstance()->getToken(), hero->heroId,
       [=](std::vector<HeroRelation> relations, std::auto_ptr<Error> e){
           inform->cancel();
           if(e.get())
           {
               e->handleErrorInform();
               return;
           }
           if(relations.size() == 0 || relations.size() <= hero->relations.size())
           {
               SystemInform::alertMessage(GameScene::getInstance(),
                                          "该侠客无其他缘分",
                                          "提示",
                                          [](){});
               return;
           }
           WashPoolLayer* layer = WashPoolLayer::create(hero);
           layer->makeRelations(relations);
           GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
       });
    }
        
}

void WashRelationLayer::onLongClick(CCTouch* touch)
{
    CCPoint localPoint = scrollView->convertTouchToNodeSpace(touch);
    Hero* hero = NULL;
    if(scrollView->boundingBox().containsPoint(localPoint))
        hero = board->getHero(touch);
    
    if(hero)
        GameScene::getInstance()->pushLayerWithDefaultAnimation(HeroProfileLayer::create(hero));
}


void WashRelationLayer::onBack()
{
    GameScene::getInstance()->popLayer();
}


}