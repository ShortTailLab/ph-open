//
//  PvPLayer.h
//  HelloCpp
//
//  Created by Zeyang Li on 4/26/13.
//
//

#pragma once

#include "ContentLayer.h"
#include "XiaKe.h"
#include "JiangHu.h"
#include "ControlExtensions.h"
#include "HeroProfileLayer.h"

USING_NS_CC;

namespace PH
{
    USING_NS_CC;

    std::string getTitleOfRepu(int reputation);
    
    class PvPInviteLayer : public ContentLayer
    {
    private:
        CCTextFieldTTF* textInput;
        
        CCMenu* friendsMenu;
        
    public:
        virtual bool init();
        void search(CCObject* o);
        void invite(CCObject* o);

        CREATE_FUNC(PvPInviteLayer);
    };
    
    class PvPTeamConfirmJuanZhou : public JuanZhou
    {
    public:
        PvPTeamConfirmJuanZhou(Player* p):player(p){};
        
        virtual bool init(bool isSwitchEnable);
        void initHeroRelations();
        virtual void openDone();
        virtual void onLongClick(CCTouch* touch);
        
        static PvPTeamConfirmJuanZhou *create(Player* player, bool isSwitchEnable = true)
        {
            PvPTeamConfirmJuanZhou *o = new PvPTeamConfirmJuanZhou(player);
            if(o && o->init(isSwitchEnable))
            {
                o->autorelease();
                return o;
            }
            return NULL;
        };
        
    private:
        Player* player;
        
        BianDuiSwitcher* switcher;
        CCLabelTTF* skillLabel;
        LabelX* descLabel;
        CCLabelBMFont* hpLabel;
        CCLabelBMFont* regenLabel;
        CCLabelBMFont* attackLabel;
        std::vector<CCLabelBMFont*> resistLabels;
        ScrollRelations* relationsView;
        RelationsEffect* effectLayer;
        
        void refresh();
        const std::vector<Hero*> getTeam();
    };

    class PvPTeamConfirmLayer : public ContentLayer
    {
    public:
        virtual bool init();
        virtual void onEnter();
        virtual void onEnterAnimationDone();
        
        
        CREATE_FUNC(PvPTeamConfirmLayer);
        
    private:
        CCMenu* menu;
        JuanZhouLayer* jz;
        int teamIdOnEnter;
        
        void onBack();
        
    };
    
    class RankItem : public ContentLayer
    {
    public:
        virtual bool init(Player* player, bool isSelf);
        virtual void setTouchEnabled(bool value);
        
        static RankItem *create(Player* player, bool isSelf = false)
        {
            RankItem *o = new RankItem();
            if(o && o->init(player, isSelf))
            {
                o->autorelease();
                return o;
            }
            return NULL;
        };
        
    private:
        CCLabelTTF* nameLabel;
        CCLabelBMFont* levelLabel;
        CCLabelBMFont* reputationLabel;
        ButtonSprite* teamBtn;
    };
    
    class PvPRankLayer : public ContentLayer
    {
    public:
        virtual bool init();
        virtual void setTouchEnabled(bool value);
        void alertTeamBoard(Player* player);
        void cancelTeamBoard();
        
        CREATE_FUNC(PvPRankLayer);
        
    private:
        std::auto_ptr<PlayerRankList> playerList;
        std::vector<RankItem*> items;
        ZqlScrollView* scrollView;
        CCMenu* topMenu;
        CCLayer* alertLayer;
        
        void onBack();
        void makeItems(const std::vector<Player*>& players);
    };
    
    class PvPMsgBoard : public ContentLayer
    {
    public:
        virtual bool init();
        
        CREATE_FUNC(PvPMsgBoard);
        
    private:
        CCLabelBMFont* rankLabel;
        CCLabelBMFont* titleLabel;
        CCLabelBMFont* reputationLabel;
        CCLabelBMFont* currencyLabel;
        CCLabelBMFont* probabilityLabel;
        
    };
    
    enum class PVPLogLogo
    {
        LOGO_UP = 0,
        LOGO_DOWN,
        LOGO_NULL
    };
    
    class PvPInformLayer : public ContentLayer
    {
    public:
        virtual bool init();
        
        CREATE_FUNC(PvPInformLayer);
        
        void onBack();
    private:
        void makeLog(ZqlScrollView* scrollView, const PvPGameLog& log);
        ContentLayer* createItem(PVPLogLogo logo, std::string msg);
    };
    
    class PvPStoreLayer : public ContentLayer
    {
    public:
        virtual bool init();
        void makeItems();
        std::string getIconPath(const PvPStoreItemType& type);
        ContentLayer* createAItem(const PvPStoreItem& item);
        void onBack();
        
        CREATE_FUNC(PvPStoreLayer);
        
    private:
        CCLabelBMFont* scoreLabel;
        
        void updateBar();
    };
    
    class PvPLayer : public ContentLayer
    {
    public:
        virtual bool init();
        
        void onRank(cocos2d::CCObject *target);
        void onStore(cocos2d::CCObject *target);
        void onZhanTie(cocos2d::CCObject *target);
        
        CREATE_FUNC(PvPLayer);
        
    private:
        CCLabelBMFont* leitaiLabel;
        std::string enterBtnPath;
        ButtonSprite* enterBtn;
        
        void schedule1Sec();
        int t;
    };
}
