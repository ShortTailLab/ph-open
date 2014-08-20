#pragma once

#include "ContentLayer.h"
#include "cocos2d.h"
#include "BattleScene.h"
#include "UIComponent.h"
#include "IuputLabel.h"
#include "ControlExtensions.h"
#include "ZqlScrollView.h"
#include "ButtonSprite.h"
#include "../libs/extensions/cocos-ext.h"

USING_NS_CC_EXT;

namespace PH
{
    
class MyFriendLayer;

class FriendItem : public ContentLayer
{
public:
    PlayerPtr player;
    
	virtual bool init(PlayerPtr player);
    
    virtual void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    void update();
    
	std::string playerName;

	static FriendItem *create(PlayerPtr player)
    {
		FriendItem *o = new FriendItem();
		if(o && o->init(player)){
			o->autorelease();
			return o;
		}
		return NULL;
	};
    
protected:
    HeroIcon *heroImg;
    cocos2d::CCMenu *menu;
    bool isClick;
    
private:
    void onDelete(cocos2d::CCObject *target);

};

class FriendAskItem : public FriendItem
{
public:
	virtual bool init(PlayerPtr player);
	void onDelete(cocos2d::CCObject *target);
	void onAccept(cocos2d::CCObject *target);
    
    
	std::string playerName;

	static FriendAskItem *create(PlayerPtr player){
		FriendAskItem *o = new FriendAskItem();
		if(o && o->init(player)){
			o->autorelease();
			return o;
		}
		return NULL;
	};
};

class SearchResultItem : public FriendItem
{
public:
	virtual bool init(const Player& player);

	Player player;

	static SearchResultItem *create(const Player& player)
    {
		SearchResultItem *o = new SearchResultItem();
		if(o && o->init(player))
        {
			o->autorelease();
			return o;
		}
		return NULL;
	};
    
private:
    void onAdd(cocos2d::CCObject *target);

};

class MyFriendLayer : public ContentLayer
{
public:
	virtual bool init();
	virtual CCMenuItem *getTitle();
	virtual void getPlayers();
	virtual ContentLayer *getItem(PlayerPtr player);
	virtual void makeItems(std::vector<PlayerPtr > friends);
	virtual void onBack(cocos2d::CCObject *target);
    void initHeroNumTips();
    std::vector<PlayerPtr>  getFriendsBySortType();

	ZqlScrollView *scrollView;
	std::vector<PlayerPtr> players;

	CREATE_FUNC(MyFriendLayer);
    
private:
    CCSprite* heroNumBg;
    CCLabelTTF* numLabel;
    LabelX* title;
    std::vector<ContentLayer*> items;
    
    std::string getSortStr();
    void onChangeSortType();
};

class FriendAskLayer : public MyFriendLayer
{
public:
	virtual CCMenuItem *getTitle();
	virtual void getPlayers();
	virtual ContentLayer *getItem(PlayerPtr player);
    virtual void makeItems(std::vector<PlayerPtr > friends);
    std::vector<PlayerPtr> clipPlayers();

	CREATE_FUNC(FriendAskLayer);
    
};


class SearchResultLayer : public MyFriendLayer
{
public:
	virtual bool init(PlayerList *list);
	virtual CCMenuItem *getTitle();
	virtual void getPlayers();
	virtual void makeItems(const std::vector<Player*>& players);
	virtual ContentLayer *getItem(const Player& player);
	virtual void onBack(cocos2d::CCObject *target);

	static SearchResultLayer *create(PlayerList *list){
		SearchResultLayer *o = new SearchResultLayer();
		if(o && o->init(list)){
			o->autorelease();
			return o;
		}
		return NULL;
	};
};

class SearchJuanZhou : public JuanZhou
{
public:
	virtual bool init();
    virtual void setTouchEnabled(bool value);
    
	cocos2d::CCLabelTTF *idLabel;
    cocos2d::extension::CCEditBox* editText;
    CCMenu *menu;
    
	void onSearch(cocos2d::CCObject *target);

	CREATE_FUNC(SearchJuanZhou);
    
private:
    CCSprite *bg;
};

class SearchFriendLayer : public ContentLayer
{
public:
	virtual bool init();
    virtual void setTouchEnabled(bool value);

    virtual void onEnterAnimationDone();
    
	void onBack(cocos2d::CCObject *target);

    SearchJuanZhou *searchBoard;
	JuanZhouLayer *zj;
    

	CREATE_FUNC(SearchFriendLayer);
};
    
class InviteJuanZhou : public JuanZhou
{
public:
    virtual bool init();
    //virtual void setTouchEnabled(bool value);
    
    CCMenu *menu;
    
    
    void onInvite(cocos2d::CCObject *target);
    void onInvSina(cocos2d::CCObject *target);
    void onInvTencent(cocos2d::CCObject *target);
    void onInvWechat(cocos2d::CCObject *target);
    
    //virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    
    CREATE_FUNC(InviteJuanZhou);
    
private:
    CCSprite *bg;
    std::vector<std::string> tweets;
 
};
    
class InviteFriendLayer : public ContentLayer
{
public:
    virtual bool init();
    virtual void onEnterAnimationDone();
    
    void onBack(cocos2d::CCObject *target);
    
    CREATE_FUNC(InviteFriendLayer);
    
private:
    JuanZhouLayer* zj;
    InviteJuanZhou* inviteJuanZhou;
};
    
class MailItem : public ContentLayer
{
public:
    bool isOpen;
    
    virtual bool init();
    virtual void onSingleClick(CCTouch* touch);
    virtual void update(float dt);
    void refresh(Mail* mail);
    void updateReadState();
    void reSize();
    void write();
    
    static MailItem* create()
    {
        MailItem* o = new MailItem();
        if(o && o->init())
        {
            o->autorelease();
            return o;
        }
        return NULL;
    }
    
protected:
    CCLabelTTF* senderLabel;
    CCLabelBMFont* typeLabel;
    CCLabelTTF* topicLabel;
    CCLabelTTF* hintBody;
    CCLabelTTF* timeLabel;
    CCLabelBMFont* zhutiLabel;
    ButtonSprite* openBtn;
    CCSprite* packLogo;
    ScissorLayer* content;
    CCSprite* zhouBottom;
    CCLabelBMFont* readLabel;
    CCScale9Sprite* bg;
   
    CCSprite* marginLeft;
    CCSprite* marginRight;
    CCRect touchRectUp;
    CCRect touchRectDown;
    int minHeight;
    int maxHeight;
    Mail* mail;
    bool isSendMail;
};
    
class WriteMailItem : public MailItem
{
public:
    virtual bool init();
    virtual void onSingleClick(CCTouch* touch);
    
    CREATE_FUNC(WriteMailItem);
    
private:
    CCLabelTTF* inputLabel;
    CCEditBox* hackLabel;
    std::string inputText;
    bool tailFlag;
    
    void update(float dt);
    void schedule1Sec(float dt);
};
    
class ShareBoard : public MailItem
{
public:
    virtual bool init(std::string sharedMsg);
    
    static ShareBoard* create(std::string sharedMsg)
    {
        ShareBoard* o = new ShareBoard();
        if(o && o->init(sharedMsg))
        {
            o->autorelease();
            return o;
        }
        return NULL;
    }
};
    
class SendMailLayer : public ContentLayer
{
public:
    virtual bool init();
    
    CREATE_FUNC(SendMailLayer);
};

class MailLayer : public ContentLayer
{
public:
    ~MailLayer();
    
    virtual bool init();
    virtual void onEnter();
    void makeItems(int type);
    void onBack();
    
    CREATE_FUNC(MailLayer);
    
private:
    MenuScrollView* scrollView;
    MailSwitcher* switcher;
    std::vector<Mail> received;
    std::vector<Mail> sent;
};

class FriendLayer : public ContentLayer
{
public:
	virtual bool init();
	virtual void setTouchEnabled(bool value);
    virtual void onEnter();
	void onMyFriend(CCObject *Target);
	void onFriendAsk(CCObject *Target);
	void onSearchFriend(CCObject *Target);
    void onInviteFriend(CCObject *Target);
    void onMail(CCObject *Target);
    
	cocos2d::CCMenu *menu;

	CREATE_FUNC(FriendLayer);

private:
    CCMenuItem *friendAskBtn;
    CCMenuItem *msgBtn;
    cocos2d::CCSprite* tips;
    cocos2d::CCSprite* friReqTips;
    cocos2d::CCSprite* mailTips;
    
};

}