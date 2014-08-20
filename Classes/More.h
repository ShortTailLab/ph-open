#pragma once

#include "cocos2d.h"
#include "ContentLayer.h"
#include "UIComponent.h"
#include "IuputLabel.h"
#include "../libs/extensions/cocos-ext.h"
#include "ZqlScrollView.h"
#include "ResourceSyncMan.h"

namespace PH
{
    
class ShowLayer : public ContentLayer
{
public:
	virtual bool init();
	void onBack(cocos2d::CCObject *target);
    CREATE_FUNC(ShowLayer);
    
protected:
    
    virtual void onLongClick(CCTouch* touch);
    virtual void onSingleClick(CCTouch* touch);

private:
    HeroIcon* curTouchIcon;
    ZqlScrollView *scrollView;
    HeroBoard *board;
    bool isClick;
};

    
class ResDownloadLayer : public CCLayer
{
public:
    CREATE_FUNC(ResDownloadLayer);
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    
protected:
    virtual bool init();
    void setProgress(float progress);
    void download();
    std::string bytesToReadableString(int bytes);
//    virtual void registerWithTouchDispatcher();
    
private:
    boost::shared_ptr<ResourceSyncMan> syncMan;
    CCLabelTTF* mText;
    CCSprite* mProgressBar;    
};
    
class SettingJuanZhou : public JuanZhou
{
public:
	virtual bool init();
    virtual void onSingleClick(CCTouch* touch);

	void onMusicOn();
	void onMusicOff();
	void onSoundOn();
	void onSoundOff();
    
	void onUseHD();
	void onUseSD();
    
	CREATE_FUNC(SettingJuanZhou);

private:
	CCMenuItem *music_on;
	CCMenuItem *music_off;

	CCMenuItem *sound_on;
	CCMenuItem *sound_off;
    
    CCMenuItem *useHDButton;
    CCMenuItem *useSDButton;
    
    std::vector<CCSprite*> circles;
    std::vector<CCRect> btns;
};

class SettingLayer : public ContentLayer
{
public:
	virtual bool init(bool isInStack = true);
	void onBack(cocos2d::CCObject *target);
	virtual void onEnterAnimationDone();

	CREATE_FUNC(SettingLayer);
    
    static SettingLayer* create(bool isInStack)
    {
        SettingLayer* o = new SettingLayer();
        if(o && o->init(isInStack))
        {
            o->autorelease();
            return o;
        }
        
        CC_SAFE_DELETE(o);
        return NULL;
    }
    
private:
	JuanZhouLayer *jz;
    bool mIsInStack;
};
    
class AccountJuanZhou : public JuanZhou
{
public:
    bool init();
    
    void onChangeName(cocos2d::CCObject *target);
    void onBindSina(cocos2d::CCObject *target);
    void onBindQQ(cocos2d::CCObject *target);
    void showNameLabel();
    
    CREATE_FUNC(AccountJuanZhou);

private:
    CCSprite *bg;
    InputLabel *inputText;
    extension::CCEditBox* editText;
    CCMenuItemSprite *bindSina;
    CCMenuItemSprite *bindQQ;
    CCMenu *menu ;
    
    bool isBindSina;
    bool isBindQQ;
};
    
class AccountLayer : public ContentLayer
{
public:
    bool init();
    void onBack(cocos2d::CCObject *target);
    void onLogout(cocos2d::CCObject *target);
    virtual void onEnterAnimationDone();
    
    JuanZhouLayer *jz;
    
    CREATE_FUNC(AccountLayer);
};
    
class AboutLayer : public ContentLayer
{
public:
    bool init();
    void onBack(cocos2d::CCObject *target);
	virtual void onEnterAnimationDone();
    
    JuanZhouLayer *jz;
    
    CREATE_FUNC(AboutLayer);
};
    
class HelpLayer : public ContentLayer
{
public:
    bool init();
    
    void onBack(CCObject* target);
    
    CREATE_FUNC(HelpLayer);
};

class MoreLayer : public ContentLayer
{
public:

	virtual bool init();
	virtual void setTouchEnabled(bool value);
	void onTuJian(cocos2d::CCObject *target);
	void onSetting(cocos2d::CCObject *target);
    void onAccountSettings(cocos2d::CCObject *target);
    void onAboutUs(cocos2d::CCObject *target);
    void onHelp(CCObject* target);
    void onGongGao();
    void exitGame();
    
    virtual void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);

	CREATE_FUNC(MoreLayer);

private:
    bool isClick;
    std::vector<CCSprite*> items;
};

}
