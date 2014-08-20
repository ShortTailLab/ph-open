#include "BuildConf.h"
#include "Login.h"
#include "extensions/utils.h"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "GameScene.h"
#include "PuzzleRPC.h"
#include "CCNative.h"
#include "CCCallLambda.h"
#include "UIPresets.h"
#include "Common.h"

#if PH_USE_WEIBO
#include <auth/Weibo.h>
#endif


USING_NS_CC;
using namespace std;

bool isInLoginScreen = false;

namespace PH
{
    
bool LoginLayer::init()
{
	if(!ContentLayer::init())
		return false;

    LOGD("LoginLayer init...\n");
    
    CCSprite *bg = GetSprite("loading_bg.jpg");
	bg->setPosition(ccp(winSize.width*0.5f , winSize.height*0.5f));
	this->addChild(bg);
    
    CCSprite* logo = GetSprite("loading_logo.png");
    logo->setPosition(ccp(320, 200));
    this->addChild(logo, 100);
    
    CCSprite* light_back = GetSprite("gameui/yuanfen_guang_al.png");
    light_back->setPosition(ccp(320, 400));
    addChild(light_back,100);
    light_back->runAction(CCRepeatForever::create(CCSequence::create(CCFadeTo::create(1, 100),
                                                                     CCFadeTo::create(1, 255),
                                                                     NULL)
                                                  ));
    
    landBg = GetSprite("gameui/denglu.png");
    landBg->setPosition(ccp(320, 400));
    addChild(landBg, 101);
    
    CCPoint desPos[5] = {ccp(80, 260), ccp(110, 540), ccp(280, 850), ccp(540, 530), ccp(535, 180)};
    CCLayer* gems[5] = {createGem("gem_fire.png", "guangquan_hong.png", 0.86),
                        createGem("gem_light.png", "guangquan_huang.png", 0.76),
                        createGem("gem_water.png", "guangquan_lan.png", 0.8),
                        createGem("gem_wood.png", "guangquan_lv.png", 0.9),
                        createGem("gem_dark.png", "guangquan_zi.png", 0.8)};
    for(int i = 0; i < 5; i++)
    {
        gems[i]->setPosition(landBg->getPosition());
        this->addChild(gems[i]);
        gems[i]->runAction(CCSequence::create(CCDelayTime::create(0.15*i),
                                              createGemOutAction(landBg->getPosition(), desPos[i]),
                                              NULL));
        CCSprite* light = (CCSprite*)gems[i]->getChildByTag(1001);
        if(light)
        {
            float orgScale = light->getScale();
            light->runAction(CCSequence::create(CCDelayTime::create(0.15*i+0.8),
                                                CCRepeatForever::create(CCSequence::create(
                                                    CCCallLambda::create([=]()
                                                                         {
                                                                             light->setOpacity(255);
                                                                             light->setScale(orgScale);
                                                                         }),
                                                    CCSpawn::create(CCScaleTo::create(0.6, 1.3),
                                                                    CCFadeOut::create(0.6),
                                                                    NULL),
                                                    
                                                    NULL)),
                                                NULL));
        }
    }

    initStandard();

	return true;
}
    
void LoginLayer::onEnter()
{
    ContentLayer::onEnter();
    isInLoginScreen = true;
}

void LoginLayer::onExit()
{
    ContentLayer::onExit();
    isInLoginScreen = false;
}

void LoginLayer::initStandard()
{    
    
	CCMenu *topMenu = CCMenu::create();
    
    /*
     // define weibo login credentials in AppDelegate.cpp
     // then uncomment this thunk to enable weibo login
	CCMenuItem *itemUp = CCMenuItemSprite::create(createBtn("微博登陆", ButtonType::FOUR_WORD_RED),
                                                  createBtn("微博登陆", ButtonType::FOUR_WORD_RED),
                                                  this,
                                                  menu_selector(LoginLayer::onSinaLogin));
	topMenu->addChild(itemUp);
     */
    
    /*
     // define QQ login credentials in AppDelegate.cpp
     // then uncomment this thunk to enable QQ login
	CCMenuItem *itemMid = CCMenuItemSprite::create(createBtn("QQ登陆", ButtonType::FOUR_WORD_RED),
                                                   createBtn("QQ登陆", ButtonType::FOUR_WORD_RED),
                                                   this,
                                                   menu_selector(LoginLayer::onTencentLogin));
	topMenu->addChild(itemMid);
     */
    
	CCMenuItem *itemDown = CCMenuItemSprite::create(createBtn("快速注册", ButtonType::FOUR_WORD_RED),
                                                    createBtn("快速注册", ButtonType::FOUR_WORD_RED),
                                                    this,
                                                    menu_selector(LoginLayer::onGuestLogin));
	topMenu->addChild(itemDown);
    
	topMenu->alignItemsVerticallyWithPadding(5);
	topMenu->setPosition(ccp(landBg->getContentSize().width*0.5, landBg->getContentSize().height*0.5-20));
	landBg->addChild(topMenu);
}
    
void LoginLayer::onInviteCode(cocos2d::CCObject *target)
{
#if PH_USE_WEIBO
    //inviCode->attachWithIME();
#endif
}

void LoginLayer::onSinaLogin(cocos2d::CCObject *target)
{
#if PH_USE_WEIBO
    auto& weibo = Weibo<Sina>::instance();
    if (weibo.isAuthorized())
    {
        GameScene::getInstance()->loginOurs(AccountType::SinaWeibo,
                                            Weibo<Sina>::instance().userId(),
                                            Weibo<Sina>::instance().token(),
                                            "");
    }
    else
    {
        weibo.authorize([=](auto_ptr<AuthError> authError)
        {
            if (authError.get())
            {
                LOGD("Sina weibo auth error: %s", authError->error.c_str());
                return;
            }
            
            Weibo<Tencent>::instance().clearLoginInfo();
            
            CCUserDefault::sharedUserDefault()->setStringForKey("guestLoginID", "");
            CCUserDefault::sharedUserDefault()->setStringForKey("guestLoginPWD", "");
            CCUserDefault::sharedUserDefault()->flush();
            
            GameScene::getInstance()->loginOurs(AccountType::SinaWeibo,
                                                Weibo<Sina>::instance().userId(),
                                                Weibo<Sina>::instance().token(),
                                                "");
        });
    }
#endif
}

void LoginLayer::onTencentLogin(cocos2d::CCObject *target)
{
#if PH_USE_WEIBO
    auto& weibo = Weibo<Tencent>::instance(); 
    if (weibo.isAuthorized())
        GameScene::getInstance()->loginOurs(AccountType::QQ, weibo.userId(), weibo.token(), "");
    else
    {
        weibo.authorize([=](auto_ptr<AuthError> authError){
            if (authError.get())
            {
                CCLog("Tencent weibo auth error: %s", authError->error.c_str());
                return;
            }
            
            Weibo<Sina>::instance().clearLoginInfo();
            CCUserDefault::sharedUserDefault()->setStringForKey("guestLoginID", "");
            CCUserDefault::sharedUserDefault()->setStringForKey("guestLoginPWD", "");
            CCUserDefault::sharedUserDefault()->flush();
            GameScene::getInstance()->loginOurs(AccountType::QQ, Weibo<Tencent>::instance().userId(), Weibo<Tencent>::instance().token(), "");
        });
    }
#endif
}

void LoginLayer::onGuestLogin(cocos2d::CCObject *target)
{
    LOGD("attemping guest login\n");
    using namespace boost::uuids;
    
#if PH_USE_WEIBO
    if (!Weibo<Sina>::instance().userId().empty())
        return onSinaLogin(target);
    if (!Weibo<Tencent>::instance().userId().empty())
        return onTencentLogin(target);
#endif
  
    string uid = CCUserDefault::sharedUserDefault()->getStringForKey("guestLoginID");
    string password = CCUserDefault::sharedUserDefault()->getStringForKey("guestLoginPWD");
    
    if (uid.empty())
    {
        auto gen = boost::uuids::random_generator();
        uid = to_string(gen());
        password = to_string(gen());

        CCUserDefault::sharedUserDefault()->setStringForKey("guestLoginID", uid);
        CCUserDefault::sharedUserDefault()->setStringForKey("guestLoginPWD", password);
        CCUserDefault::sharedUserDefault()->flush();
    }
    
    SystemInform::alertConfirmPanel(runningScene(),
                                    "快速登录如果登出将会丢失用户信息，请及时绑定账户",
                                    "警告",
                                    [=]()
    {
        GameScene::getInstance()->loginOurs(AccountType::Guest, uid, password, "");
    },
                                    [](){});
}
    
CCLayer* LoginLayer::createGem(const char* path, const char* lightPath, float scale)
{
    CCLayer* layer = CCLayer::create();
    CCSprite* gem = GetSprite(path);
    gem->setScale(scale);
    layer->addChild(gem, 10);
    
    CCSprite* light = GetSprite(lightPath);
    light->setOpacity(0);
    light->setTag(1001);
    light->setScale(scale);
    layer->addChild(light);
    
    return layer;
}

CCAction* LoginLayer::createGemOutAction(CCPoint org, CCPoint target)
{
    double angle = atan2(target.y-org.y, target.x-org.x);
    CCPoint middle = ccp(50*cos(angle), 50*sin(angle));
    return CCSequence::create(CCMoveTo::create(0.5, target+middle),
                              CCMoveTo::create(0.2, target),
                              CCRepeat::create(
                                               CCSequence::create(CCMoveTo::create(0.8, ccp(target.x, target.y+15)),
                                                                  CCMoveTo::create(0.8, ccp(target.x, target.y-15)),
                                                                  NULL),100000),
                              NULL);
}

}


