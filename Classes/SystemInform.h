#pragma once

#include <cocos2d.h>
#include <extensions/utils.h>
#include <boost/function.hpp>
#include <boost/move/move.hpp>
#include <string>
#include <CCRichLabelTTF.h>

#include "Board/Utils.h"
#include "Types.h"
#include "ContentLayer.h"

namespace PH
{
    USING_NS_CC;
    class SystemInform;

    class RewardBoard : public CCLayer
    {
    public:
        ~RewardBoard();
        
        bool init(SystemInform* inform,
                  const LoginRewards& loginRewards,
                  const std::vector<int>& path);
        
        virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
        virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
        virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
        virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
        
        void createRoute(const std::vector<int>& path);
        virtual void registerWithTouchDispatcher();
        
        static RewardBoard* create(SystemInform* inform,
                                   const LoginRewards& loginRewards,
                                   const std::vector<int>& path)
        {
            RewardBoard* o = new RewardBoard();
            if(o != NULL && o->init(inform, loginRewards, path))
            {
                o->autorelease();
                return o;   
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }
        
    private:
        bool isPressed;
        int currNode;
        int currUp;
        int currLeft;
        std::vector<int> path;
        std::vector<CCSprite*> items;
        LoginRewards rewards;
        CCLayer* routeLayer;
        CCPoint startPoint;
        CCSprite* man;
        CCSprite* man2;
        CCLayer* arrowLayer;
        CCRichLabelTTF* msgLabel;
        CCSprite* btn;
        SystemInform* inform;
        CCAnimation* anim;
        
        void initCurrPos();
        void choose(int index);
        void connectServer(int index);
        CCSprite* createRewardItem(const Treasure& type);
        std::string getRewardTitle(const Treasure& type);
        void connect(int dot1, int dot2);
        CCSprite* connect(CCLayer* parent,
                     const CCPoint& p1,
                     const CCPoint& p2,
                     bool isRoad = true);
        void blink(CCSprite* node);
        CCPoint getManPos(const CCPoint& pos);
    };
    
    class SystemInform : public CCLayer
    {
    public:
        SystemInform() : rewardBoard(NULL){};
        
        static void alertMessage(CCNode* parent,
                                 const char* msg,
                                 const char* title,
                                 boost::function<void()> ycall);
        
        static void alertMessage(CCNode* parent,
                                 const char* msg,
                                 const char* title,
                                 CCPoint center,
                                 bool useBg,
                                 boost::function<void()> ycall);
        
        static void alertTips(CCNode* parent,
                              const char* msg,
                              const char* title);
        
        static SystemInform* alertLoading(CCNode* parent);
        
        //-----------------------------------------
        // Only for alerting skill info in battle
        static void alertSkill(CCNode* parent,
                               const std::string& title,
                               const std::string& detail,
                               const std::string& energy,
                               CCPoint p,
                               boost::function<void()> ycall,
                               boost::function<void()> ncall);
        
        //-----------------------------------------
        // Annoucement alert
        static void alertAnnoucement(CCNode* parent,
                                     const std::string& title,
                                     const std::string& html,
                                     boost::function<void()> ycall);
        
        //-----------------------------------------
        // Panel has confirm and cancel buttons
		static void alertConfirmPanel(CCNode* parent,
                                      const char* title,
									  const char* msg,
									  boost::function<void()> yesCall, 
									  boost::function<void()> noCall,
                                      bool hasSchedule = false,
                                      std::string btnLabel = "确定",
                                      std::string btnLabel2 = "取消");
        
        static void alertConfirmPanel(CCNode* parent,
                                      const char* title,
									  const char* msg,
                                      CCPoint center,
									  boost::function<void()> yesCall,
									  boost::function<void()> noCall);
        
        static void alertConfirmPanel2(CCNode* parent,
                                       const char* msg,
                                       const char* title,
                                       boost::function<void()> call1,
                                       boost::function<void()> call2,
                                       boost::function<void()> call3,
                                       boost::function<void()> call4);
        
        static void alertBattleExitMenu(CCNode* parent,
                                        const std::string& stageName,
                                        const std::string& levelName,
                                        int round,
                                        int maxRound,
                                        int heroDrop,
                                        int soulDrop,
                                        boost::function<void()> yes);
        
        static void alertVIPLevelUpInform(CCNode* parent,
                                          int levelTo,
                                          const VipUpgrade& vipMsg);
        
        static void alertPurchaseVipPacks(CCNode* parent,
                                          const VIPPack& pack,
                                          boost::function<void()> purchaseSucCall);
        
        static void alertVipPacks(CCNode* parent,
                                  const VIPPack& pack,
                                  boost::function<void()> purchaseSucCall);
        
        static void alertHint(CCNode* parent,
                              const std::string& hint,
                              boost::function<void()> yes);
        
        static void alertLandReward(CCNode* parent,
                                    const LoginRewards& loginRewards,
                                    const std::vector<int>& path,
                                    boost::function<void()> yes);
        static void alertRewardTips(CCNode* parent,
                                    std::string title,
                                    const LoginRewards& loginRewards,
                                    boost::function<void()> yes,
                                    std::string hintTips = "",
                                    std::string showShareMsg = "");
        static void alertAchievementPacks(CCNode* parent,
                                        const LoginRewards& loginRewards,
                                        boost::function<void()> yes);
        static void alertSharePanel(CCNode* parent, std::string msg);
        static void alertShareConfirmPanel(CCNode* parent, int type, std::string msg);
        static void alertPvPMsg(CCNode* parent);
        
        void cancel();
        
        virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
        virtual void registerWithTouchDispatcher();
        
    private:
        static SystemInform* announceInform;
        static SystemInform* landRewardInform;
        CCLayerColor *background;
        RewardBoard* rewardBoard;
        int timeCount;
        
		boost::function<void ()> yesCall;
		boost::function<void ()> noCall;
        boost::function<void ()> call1;
        boost::function<void ()> call2;
        boost::function<void ()> call3;

        bool initWithMessage(const char* message,
                             const char* title,
                             CCPoint p,
                             bool useBg,
                             boost::function<void()> ycall);
        
        bool initWithTips(const char* message,
                          const char* title,
                          CCPoint p);
        
        bool initWithLoading();
        
        bool initWithAnnoucement(const std::string& title,
                                 const std::string& annoucement,
                                 bool useBg,
                                 boost::function<void()> ycall);
        
		bool initWithPanel(const char* msg,
                           const char* title,
                           CCPoint p,
                           boost::function<void()> yesCall,
                           boost::function<void()> noCall,
                           bool hasSchedule = false,
                           std::string btnLabel = "",
                           std::string btnLabel2 = "");
        bool initWithPanel2(const char* msg,
                            const char* title,
                            CCPoint p,
                            boost::function<void()> call1,
                            boost::function<void()> call2,
                            boost::function<void()> call3,
                            boost::function<void()> call4);
        
        bool initWithSkill(const std::string& title,
                           const std::string& detail,
                           const std::string& energy,
                           CCPoint p,
                           boost::function<void()> ycall,
                           boost::function<void()> ncall);
        
        bool initBattleExitMenu(const std::string& stageName,
                                const std::string& levelName,
                                int round,
                                int maxRound,
                                int heroDrop,
                                int soulDrop,
                                boost::function<void()> yes);
        
        bool initWithHint(const std::string& hint,
                          boost::function<void()> yes);
        bool initVIPLevelUpInform(int level, const VipUpgrade& vipMsg);
        bool initPurchaseVipPacks(const VIPPack& pack,
                                  boost::function<void()> purchaseSucCall,
                                  bool isPurchase = true);
        
        bool initWithLandReward(const LoginRewards& loginRewards,
                                const std::vector<int>& path,
                                boost::function<void()> yes);
        bool initWithRewardTips(std::string title,
                                const LoginRewards& loginRewards,
                                boost::function<void()> yes,
                                std::string hintTips,
                                bool isAchievement,
                                std::string shareMsg);
        bool initWithSharePanel(std::string msg);
        bool initWithShareConfirmPanel(int type, std::string msg);
        bool initWithPvPMsg();

		void onConfirm(CCObject *target);
		void onCancel(CCObject *target);
        void onCall1(CCObject *target);
        void onCall2(CCObject *target);
        void onCall3(CCObject *target);
        
    };
    
    
    class AlertMessage : public CCLayer
    {
    public:
        static AlertMessage* create(const std::string& title,
                                    const std::string& message,
                                    bool useDarkBG = false,
                                    boost::function<void()> click = [](){})
        {
            AlertMessage* o = new AlertMessage;
            if(o && o->init(runningScene(), title, message, useDarkBG, click))
            {
                o->autorelease();
                return o;
            }
            
            CC_SAFE_DELETE(o);
            return NULL;
        }
        

        static AlertMessage* create(CCNode* node,
                                    const std::string& title,
                                    const std::string& message,
                                    bool useDarkBG = false,
                                    boost::function<void()> click = [](){})
        {
            AlertMessage* o = new AlertMessage;
            if(o && o->init(node, title, message, useDarkBG, click))
            {
                o->autorelease();
                return o;
            }
            
            CC_SAFE_DELETE(o);
            return NULL;
        }
        
        virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
        virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
        
        virtual void registerWithTouchDispatcher();
        virtual void update(float dt);
        void attachImg(CCSprite* img);
        
    private:
        virtual bool init(CCNode* parent,
                          const std::string& title,
                          const std::string& message,
                          bool useDarkBG,
                          boost::function<void()> click);
        
    private:
        bool waitingForInput;
        int pageIndex, nowIndex;
        double delta;
        CCSprite* textBG;

        std::vector<std::wstring> pages;
        CCRichLabelTTF* textLabel;
        CCSprite * cursor;
        boost::function<void()> lastClick;
    };
    
    class BattleExitMenu : public CCLayer
    {
    private:
        std::vector<CCRect>      areas;
        std::vector<CCSprite*>   circles;
        boost::function<void ()> yesCall;
        boost::function<void ()> noCall;
        
    public:
        static BattleExitMenu * create(CCNode * parent,
                                       const std::string& stageName,
                                       const std::string& levelName,
                                       int round,
                                       int maxRound,
                                       int heroDrop,
                                       int soulDrop,
                                       boost::function<void()> yes,
                                       boost::function<void()> no)
        {
            BattleExitMenu * ret = new BattleExitMenu();
            if(ret && ret->init(parent,
                                stageName, levelName,
                                round, maxRound, heroDrop, soulDrop,
                                boost::move(yes), boost::move(no)))
            {
                ret->autorelease();
                return ret;
            }
            
            CC_SAFE_DELETE(ret);
            return NULL;
        }
        
        virtual bool init(CCNode * parent,
                          const std::string& stageName,
                          const std::string& levelName,
                          int round,
                          int maxRound,
                          int heroDrop,
                          int soulDrop,
                          boost::function<void()> yes,
                          boost::function<void()> no);
    
        virtual void onConfirm();
        virtual void onCancel();
        
        /*
        virtual void onSingleClick(CCTouch* touch);
        */
        virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
        virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
        
        virtual void registerWithTouchDispatcher();
    };
    
    
    class ModalBase : public CCLayer
    {
    private:
        CCLayerColor* mBG;
    public:
        virtual bool init(CCNode* parent, bool enableBG);
        virtual void registerWithTouchDispatcher();
        
        virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    };
    
    class LoadingDialog : public ModalBase
    {
    public:
        static LoadingDialog* create(CCNode* parent,
                                     const std::string& title,
                                     const std::string& message)
        {
            LoadingDialog* o = new LoadingDialog();
            if(o && o->init(parent, title, message))
            {
                o->autorelease();
                return o;
            }
            
            CC_SAFE_RELEASE(o);
            return NULL;
        }
        
        virtual bool init(CCNode* parent,
                          const std::string& title,
                          const std::string& message);
        
        void setMessage(const std::string& message);
        void cancel();
    private:
        CCLabelTTF* mMsgLabel;
    };
}
