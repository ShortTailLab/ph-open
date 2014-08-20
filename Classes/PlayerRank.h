//
//  PlayerRank.h
//  HelloCpp
//
//  Created by zql on 13-10-9.
//
//

#pragma once

#include "ContentLayer.h"
#include "ControlExtensions.h"
#include "Types.h"
#include "ZqlScrollView.h"

namespace PH
{

    enum PlayerRankType
    {
        Rank_mix = 0,
        Rank_water,
        Rank_fire,
        Rank_wood,
        Rank_light,
        Rank_dark,
        Rank_level
    };
    
    CCSprite* createMedal(int rank);
    
    class PlayerRankItem : public ContentLayer
    {
    public:
        virtual bool init(RankingItem& item, int rank, RankingType type, bool isMine);
        
        static PlayerRankItem* create(RankingItem& item, int rank, RankingType type, bool isMine)
        {
            PlayerRankItem* o = new PlayerRankItem();
            if(o != NULL && o->init(item, rank, type, isMine))
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }
        
    private:
        RankingItem rankData;
    };
    
    class PlayerRankLayer : public ContentLayer
    {
    public:
        ~PlayerRankLayer();
        
        virtual bool init();
        virtual void onEnterAnimationDone();
        void switchRank();
        ContentLayer* createZhanliLayer();
        ContentLayer* createLevelLayer();
        ZqlScrollView* createRankList(int type);
        void onBack();
        
        CREATE_FUNC(PlayerRankLayer);
        
    private:
        int rankType;
        LabelX* title;
        ContentLayer* currLayer;
        std::map<RankingType, int> rankMap;
        std::map<RankingType, int> deltaMap;
        std::map<RankingType, int> zhanliMap;
        CCSprite* statusBar;
        RankingType rankTypeToServer;
        CCLabelTTF* statusTitleLabel;
        CCLabelTTF* myZhanliLabel;
        CCNode* rankNode;
        std::map<RankingType, ZqlScrollView*> casheLayer;
        
        void updateBottomBar();
    };
    
    
}
