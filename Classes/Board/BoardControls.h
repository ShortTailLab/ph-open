//
//  BoardControls.h
//  HelloCpp
//
//  Created by Zeyang Li on 4/16/13.
//
//

#pragma once

#include <utility>
#include <boost/make_shared.hpp>

#include "Board/Common.h"
#include "Board/Gem.h"
#include "Board/Grid2D.h"
#include "Board/Utils.h"
#include "Board/HeroControl.h"
#include "Board/PlayerControl.h"
#include "Board/EnemyControl.h"

namespace PH
{
    USING_NS_CC;
    USING_NS_CC_EXT;
   
    typedef std::pair<int, GemUtils::GemColor> NGemSet;
    typedef std::vector<NGemSet> NGemComboList;

    typedef std::vector<GemSet> GemComboList;
    
    class AttackTable
    {
    public:
        struct Value
        {
            float attack;
            bool isArea;
            CCLabelBMFont* attackLabel;
        };
        
        float heal;
        CCLabelBMFont* healLabel;
        
        AttackTable(HeroControlList& l)
        {
            for(HeroControl* hc : l)
            {
                table[hc] = { 0.0f, false, NULL };
            }
            
            heal = 0.0f;
            healLabel = NULL;
        }
        
        bool contains(HeroControl* hc) const
        {
            return table.find(hc) != table.end();
        }
        
        Value& operator() (HeroControl* hc)
        {
            return table.at(hc);
        }
        
        const Value& operator() (HeroControl* hc) const
        {
            return table.at(hc);
        }
        
    private:
        std::map<HeroControl*, Value> table;
    };
    
    class BoardResult
    {
    private:
        NGemComboList comboList;
        
    public:
        AttackTable baseAttack;
        std::vector<CCSprite*> comboTextList;
        
    public:
        BoardResult(HeroControlList& team) : baseAttack(team)
        {
        }
        
        NGemComboList combosByColor(GemUtils::GemColor c) const
        {
            NGemComboList list;
            for(const NGemSet& combo : comboList)
            {
                assert(combo.first > 0 && "combo is empty");
                if(combo.second == c)
                    list.push_back(combo);
            }
            return list;
        }
        
        void add(const GemSet& combo, CCSprite* comboText)
        {
            comboList.push_back(std::make_pair(combo.size(), (*combo.begin())->color()));
            comboTextList.push_back(comboText);
        }
                
        int comboCount() const { return comboList.size(); }
        void clear() { comboList.clear(); }
        
        int totalGemCount() const
        {
            int count = 0;
            for(auto p : comboList)
                count += p.first;

            return count;
        }
    };
    
    typedef boost::shared_ptr<BoardResult> BoardResultPtr;
    
    class DamageTable
    {
    public:
        DamageTable(EnemyControlList& l)
        {
            for(EnemyControlPtr ec : l)
                table[ec] = 0.0f;
        }
        
        bool contains(EnemyControlPtr ec) const
        {
            return table.find(ec) != table.end();
        }
        
        float& operator() (EnemyControlPtr ec)
        {
            return table.at(ec);
        }
        
        const float& operator() (EnemyControlPtr ec) const
        {
            return table.at(ec);
        }
        
        const std::map<EnemyControlPtr, float>& items() { return table; };
        
    private:
        std::map<EnemyControlPtr, float> table;
    };
    
    static const int skBoardWidth = 6;
    static const int skBoardHeight = 5;
    
    class BoardControl;
    typedef boost::shared_ptr<BoardControl> BoardControlPtr;
    typedef boost::function<GemUtils::GemColor ()> ColorGenerator;
    
    typedef Grid2D<GemUtils::GemColor> GemColorGrid;
    
    class BoardControl : public IObject
    {
    protected:
        GemGrid     mGrid;
        
        void addGem(const Vec2i& pos, GemPtr gem);
    public:
        CCNode*     root;
        float       damageFromGems;
        
        // --- IObject ---
        BoardControl() : root(NULL), mGrid(skBoardWidth, skBoardHeight), damageFromGems(0) {}
        ~BoardControl();
        INITIALIZE_OBJECT();
        virtual void vSerialize(std::ostringstream& out) const{}
        virtual void vDeserialize(std::istringstream& in){}
        
        virtual void vSerialize(rapidjson::Value& o, rapidjson::Document::AllocatorType& alloc) const override;
        virtual void vDeserialize(rapidjson::Value& o) override;
        
        
        static BoardControlPtr make(ColorGenerator gen)
        {
            BoardControlPtr ret = boost::make_shared<BoardControl>();
            if(ret && !ret->init(boost::move(gen)))
                ret.reset();
            return ret;
        }
        
        static BoardControlPtr make()
        {
            BoardControlPtr ret = boost::make_shared<BoardControl>();
            if(ret && !ret->init())
                ret.reset();
            return ret;
        }

        virtual bool init();
        virtual bool init(ColorGenerator gen);
        
        void fillBoardFirstTime(ColorGenerator gemGen);
        
        // --- presets ---
        TaskPtr removeGemFromGrid(GemPtr gem);
        TaskPtr removeGemFromGrid(const Vec2i & pos);
        TaskPtr removeGemFromGrid(const int pos);
        
        void removeGemFromGridRaw(GemPtr gem);
        void removeGemFromGridRaw(const Vec2i& pos);
        void removeGemFromGridRaw(const int pos);

        TaskPtr fillBoard(ColorGenerator gemGen,
                          bool forceNoStacking = false);
        TaskPtr sweepBoard(PlayerControlPtr player,
                           BoardResultPtr result);
        
        TaskPtr patchGemGrid(GemColorGrid& patch);
        TaskPtr updateGemGrid();
        
        TaskPtr replaceGem(const Vec2i& pos, GemPtr gem);
        TaskPtr replaceGemRaw(const Vec2i& pos, GemPtr gem);
        
        void swapGem(const Vec2i& left, const Vec2i right);
        
        // --- accessor --
        bool hasEmptyCell();
        inline GemGrid & getGrid() { return mGrid; }
        inline GemPtr operator() (const Vec2i & pos){ return mGrid(pos); }
        inline GemPtr get(const Vec2i & pos) { return mGrid(pos); }
    };
    
//    TaskPtr calcDamagePvP(CCLayer* layer,
//                          const float attack,
//                          const GemUtils::GemColor color,
//                          const PlayerControlPtr target,
//                          float& damage);
    
    TaskPtr healWithAnim(CCLayer* layer,
                         PlayerControl* player,
                         float healthToHeal,
                         bool isRevial=false,
                         CCLabelBMFont* healLabel = NULL,
                         bool doPassiveHeal = false);
    
    std::vector<int> genReasonableFingerPath(GemGrid & grid, int maxCombo=9999);
}
