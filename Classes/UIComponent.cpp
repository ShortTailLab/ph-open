//
//  UIComponent.cpp
//  HelloCpp
//
//  Created by zql on 13-6-23.
//
//

#include "UIComponent.h"
#include "UIPresets.h"
#include "Common.h"
#include "PlayerData.h"
#include "PageScrollView.h"
#include "HeroProfileLayer.h"
#include "Fonts.h"
#include "CCCallLambda.h"
#include "BuildConf.h"

#if PH_USE_WEIBO
#include <auth/Weibo.h>
#endif

namespace PH
{
    using namespace std;
    using namespace boost::posix_time;
    USING_NS_CC;
    
LabelX* createTitleMenu(CCLayer* layer,
                        std::string titleMsg,
                        std::string sortMsg,
                        SEL_MenuHandler onBack,
                        SEL_MenuHandler onTitle,
                        bool isShowTop)
{
    LabelX* title = NULL;
    CCMenuItem* sortButton;
    CCMenu* menu = CCMenu::create();
    CCMenuItem *backItem = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"), GetSprite("gameui/back_t.png"),
                                                    layer , onBack);
    menu->addChild(backItem);

    if(onTitle)
        sortButton = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                          GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                          layer,
                                                          onTitle);
    else
        sortButton = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                          GetSprite("gameui/hongchengdi_t_tongyong.png"));
    sortButton->setAnchorPoint(ccp(0.5 , 1));
    
    CCPoint pos = ccp( 141 , 35);
    if(!sortMsg.empty())
    {
        pos = ccp(90 , sortButton->getContentSize().height*0.5f);
        title = LabelX::create(sortMsg.c_str(), FONT_CN , 20, LabelX::FontEffect::SHADOW, ccp(2, -2));
        title->setColor(ccWHITE);
        title->setPosition(ccp( 170 , sortButton->getContentSize().height*0.5f));
        title->setAnchorPoint(ccp(0, 0.5));
        sortButton->addChild(title);
        
        CCSprite *xian = GetSprite("gameui/hongchengdi_huang_tongyong.png");
        xian->setPosition(ccp(160 , sortButton->getContentSize().height*0.5f));
        sortButton->addChild(xian);
        
    }
    
    LabelX* name = LabelX::create(titleMsg.c_str(), FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(pos);
    sortButton->addChild(name);
    menu->addChild(sortButton);

    backItem->setAnchorPoint(ccp(0.5 , 1));
    menu->alignItemsHorizontallyWithPadding(8);
    int posY = isShowTop ? 18 : -20;
    menu->setPosition(ccp(-70 , posY));
    menu->setTag(10000);
    layer->addChild(menu, 10);
    return title ? title : name;
}
    
    std::vector<Hero*> electHeroes(const std::vector<Hero*>& heroes)
    {
        std::vector<Hero*> elected;
        for(Hero* h : heroes)
            if(h->profile->tableId >= 10000)
                elected.push_back(h);
        return elected;
    }
    std::vector<Hero*> electDanYaoFromHeroes(const std::vector<Hero*>& heroes)
    {
        std::vector<Hero*> elected;
        for(Hero* h : heroes)
            if(h->profile->type == "强化道具")
                elected.push_back(h);
        return elected;
    }
    std::vector<Hero*> electToolsFromHeroes(const std::vector<Hero*>& heroes)
    {
        std::vector<Hero*> elected;
        for(Hero* h : heroes)
            if(h->profile->type == "转生道具")
                elected.push_back(h);
        return elected;
    }
    
    std::vector<Hero*> getHeroesBySortAndSwitchType(int currIndex)
    {
        std::vector<Hero*> heroes =  PlayerData::getInstance()->player->storage;;
        if(currIndex == 1)
            heroes = electHeroes(heroes);
        else if(currIndex == 2)
            heroes = electDanYaoFromHeroes(heroes);
        else if(currIndex == 3)
            heroes = electToolsFromHeroes(heroes);
        
        return sortHeroesByStorageType(heroes, PlayerData::getInstance()->getSortType());
    }
    
    CCSprite* makeTips(int num)
    {
        CCSprite* tips = GetSprite("gameui/shuliang.png");
        std::string n = num < 10 ? toStr(num) : "n";
        
        CCLabelTTF* s;
        addLabelTTF(tips ,s, n.c_str() , FONT_NUM, 26, ccp(20, 20), ccp(0.5, 0.5) , ccc3(83, 0, 70));
        
        return tips;
    }
    
    void runCircleAction(CCNode* target, int radius)
    {
        
        float w = 3.1415926/12;
        float cosw = std::cos(w);
        float sinw = std::sin(w);
        CCPoint org = target->getPosition();
        target->setPosition(ccp(org.x, org.y-radius));
        target->runAction(CCRepeatForever::create(CCSequence::create(
                                                    CCDelayTime::create(0.1),
                                                    CCCallLambda::create([=]()
                 {
                     CCPoint curPos = target->getPosition()-org;
                     CCPoint tarPos = ccp(curPos.x*cosw-curPos.y*sinw,
                                          curPos.y*cosw+curPos.x*sinw);
                     
                     target->setPosition(tarPos+org);
                 }),
                                                    NULL)));
    }
    
    bool isSharedAvailable()
    {
#if PH_USE_WEIBO && (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
        auto& sina = Weibo<Sina>::instance();
        auto& tencent = Weibo<Tencent>::instance();
        return sina.isAuthorized() || tencent.isAuthorized();
#else
        return false;
#endif
    }
    
    std::vector<std::string> getMatSource(const Hero* hero)
    {
        vector<string> stageNames;
        for(const boost::shared_ptr<Stage>& stage : PlayerData::getInstance()->normalStageList)
            for(const Level& level : stage->levelList)
                if(std::find(level.dropsHint.begin(), level.dropsHint.end(), hero->heroId) != level.dropsHint.end() &&
                   std::find(stageNames.begin(), stageNames.end(), stage->name) == stageNames.end())
                stageNames.push_back(stage->name);
        for(const boost::shared_ptr<Stage>& stage : PlayerData::getInstance()->instanceStageList)
            for(const Level& level : stage->levelList)
                if(std::find(level.dropsHint.begin(), level.dropsHint.end(), hero->heroId) != level.dropsHint.end() &&
                   std::find(stageNames.begin(), stageNames.end(), stage->name) == stageNames.end())
                    stageNames.push_back(stage->name);
        
        return stageNames;
    }
    
    HeroIcon::LabelType getIconTypeOnSortType(int sortType)
    {
        HeroIcon::LabelType type = HeroIcon::LabelType::Level;
        if(sortType == 3)
            type = HeroIcon::LabelType::HP;
        else if(sortType == 4)
            type = HeroIcon::LabelType::Attack;
        else if(sortType == 5)
            type = HeroIcon::LabelType::Regen;
        return type;
    }
    
    template<> int easing<>(int curr, int target, float rate)
    {
        if(std::abs(target - curr) <= 1)
            return target;
        
        return curr+(target-curr)*rate;
    };
    
    
    CCPoint MatrixTrans::get2DPoint(float px, float py, float pz)
    {
        float x = viewPoint.x+px*getScale(pz);
        float y = viewPoint.y+py*getScale(pz);
        return CCPoint(x, y);
    }
    
    float MatrixTrans::getScale(float pz)
    {
        return distance/(distance+pz);
    }
    
bool JuanZhou::init()
{
    //if(!ContentLayer::init())
    if(!CCLayer::init())
        return false;
    
    heroNum = 5;
    return true;
}


bool JuanZhou::addHero(PH::Hero *hero, int id, bool isUpdate)
{
    HeroIcon *sprite = HeroIcon::create(hero);
    addHeroImg(sprite , id , isUpdate);
    return true;
}

void JuanZhou::addHeroImg(HeroIcon *heroImg, int id, bool isUpdate)
{
    if(heroImgVec.at(id))
        removeChild(heroImgVec.at(id), true);
    heroImg->showElite(true);
    heroImg->setAnchorPoint(ccp(0.5, 0.5));
    CCRect rect = locations[id];
    heroImg->setPosition(ccp(rect.origin.x + rect.size.width*0.5f,
                             rect.origin.y + rect.size.height*0.5f));
    addChild(heroImg);
    heroImgVec.at(id) = heroImg;
    heroVec.at(id) = heroImg->getHero();
    if(isUpdate)
        updateData();
}

void JuanZhou::removeHeroById(int id , bool isUpdate)
{
    removeChild( heroImgVec.at(id) , true);
    heroImgVec.at(id) = NULL;
    heroVec.at(id) = NULL;
    
    if(isUpdate)updateData();
}

void JuanZhou::removeHero(const Hero *hero , bool isUpdate)
{
    for(int i = 0 ; i < heroVec.size() ; i++){
        if(heroVec[i] && heroVec[i] == hero){
            removeHeroById(i , isUpdate);
        }
    }
}

void JuanZhou::clear()
{
    for(int i = 0 ; i < heroNum ; i++)removeHeroById(i , false);
}

int JuanZhou::getIdByTouch(cocos2d::CCTouch *touch)
{
    CCPoint point = convertTouchToNodeSpace(touch);
    
    return getIdByPoint(point);
}

int JuanZhou::getIdByPoint(CCPoint worldPoint)
{
    CCPoint localPoint = convertToNodeSpace(worldPoint);
    
    for(int i = 0; i < heroNum; i++)
    {
        if(locations[i].containsPoint(localPoint))
            return i;
    }
    return -1;
}


PH::Hero *JuanZhou::getHeroById(int id)
{
    return heroVec.at(id);
}

Hero *JuanZhou::getHeroByTouch(CCTouch *touch)
{
    int id = getIdByTouch(touch);
    if(id != -1)return getHeroById(id);
    else return NULL;
}

int JuanZhou::getHeroId(Hero *hero)
{
    for(int i = 0 ; i < heroVec.size() ; i++){
        if(heroVec[i] && heroVec[i]->id == hero->id)return i;
    }
    return -1;
}

HeroIcon *JuanZhou::getImgById(int id)
{
    return heroImgVec[id];
}

HeroIcon *JuanZhou::getImgByPoint(cocos2d::CCPoint worldPoint)
{
    int id = getIdByPoint(worldPoint);
    if(id != -1)
        return getImgById(id);
    else return NULL;
}

void JuanZhou::openDone()
{
    //called when juanzhou opened
}

void JuanZhou::visit()
{
    CCDirector *director = CCDirector::sharedDirector();
    
    float n_width = viewPort.width ;
    float n_height = viewPort.height ;
    
    CCPoint pos = getPosition();
    CCPoint pos2 = ccp(pos.x - n_width*0.5f , pos.y - n_height);
    
    // apply world transform
    CCPoint temp = getParent()->convertToWorldSpace(pos2);
    //CCPoint glPoint = director->convertToGL(temp);
    // apply scale factor
    
    float x = temp.x ;
    float y = temp.y ;
    
    glEnable(GL_SCISSOR_TEST);
    director->getOpenGLView()->setScissorInPoints(x, y, n_width, n_height);
    CCLayer::visit();
    glDisable(GL_SCISSOR_TEST);
}


void JuanZhou::updateData()
{
}

////////////////////////////////////////////////
//JuanZhouLayer
////////////////////////////////////////////////

bool JuanZhouLayer::init()
{
    if(!ContentLayer::init())
        return false;
    
    juanzhou = NULL;
    isOpen = false;
    
    this->scheduleUpdate();
    return true;
}

void JuanZhouLayer::addSkin(JuanZhou *juanzhou,
                            CCSprite *zhouLeft,
                            CCSprite *zhouRight)
{
    this->juanzhou = juanzhou;
    juanzhou->setPosition(ccp(0 ,-18));
    this->addChild(juanzhou);
    
    //float h = zhou1->getContentSize().height;
    
    zhou1 = zhouLeft;
    zhou1->setAnchorPoint(ccp(0 , 1));
    zhou1->setPosition(ccp(-40 , 0));
    addChild(zhou1);
    
    zhou2 = zhouRight;
    zhou2->setAnchorPoint(ccp(1 , 1));
    zhou2->setPosition(ccp(40 , 0));
    addChild(zhou2);
}


void JuanZhouLayer::update(float dt)
{
    if(isOpen)
    {
        easing(zhou1 , -320);
        if(easing(zhou2 , 320))
        {
            juanzhou->openDone();
            isOpen = false;
        }
        float w = zhou2->getPositionX();
        juanzhou->viewPort.width = 2*(w-10);
    }
}

void JuanZhouLayer::startAnim()
{
    isOpen = true;
    ring("sound/v/scroll_unfold.mp3");
}

////////////////////////////////////////////////
//Board
////////////////////////////////////////////////

HeroBoard::HeroBoard()
:displayPlaceholder(true), mClipLower(0), mClipUpper(760), mLastPos(0), mKidsCount(0)
{
    viewPort = CCRectMake(0, 0, 640, 960);
    this->scheduleUpdate();
}

HeroBoard::~HeroBoard()
{
    clear();
}

void HeroBoard::update(float dt)
{
    if(this->getParent() == NULL)
        return;
    
    int size = heroImgVec.size() + placeHolder.size();
    for(int i = 0; i < size; i++)
    {
        CCSprite* node = i < heroImgVec.size() ? heroImgVec[i] : placeHolder[i-heroImgVec.size()];
        CCPoint worldPoint = convertToWorldSpace(node->getPosition());
        float height = node->getContentSize().height;
        node->setVisible(worldPoint.y+height*0.5f <= viewPort.getMaxY() &&
                         worldPoint.y-height*0.5f >= viewPort.getMinY());
    }
}

void HeroBoard::setDisplayStoragePlaceholder(bool shouldDisplay)
{
    if (displayPlaceholder != shouldDisplay)
    {
        displayPlaceholder = shouldDisplay;
        refresh();
    }
}

bool HeroBoard::init(const std::vector<PH::Hero *>& heroVec, bool isPlaceHolder)
{
    if(!ContentLayer::init())
        return false;
    displayPlaceholder = isPlaceHolder;
    refresh(heroVec);
    
    return true;
}

void HeroBoard::clear( void )
{
    for(int i=0; i<this->heroImgVec.size(); i++)
    {
        if(NULL!=this->heroImgVec[i]){
            this->heroImgVec[i]->removeFromParentAndCleanup(true);
        }
    }
    for(int i=0; i<this->placeHolder.size(); i++)
    {
        if(NULL!=this->placeHolder[i]){
            this->placeHolder[i]->removeFromParentAndCleanup(true);
        }
    }
    
    this->placeHolder.clear();
    this->heroImgVec.clear();
}

void HeroBoard::refresh(const std::vector<PH::Hero *>& heroVec)
{
    this->clear();
    this->heroVec = heroVec;
    
    capacity = displayPlaceholder ? PlayerData::getInstance()->player->storageLimit : heroVec.size();
    refresh();
}

// refresh base on heroboard's own state
void HeroBoard::refresh()
{
    // compromise for GalleryBoard
    for(int i = 0 ; i < heroVec.size() ; i++)
    {
        HeroIcon *icon = HeroIcon::create(heroVec[i] , true , true);
        icon->defaultBottmMsg();
        heroImgVec.push_back(icon);
        addHero(icon, i);
    }
    //refreshBottomMsgOnIcons();
    if(!displayPlaceholder)
        return;
    
    for(int i= heroVec.size(); i<capacity; i++)
    {
        CCSprite *s = GetSprite("gameui/zhanwei_biankuang.png");
        placeHolder.push_back(s);
        addHero(s, i);
    }
}

void HeroBoard::reOrder(const std::vector<PH::Hero *>& heroes)
{
    assert(heroes.size() == heroImgVec.size());
    for(int i = 0; i<heroes.size(); i++)
        for(int j =0; j<heroImgVec.size(); j++)
            if(heroes[i] == heroImgVec[j]->getHero())
            {
                if(i != j)
                {
                    HeroIcon* icon = heroImgVec[i];
                    heroImgVec[i] = heroImgVec[j];
                    heroImgVec[j] = icon;
                }
                heroImgVec[i]->setPosition(getPos(i));
                break;
            }
    this->heroVec = heroes;
    refreshBottomMsgOnIcons();
}
    
void HeroBoard::refreshBottomMsgOnIcons()
{
    for(HeroIcon* icon : heroImgVec)
    {
        if(icon->getLabelType() == HeroIcon::LabelType::Level ||
           icon->getLabelType() == HeroIcon::LabelType::HP ||
           icon->getLabelType() == HeroIcon::LabelType::Attack ||
           icon->getLabelType() == HeroIcon::LabelType::Regen ||
           icon->getLabelType() == HeroIcon::LabelType::Type ||
           icon->getLabelType() == HeroIcon::LabelType::Fight)
            icon->defaultBottmMsg();
    }
}

void HeroBoard::initTeam()
{
    for(Hero* h : PlayerData::getInstance()->getTeam())
    {
        if(h != NULL)
            ((HeroIcon*)getHeroImg(h))->setBottomMsg(HeroIcon::LabelType::InTeam);;
    }
}

PH::Hero* HeroBoard::getHero(CCTouch *touch)
{
    CCPoint localPoint = convertTouchToNodeSpace(touch);
    Hero *hero = NULL;
    for(HeroIcon *icon : heroImgVec)
    {
        CCSize size = icon->getContentSize();
        CCRect rect = CCRect(icon->getPositionX()-size.width*0.5f , icon->getPositionY()-size.height*0.5f , size.width , size.height);
        if(rect.containsPoint(localPoint))
        {
            hero = icon->getHero();
        }
    }
    return hero;
}

HeroIcon* HeroBoard::getHeroImg(const PH::Hero *hero)
{
    for(int i = 0 ; i < heroVec.size() ; i++)
    {
        if(hero->id == heroVec[i]->id)
            return heroImgVec[i];
    }
    return NULL;
}

//return NULL if no icon in the certain point
HeroIcon *HeroBoard::getHeroImgByPoint(CCPoint worldPoint)
{
    CCPoint localPoint = convertToNodeSpace(worldPoint);
    for(HeroIcon *icon : heroImgVec)
    {
        CCRect bound = icon->boundingBox();
        
        if(bound.containsPoint(localPoint))
            return icon;
    }
    return NULL;
}

    CCPoint HeroBoard::getPos(int num)
    {
        int row = num % 6;
        int cal = (int)num / 6;
        //    int wInterval = 120
        //    int hInterval = 120;
        //    int row = num % 5;
        //    int cal = num / 5;
        float px = -264 + row * skHeroIconWidth;
        float py = -60 - cal * skHeroIconHeight;
        
        return ccp(px, py);
    }
    
void HeroBoard::addHero(CCSprite * heroImg , int num, bool isUpdateSize)
{
    CCSize size = heroImg->getContentSize();
    CCPoint pos = getPos(num);
    heroImg->setPosition(pos);
    this->addChild(heroImg);
    
    int height = abs(pos.y - 60);
    CCSize contentSize = getContentSize();
    if(height > contentSize.height)setContentSize(CCSize(contentSize.width , height));
    
    if(isUpdateSize)
        setContentSize(CCSize( 630 , abs(pos.y - 60)));
}

CCPoint HeroBoard::getPosByNum(int num)
{
    int row = num % 6;
    int cal = (int)num / 6;
    //    int wInterval = 120;
    //    int hInterval = 120;
    //    int row = num % 5;
    //    int cal = num / 5;
    float px = -264 + row * skHeroIconWidth;
    float py = -60 - cal * skHeroIconHeight;
    return ccp(px, py);
}

void HeroBoard::putBackHero(Hero *hero)
{
    HeroIcon *icon = getHeroImg(hero);
    if(icon)
        icon->setEnabled(true);
}
    
bool ScrollRelations::init(const std::vector<HeroRelation>& relations, const CCRect& viewPort)
{
    if(!ContentLayer::init())
        return false;
    
    this->relations = relations;
    
    for(const HeroRelation& r : relations)
        relationItems.push_back(HeroRelationItem::create(r));
    
    relationBoard = GetSprite("gameui/yuanfen_lv_neirong_chang.png");
    relationBoard->setPositionY(180);
    relationBoard->setVisible(false);
    addChild(relationBoard, 10);
    
    addLabelTTF(relationBoard, rName, "", FONT_CN, 36, ccp(45 , 195) , ccp(0 , 0.5) );
    addLabelTTF(relationBoard, rHeroes, "", FONT_CN, 28, ccp(45 , 120), ccp(0 , 0.5));
    addLabelTTF(relationBoard, rEffect, "", FONT_CN, 26, ccp(45 , 50), ccp(0 , 0.5));
    
    scrollView = PageScrollView::create(relationItems, CCSizeMake(160, 60) , false);
    scrollView->setCurrentNode(0);
    scrollView->setViewPort(viewPort);
    scrollView->setPosition(ccp(-83, 0));
    addChild(scrollView);
    
    this->setTouchEnabled(true);
    
    return true;
}

void ScrollRelations::updateRelationBoard(const HeroRelation* r)
{
    if(r)
    {
        relationBoard->setVisible(true);
        rName->setString((r->name+"("+HPT(r->withHeroes[0]).nameCN+")").c_str());
        std::string heroStr;
        for(int id : r->withHeroes)
            heroStr += (HPT(id).nameCN + " ");
        rHeroes->setString(heroStr.c_str());
        
        std::string effectStr;
        for(boost::tuple<RelationEffects, double> t : r->effects)
        {
            boost::get<0>(t) == RelationEffects::HP ? effectStr += "血量增加" :
            (boost::get<0>(t) == RelationEffects::STRENGTH ? effectStr += "攻击力增加" : effectStr += "恢复力增加");
            
            effectStr += (toStr(int(boost::get<1>(t)*100)) + "% ");
        }
        rEffect->setString(effectStr.c_str());
    }
    else
    {
        relationBoard->setVisible(false);
        rName->setString(" ");
        rHeroes->setString(" ");
        rEffect->setString(" ");
    }
}

void ScrollRelations::setTouchEnabled(bool value)
{
    ContentLayer::setTouchEnabled(value);
    scrollView->setTouchEnabled(value);
}

void ScrollRelations::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    ContentLayer::ccTouchesBegan(pTouches, pEvent);
    
    CCTouch *touch = (CCTouch*)pTouches->anyObject();
    CCPoint localPoint;
    
    for (int i = 0 ; i <  relationItems.size() ; i++)
    {
        localPoint = relationItems[i]->getParent()->convertTouchToNodeSpace(touch);
        if(relationItems[i]->boundingBox().containsPoint(localPoint))
        {
            tempRItem = relationItems[i];
            ((HeroRelationItem*)tempRItem)->onPress(true);
            updateRelationBoard(&((HeroRelationItem*)tempRItem)->relation);
        }
    }
    
}
void  ScrollRelations::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    ContentLayer::ccTouchesMoved(pTouches, pEvent);
    
    CCTouch *touch = (CCTouch*)pTouches->anyObject();
    CCPoint localPoint;
    
    if (tempRItem) {
        localPoint = tempRItem->getParent()->convertTouchToNodeSpace(touch);
        if(!tempRItem->boundingBox().containsPoint(localPoint)) {
            ((HeroRelationItem*)tempRItem)->onPress(false);
            tempRItem = NULL;
            updateRelationBoard(NULL);
        }
    }
    
}
void ScrollRelations::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    ContentLayer::ccTouchesEnded(pTouches, pEvent);
    
    if (tempRItem) {
        ((HeroRelationItem*)tempRItem)->onPress(false);
        tempRItem = NULL;
        updateRelationBoard(NULL);
    }
}

    
///////////////////////////////////////////////
//Switcher
///////////////////////////////////////////////
bool Switcher::init(const std::vector<std::string>& paths)
{
    if(!ContentLayer::init())
        return false;
    
    callBack = [](int i){};
    container = ContentLayer::create();
    addChild(container);
    
    int posX = 0;
    int height = 0;
    for(int i = 0; i < paths.size(); i += 2)
    {
        CCSprite* btn = GetSprite(paths[i]);
        btn->setAnchorPoint(ccp(0, 0));
        btn->setPositionX(posX);
        
        container->addChild(btn);
        
        CCSprite* mask = GetSprite(paths[i+1]);
        mask->setAnchorPoint(ccp(0, 0));
        mask->setPositionX(posX);
        btns.push_back(mask);
        container->addChild(mask);
        
        posX += btn->getContentSize().width;
        height = btn->getContentSize().height;
    }
    contentSize = CCSizeMake(posX, height);
    setCurNode(0);
    setTouchMode(kCCTouchesOneByOne);
    return true;
}

void Switcher::setAnchorPoint(CCPoint anchor)
{
    container->setPosition(ccp(-contentSize.width*anchor.x, -contentSize.height*anchor.y));
}

bool Switcher::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    ContentLayer::ccTouchBegan(pTouch, pEvent);
    
    CCPoint localPos = container->convertTouchToNodeSpace(pTouch);
    
    for(int i=0;i<btns.size();i++)
        if(btns[i]->boundingBox().containsPoint(localPos))
            return true;
    return false;
}
    
    
void Switcher::setCurNode(int i)
{
    curNode = i;
    for(int j = 0; j < btns.size(); j++)
        btns[j]->setVisible(j==curNode);
}
int Switcher::getCurNode()
{
    return curNode;
}

void Switcher::onSingleClick(CCTouch* touch)
{
    CCPoint localPos = container->convertTouchToNodeSpace(touch);
    
    for(int i=0;i<btns.size();i++)
    {
        if(curNode != i && btns[i]->boundingBox().containsPoint(localPos))
        {
            setCurNode(i);
            callBack(i);
        }
        
    }
}

void Switcher::setCallBack(boost::function<void (int)> _callBack)
{
    callBack = _callBack;
}

    
bool ScissorLayer::init()
{
    if(!ContentLayer::init())
        return false;
    viewPort = winSize;
    
    return true;
}

//make the visible region
void ScissorLayer::visit()
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

///////////////////////////////////////////////
//BianDuiSwitcher
///////////////////////////////////////////////

bool BianDuiSwitcher::init(bool isBlack)
{
    std::vector<std::string> paths;
    for(int i = 0; i < 5; i++)
    {
        if(!isBlack)
        {
            paths.push_back("gameui/duiwu"+toStr(i+1)+"_a.png");
            paths.push_back("gameui/duiwu"+toStr(i+1)+"_b.png");
        }
        else
        {
            paths.push_back("gameui/duiwu"+toStr(i+1)+"_c.png");
            paths.push_back("gameui/duiwu"+toStr(i+1)+"_c.png");
        }
        
    }
    if(!Switcher::init(paths))
        return false;
    setAnchorPoint(ccp(0, 0.5));
    setCurNode(PlayerData::getInstance()->getActiveTeam());
    
    this->setTouchEnabled(!isBlack);
    
    return true;
}
    
///////////////////////////////////////////////
//HeroBoardSwitcher
///////////////////////////////////////////////

bool HeroBoardSwitcher::init()
{
    std::vector<std::string> paths;
    for(int i = 0; i < 4; i++)
    {
        paths.push_back("gameui/dizifenlei"+toStr(i+1)+"_a.png");
        paths.push_back("gameui/dizifenlei"+toStr(i+1)+"_b.png");
    }
    if(!Switcher::init(paths))
        return false;
    setAnchorPoint(ccp(0.5, 0.5));
    setCurNode(0);
    
    this->setTouchEnabled(true);
    
    return true;
}
///////////////////////////////////////////////
//MailSwitcher
///////////////////////////////////////////////
bool MailSwitcher::init()
{
    std::string btnPaths[] = {
        "gameui/shoujianxiang_a.png",
        "gameui/shoujianxiang_b.png",
        "gameui/fajianxiang_a.png",
        "gameui/fajianxiang_b.png"
    };
    std::vector<std::string> paths(btnPaths, btnPaths+4);
    
    if(!Switcher::init(paths))
        return false;
    
    setTouchMode(kCCTouchesOneByOne);
    setTouchEnabled(true);
    
    return true;
}
    
bool MailSwitcher::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    ContentLayer::ccTouchBegan(touch, event);
    CCPoint localPoint = container->convertTouchToNodeSpace(touch);
    for(CCSprite* btn : btns)
        if(btn->boundingBox().containsPoint(localPoint))
            return true;
    return false;
}
    
bool AchievementSwitcher::init()
{
    std::vector<std::string> paths;
    for(int i = 0; i < 4; i++)
    {
        paths.push_back("gameui/yuelifenlei"+toStr(i+1)+"_a.png");
        paths.push_back("gameui/yuelifenlei"+toStr(i+1)+"_b.png");
    }
    if(!Switcher::init(paths))
        return false;
    setAnchorPoint(ccp(0.5, 0.5));
    setCurNode(0);
    
    this->setTouchEnabled(true);
    
    return true;
}
    
bool RankSwitcher::init()
{
    std::vector<std::string> paths;
    for(int i = 0; i < 6; i++)
    {
        paths.push_back("gameui/zhanli"+toStr(i+1)+"_a.png");
        paths.push_back("gameui/zhanli"+toStr(i+1)+"_b.png");
    }
    if(!Switcher::init(paths))
        return false;
    setAnchorPoint(ccp(0.5, 0.5));
    setCurNode(0);
    
    this->setTouchEnabled(true);
    
    return true;
}
    
}
