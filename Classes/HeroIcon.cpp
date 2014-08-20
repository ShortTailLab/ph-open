#include "HeroIcon.h"
#include "GameScene.h"
#include "CCCallLambda.h"
#include "UIPresets.h"
#include "PlayerData.h"

namespace PH
{

enum IconDecoratorDepth
{
    Elite = 10,
    LpLack = 15,
    Level = 20,
    Team = 21,
    Num = 22,
    Ultimate = 23,
    Lock = 30,
    PlusNum = 31,
    Mark = 32,
    New = 33
};

HeroIcon::HeroIcon() :
    frame(NULL), bg(NULL), bottom(NULL), lock(NULL), icon(NULL),
    elite(NULL), eliteNum(NULL),check(NULL), atlasNum(NULL), number(NULL),
    mIsClickEnabled(true),
    mIsLongClickEnabled(true),
    ownHero(false)
{
}

HeroIcon::~HeroIcon()
{
    if(ownHero)
        delete cacheHero;
}

bool HeroIcon::init(Hero *hero, bool isShowLock, bool isShowLevel)
{
	if(!init(hero->profile, false))
        return false;
    
    mLabelType = LabelType::Level;
    
    cacheHero = hero;
    ownHero = false;
    
	if(isShowLock && hero->isLock)
    {
		lock = GetSprite("gameui/suo.png");
		lock->setPosition(ccp(15, 90));
		this->addChild(lock, IconDecoratorDepth::Lock);
	}

	if(isShowLevel)
    {
        setBottomMsg(LabelType::Level);
        showElite(true);
	}

	return true;
}

bool HeroIcon::init(const HeroProfile *heroProfile, bool isGhost)
{
    if (!CCSprite::init())
        return false;
    
    std::string bgPath = "";
    switch (heroProfile->star)
    {
        case 1:
            bgPath = "gameui/touxiang_bg.png";
            break;
        case 2:
            bgPath = "gameui/2xing_bg.png";
            break;
        case 3:
            bgPath = "gameui/3xing_bg.png";
            break;
        case 4:
            bgPath = "gameui/4xing_bg.png";
            break;
        case 5:
            bgPath = "gameui/5xing_bg.png";
            break;
        default:
            bgPath = "gameui/6xing_bg.png";
            break;
    }
    bg = GetSprite(bgPath);
    CCSize size = bg->getContentSize();
    setContentSize(size);

	bg->setPosition(ccp(size.width*0.5f, size.height*0.5f));
    this->addChild(bg, -1);
    
    if(!isGhost)
        frame = GetSprite(GemUtils::frame(heroProfile->color));
    else
        frame = GetSprite("gameui/hunpo.png");

	frame->setPosition(ccp(size.width*0.5f , size.height*0.5f));
    this->addChild(frame, 1);

    icon = GetSprite(heroProfile->iconPath());
    if (!icon)
        icon = GetSprite("heroes/placeholder_icon.png");
    
    icon->setPosition(ccp(size.width*0.5f , size.height*0.5f));
    this->addChild(icon, 0);
    
    cacheHero = new Hero(toStr(1) , heroProfile->tableId , 1, 0);
    cacheHero->isLock = false;
    ownHero = true;
    mIsTouchable = false;
    mIsSwallow = false;
    
    return true;
}

void HeroIcon::setColor(const ccColor3B& color3)
{
    CCSprite::setColor(color3);
    frame->setColor(color3);
    icon->setColor(color3);
    bg->setColor(color3);
}
    
void HeroIcon::setOpacity(GLubyte opacity)
{
    CCSprite::setOpacity(opacity);
    bg->setOpacity(opacity);
    frame->setOpacity(opacity);
    icon->setOpacity(opacity);
}
    
void HeroIcon::levelBlink()
{
    /*
    auto* seq = CCSequence::create(CCFadeOut::create(0.5),
                                   CCCallLambda::create([=]()
                                   {
                                       if (level->getTag() == 1)
                                           ((CCLabelBMFont*)level)->setString("等級不足");
                                   }),
                                   CCFadeIn::create(0.5),
                                   CCFadeOut::create(0.5),
                                   CCCallLambda::create([=]()
                                   {
                                       if (level->getTag() == 1)
                                       {
                                           Hero* hero = this->getHero();
                                           int l = hero ? hero->level : 1;
                                           ((CCLabelBMFont*)level)->setString(("LV." + toStr(l)).c_str());
                                       }
                                   }) ,
                                   CCFadeIn::create(0.5),
                                   NULL);
    
    level->runAction(CCRepeatForever::create((CCActionInterval*)seq));*/
}

static inline void setVisibleSafely(CCNode* target, bool visible)
{
    if(target != NULL)
        target->setVisible(visible);
}
    
void HeroIcon::setVisible(bool visible)
{
    setVisibleSafely(frame, visible);
    setVisibleSafely(bg,    visible);
    setVisibleSafely(icon,  visible);
    setVisibleSafely(lock,  visible);
    setVisibleSafely(elite, visible);
    setVisibleSafely(eliteNum, visible);
    setVisibleSafely(number,   visible);
    setVisibleSafely(atlasNum, visible);
    setVisibleSafely(check,    visible);
    setVisibleSafely(bottom,   visible);
}

void HeroIcon::setBottomMsg(const LabelType& type)
{
    mLabelType = type;
    if(bottom)
    {
        bottom->removeFromParent();
        bottom = NULL;
    }
    
    Hero* hero = cacheHero;
    std::string msg = "";
    if(type == LabelType::Level)
    {
        if (hero->level == hero->profile->maxLevel)
        {
            bottom = GetSprite("gameui/lv_zuida.png");
            bottom->setTag(0);
        }
        else
            msg = "LV." + toStr(hero->level);
    }
    else if(type == LabelType::HP)
        msg = toStr(hero->health);
    else if(type == LabelType::Attack)
        msg = toStr(hero->attack);
    else if(type == LabelType::Regen)
        msg = toStr(hero->regen);
    else if(type == LabelType::Fight)
        msg = toStr(hero->fightSkill);
    else if(type == LabelType::Type)
    {
        bottom = CCLabelBMFont::create(hero->profile->title.c_str(), "bmfont/PW_24_white.fnt");
        bottom->setTag(1);
        bottom->setScale(contentScale());
        bottom->setAnchorPoint(ccp(0.5, 0.5));
        bottom->setPosition(ccp(50, 0));
    }
    else if(type == LabelType::Ultimate)
        bottom = GetSprite("gameui/zhongji.png");
    else if(type == LabelType::InTeam)
        bottom = GetSprite("gameui/shiyongzhong.png");
    else if(type == LabelType::LpLack)
        bottom = GetSprite("gameui/tongyubuzu.png");
    
    if(!msg.empty())
    {
        bottom = CCLabelBMFont::create(msg.c_str(), "bmfont/Sumo_22_white_brown.fnt");
        //this use to distinguish sprite and bmfont
        bottom->setTag(1);
        bottom->setScale(contentScale());
        bottom->setAnchorPoint(ccp(0.5, 0.5));
        bottom->setPosition(ccp(50, 0));
    }
    else
        bottom->setPosition(ccp(50, 3));
    
    this->addChild(bottom, IconDecoratorDepth::Level);
}
    
void HeroIcon::defaultBottmMsg()
{
    LabelType type = LabelType::Level;
    if(PlayerData::getInstance()->getSortType() == 3)
        type = LabelType::HP;
    else if(PlayerData::getInstance()->getSortType() == 4)
        type = LabelType::Attack;
    else if(PlayerData::getInstance()->getSortType() == 5)
        type = LabelType::Regen;
    else if(PlayerData::getInstance()->getSortType() == 6)
        type = LabelType::Type;
    else if(PlayerData::getInstance()->getSortType() == 7)
        type = LabelType::Fight;
    setBottomMsg(type);
}
    
void HeroIcon::showNum(int n)
{
    if(atlasNum == NULL)
    {
        atlasNum = CCLabelBMFont::create("", "bmfont/Sumo_22_white_brown.fnt" );
        atlasNum->setAnchorPoint(ccp(0.5, 0.5));
        atlasNum->setPosition(ccp(getContentSize().width/2, 3));
        atlasNum->setScale(contentScale());
        this->addChild(atlasNum, IconDecoratorDepth::Num);
    }
    
    atlasNum->setString(("NO." + toStr(n)).c_str());
}

void HeroIcon::showElite(bool isShow)
{
    if(elite)
    {
        elite->removeFromParentAndCleanup(true);
        elite = NULL;
    }

    if(isShow && cacheHero && cacheHero->eliteLevel)
    {
        elite = GetSprite("gameui/neili"+toStr(cacheHero->eliteLevel)+".png");
        elite->setPosition(ccp(45, 45));
        this->addChild(elite, IconDecoratorDepth::Elite);
    }
}

void HeroIcon::showPlusNum(int value)
{
    number = CCLabelBMFont::create(("+"+toStr(value)).c_str(), "bmfont/Sumo_22_yellow.fnt");
    number->setAnchorPoint(ccp(0,0.5));
    number->setPosition(ccp(0, 80));
    number->setScale(contentScale());
    this->addChild(number,10);
}

void HeroIcon::showNew()
{
    if(cacheHero->isLock)
        return;
    
    CCSprite* newSigh = GetSprite("gameui/new.png");
    newSigh->setPosition(ccp(15, 90));
    newSigh->setScale(0.8);
    this->addChild(newSigh, IconDecoratorDepth::PlusNum);
}

void HeroIcon::setEnabled(bool val)
{
    setDark(!val);
    setClickEnabled(val);
//    setLongClickEnabled(val);
}
    
void HeroIcon::setDark(bool value)
{
    ccColor3B color = value ? ccc3(100, 100, 100) : ccc3(255, 255, 255);
    icon->setColor(color);
    frame->setColor(color);
    if(elite)
        elite->setColor(color);
}

void HeroIcon::mark(bool value)
{
	if(value && !check)
    {
		check = GetSprite("gameui/duigou.png");
		check->setPosition(ccp(getContentSize().width/2-35,
                               getContentSize().height/2+33));
		this->addChild(check, IconDecoratorDepth::Mark);
	}
    else if(!value && check)
    {
        this->check->removeFromParentAndCleanup(true);
        this->check = NULL;
	}
}
    
//----------------------------------------

void HeroIcon::onEnter()
{
    CCSprite::onEnter();
    if(mIsTouchable)
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,mTouchPriority, mIsSwallow);
}

void HeroIcon::onExit()
{
    CCSprite::onExit();
    if(mIsTouchable)
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}
    
void HeroIcon::setClickEnabled(bool val)
{
    mIsClickEnabled = val;
    /*if(mIsTouchable)
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,mTouchPriority, mIsSwallow);
    else
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);*/
}

bool HeroIcon::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    // response to one touch at a time.
    if(mIsDown)
        return false;
    
    CCPoint touchPoint = getParent()->convertTouchToNodeSpace(pTouch);
    if (boundingBox().containsPoint(touchPoint))
    {
        this->scheduleOnce(schedule_selector(HeroIcon::onPressTimer), 0.5);
        mIsDown = true;
        return true;
    }
    
    return false;
}

void HeroIcon::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint currPoint = getParent()->convertTouchToNodeSpace(pTouch);
    CCPoint prevPoint = getParent()->convertToNodeSpace(pTouch->getPreviousLocation());
    
    CCPoint delta = currPoint - prevPoint;
    
    
    float deltaLen = delta.getLength();
    
    if (!boundingBox().containsPoint(currPoint) || !mIsDown || deltaLen > 6)
    {
        mIsDown = false;
        this->unschedule(schedule_selector(HeroIcon::onPressTimer));
        return;
    }
}

void HeroIcon::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    if (mIsDown)
    {
        this->unschedule(schedule_selector(HeroIcon::onPressTimer));
        if(mIsClickEnabled)
            mClick(this);
        
        mIsDown = false;
    }
}

void HeroIcon::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    mIsDown = false;
}

void HeroIcon::onPressTimer(float dt)
{
    mIsDown = false;
    if(mIsLongClickEnabled)
        mLongClick(this);
}

};
