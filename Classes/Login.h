#pragma once

#include "ContentLayer.h"
#include "cocos2d.h"
#include "IuputLabel.h"
#include <extensions/GUI/CCEditBox/CCEditBox.h>

USING_NS_CC;

namespace PH
{

class LoginLayer : public ContentLayer
{
public:
	bool init();

	CREATE_FUNC(LoginLayer);

private:
    void initOP();
    void initStandard();
    
private:
    CCSprite* landBg;
    cocos2d::extension::CCEditBox* accountText;
    cocos2d::extension::CCEditBox* pwText;
    cocos2d::extension::CCEditBox* pwConfirmText;

    void onInviteCode(cocos2d::CCObject *target);
    void onSinaLogin(cocos2d::CCObject *target);
    void onTencentLogin(cocos2d::CCObject *target);
    void onGuestLogin(cocos2d::CCObject *target);
    
    virtual void onEnter();
    virtual void onExit();

    CCLayer* createGem(const char* gemPath, const char* lightPath, float scale);
    CCAction* createGemOutAction(CCPoint org, CCPoint target);
};

}