//
//  BoardControls.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 4/16/13.
//
//

#include <iterator>

#include "DebugMisc.h"
#include "BoardControls.h"

#include "Board/AnimPreset.h"
#include "Board/HeroTable.h"

#include "Animation/CCCallLambda.h"
#include "CCScale9Sprite.h"

namespace PH
{
    USING_NS_CC;
    USING_NS_CC_EXT;
    
    static void findSets(GemGrid::Array l, std::vector<GemSet>& sets)
    {
        GemUtils::GemColor lastGemColor = l(0)->color();
        GemSet gemSet;
        gemSet.insert(l(0));
        
        for(int i=1; i<l.size(); ++i)
        {
            GemPtr gem = l(i);
            if(gem->color() != lastGemColor)
            {
                sets.push_back(gemSet);
                gemSet.clear();
                lastGemColor = gem->color();
            }
            gemSet.insert(gem);
        }
        
        sets.push_back(gemSet);
        
        // filter out all sets with less than 3 gems
        sets.erase(remove_if(sets.begin(), sets.end(), [](const GemSet& g){ return g.size() < 3; }), sets.end());
    }
    
    static void insertDisjointGemSet(GemComboList& gemSets, const GemSet& newset)
    {
        GemComboList setsToMerge;
        
        for(GemSet& refset : gemSets)
        {
            GemSet cross = intersectGemSet(refset, newset);
            if(cross.size())
                setsToMerge.push_back(refset);
        }
        
        GemSet merged = newset;
        for(const GemSet& s : setsToMerge)
        {
            // merge intersecting sets with newset
            merged.insert(s.begin(), s.end());
            
            // remove intersecting set from combo list
            gemSets.erase(std::find(gemSets.begin(), gemSets.end(), s));
        }
        
        gemSets.push_back(merged);
    }
    
    static CCPoint computeGemSetCenter(GemSet& set)
    {
        std::vector<GemPtr> sorted;
        sorted.insert(sorted.end(), set.begin(), set.end());
        
        std::sort(sorted.begin(), sorted.end(), [](GemPtr a, GemPtr b)->bool
                  {
                      return (a->position[0] < b->position[0]) ||   // sort by x coord
                      (a->position[0] == b->position[0] && a->position[1] < b->position[1]); // if the same, sort by y corrd
                  } );
        
        Vec2i c = sorted[sorted.size() / 2]->position;
        CCPoint p = g2w_center(c);
        
        return p;
    }
    
    static GemUtils::GemColor forceDifferentGem(const GemGrid& grid,
                                           const Vec2i& p,
                                           boost::function<GemUtils::GemColor ()> gemGen)
    {
        bool gemAllowed[GemUtils::AllColor];
        for(int i=0; i<GemUtils::AllColor; i++)
            gemAllowed[i] = true;
        
        Vec2i around[4] = { Vec2i(p[0]+1, p[1]), Vec2i(p[0]-1, p[1]),
            Vec2i(p[0], p[1]-1), Vec2i(p[0], p[1]+1) };
        
        for(const Vec2i p : around)
        {
            if(p[0] >=0 && p[0] < BOARD_WIDTH &&
               p[1] >=0 && p[1] < BOARD_HEIGHT)
            {
                const GemPtr gem = grid(p);
                if(gem)
                    gemAllowed[gem->color()] = false;
            }
        }
        
        while(true)
        {
            GemUtils::GemColor c = gemGen();
            if(gemAllowed[c])
                return c;
        }
        
        return GemUtils::Fire;
    }
    
    //------------------------
    // BoardControl
    //------------------------
    BoardControl::~BoardControl()
    {
        safeRelease(root);
    }
    
    bool BoardControl::init()
    {
        this->root = CCLayer::create();
        if(this->root == NULL)
            return false;
        
        this->root->retain();
        this->root->setPosition(ccp(0, 0));
        
        return true;
    }
    
    bool BoardControl::init(ColorGenerator gen)
    {
        this->root = CCLayer::create();
        if(this->root == NULL)
            return false;
        
        this->root->retain();
        this->root->setPosition(ccp(0, 0));
        this->fillBoardFirstTime(gen);
        
        return true;
    }
    
    void BoardControl::vSerialize(rapidjson::Value& o, rapidjson::Document::AllocatorType& alloc) const
    {
        using namespace rapidjson;
        
        // Grid info
        for(int i=0; i<mGrid.size(); i++)
        {
            GemPtr gem = mGrid(i);
            Value gemVal(kObjectType);
            gem->vSerialize(gemVal, alloc);
            
            o.PushBack(gemVal, alloc);
        }
    }
    
    void BoardControl::vDeserialize(rapidjson::Value& o)
    {
        assert(false && "not implemented");
    }
    
    void BoardControl::fillBoardFirstTime(ColorGenerator gemGen)
    {
        for(int x=0; x<mGrid.Width; ++x)
        {
            // count empty cells in row
            for(int y=0; y<mGrid.Height; ++y)
            {
                assert( mGrid(x,y) == NULL && "mGemGrid is not empty");
                
                GemUtils::GemColor left = x-1 >= 0 ? mGrid(x-1,y)->color(): GemUtils::GemColorMax;
                GemUtils::GemColor bottom = y-1 >= 0 ? mGrid(x,y-1)->color() : GemUtils::GemColorMax;
                
                // randomize a color that is not the same as left and bottom gems
                GemUtils::GemColor rc;
                while(true)
                {
                    rc = gemGen();
                    if(rc != left && rc != bottom)
                        break;
                }
                GemPtr gem = Gem::make(rc);
                
                this->addGem(Vec2i(x, y), gem);
            }
        }
    }
    
    void BoardControl::swapGem(const Vec2i &left, const Vec2i right)
    {
        GemPtr a = get(left);
        GemPtr b = get(right);
        
        // swap grid item
        std::swap(mGrid(a->position),
                  mGrid(b->position));
        // swap position of the two gems
        std::swap(a->position, b->position);
    }
    
    void BoardControl::removeGemFromGridRaw(GemPtr gem)
    {
        mGrid(gem->position).reset();
    }
    void BoardControl::removeGemFromGridRaw(const Vec2i& pos)
    {
        if(pos[0] < 0 || pos[0] >= mGrid.Width ||
           pos[1] < 0 || pos[1] >= mGrid.Height)
            return;
        return this->removeGemFromGridRaw(mGrid(pos));
    }
    void BoardControl::removeGemFromGridRaw(const int pos)
    {
        if(pos < 0 || pos >= mGrid.size())
            return;
        return this->removeGemFromGridRaw(mGrid(pos));
    }
    
    TaskPtr BoardControl::removeGemFromGrid(GemPtr gem)
    {
        mGrid(gem->position).reset();
        return TaskSequence::make()
        << gem->fadeOut()
        << TaskLambda::make([=]()
        {
            gem->root->removeFromParent();
        });
    }
    
    TaskPtr BoardControl::removeGemFromGrid(const Vec2i & pos)
    {
        if(pos[0] < 0 || pos[0] >= mGrid.Width ||
           pos[1] < 0 || pos[1] >= mGrid.Height)
            return TaskIgnore::make();
        
        GemPtr gem = mGrid(pos);
        return removeGemFromGrid(gem);
    }
    
    TaskPtr BoardControl::removeGemFromGrid(const int pos)
    {
        if(pos < 0 || pos >= mGrid.size())
            return TaskIgnore::make();
        
        GemPtr gem = mGrid(pos);
        return removeGemFromGrid(gem);
    }
    
    TaskPtr BoardControl::fillBoard(ColorGenerator gemGen,
                                    bool forceNoStacking)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        const float speed = 0.07f;
        
        for(int x=0; x<mGrid.Width; ++x)
        {
            int emptyCellCount = 0;
            
            // count empty cells in row
            for(int y=0; y<mGrid.Height; ++y)
            {
                GemPtr gem = mGrid(x, y);
                if(gem == NULL)
                    emptyCellCount++;
                else if(emptyCellCount)
                {
                    // maintain position and grid
                    gem->position[1] -= emptyCellCount;
                    mGrid(gem->position) = gem;
                    
                    float time = speed * (emptyCellCount+2);
                    TaskBatchPtr b2 = TaskBatch::make();
                    
                    batch << createDelay(root, std::max(0.03f, time - 0.3f));
                    batch << gem->moveTo(gem->position, time);
                }
            }
            
            int filledCellCount = mGrid.Height - emptyCellCount;
            
            // generate new gems to fill in empty cells
            for(int i=0; i<emptyCellCount; ++i)
            {
                Vec2i p = Vec2i(x, filledCellCount+i);
                GemPtr gem = Gem::make(forceNoStacking ? forceDifferentGem(mGrid, p, gemGen) : gemGen());
                this->addGem(p, gem);
                
                // set initial anim position
                gem->root->setPosition(g2w_center(Vec2i(x, mGrid.Height + i)));
            
                TaskSequencePtr seq = TaskSequence::make();
                gem->root->setOpacity(0);
                seq << TaskLambda::make([=]() { gem->root->setOpacity(255); });

                float time = speed * (emptyCellCount + 2);
                TaskBatchPtr b2 = TaskBatch::make();
                b2 << gem->moveTo(gem->position, time);
                // use this time to control fall duration, the actual falling animation
                // is nonblocking
                b2 << createDelay(root, std::max(0.03f, time - 0.3f));
                seq << b2;
                
                //seq << TaskLambda::make([=](){gem->root->setPosition(g2w_center(gem->position));});
                
                batch << seq;
                
                mGrid(gem->position) = gem;
            }
        }
        
        return batch;
    }
    
    TaskPtr BoardControl::sweepBoard(PlayerControlPtr player,
                                     BoardResultPtr result)
    {
        GemComboList combos;
        
        //find horizonal sets
        for(int y=0; y<mGrid.Height; ++y)
        {
            std::vector<GemSet> sets;
            GemGrid::Array row = mGrid.row(y);
            findSets(row, sets);
            
            for(GemSet set : sets)
                insertDisjointGemSet(combos, set);
        }
        
        //find vertical sets
        for(int x=0; x<mGrid.Width; ++x)
        {
            std::vector<GemSet> sets;
            GemGrid::Array col = mGrid.column(x);
            findSets(col, sets);
            
            for(GemSet set : sets)
                insertDisjointGemSet(combos, set);
        }
        
#if PH_DEBUG_BUILD
        for(GemSet& set1 : combos)
        {
            for(GemPtr g1 : set1)
            {
                int count = 0;
                for(GemSet& set2 : combos)
                {
                    for(GemPtr g2 : set2)
                    {
                        if(g1 == g2)
                            count++;
                    }
                }
                assert(count == 1 && "sets are not disjoint");
            }
        }
#endif
        auto sweepBatch = TaskBatch::make();
        
        int     boostEnergyCount = 0;
        float   boostEnergy = 1.f;
        auto    boostEnergyColor = GemUtils::AllColor;
        
        //if( player->buffGroup.invokeBuff("active.boost.energy.by.factor", sweepBatch) )
        if( player->buffGroup.isBuffActive("active.boost.energy.by.factor") )
        {
            auto buff = player->buffGroup.getBuffContent<FloatBuff>("active.boost.energy.by.factor");
            boostEnergy = buff->d1;
            boostEnergyColor = buff->getColor();
        }
    
        float delay = 0.f;
        // compute results and animate gems
        for(GemSet& set : combos)
        {
            auto comboBatch = TaskBatch::make();
            auto comboSeq = TaskSequence::make();
            comboSeq << createDelay(runningScene(), delay);
            delay += 0.325f;
            
            // clear each combo set
            bool still = true;
            for(GemPtr gem : set)
            {
                TaskSequencePtr localSeq = TaskSequence::make();
                localSeq << gem->sweep(this);
                
                // gem->sweep might interrupt this procedure
                if(mGrid(gem->position)) still = false;
                
                // count all gem sweeps
                player->mTotalGemCount[gem->color()]++;
                comboBatch << localSeq;
            }
            comboBatch << TaskSound::make("sound/v/gem_sweep.mp3");
            
            if(!still) {
                comboSeq << comboBatch;
                sweepBatch << comboSeq;
                continue;
            }
            
            //sweepSeq << createDelay(parent, 0.05f);
            
            CCPoint setCenter = computeGemSetCenter(set);
            
            // if this is not the first iteration.
            if(result->comboCount() + player->minComboCount)
            {
                // Result MUST be used additively because it may carry stuff from
                // previous fill sweep iterations.
                CCSprite* comboText = createComboTextLabel(result->comboCount() + player->minComboCount + 1,
                                                           setCenter);
                comboText->setVisible(false);
                comboText->setScale(0);
                this->root->addChild(comboText, ORDER_MASK);
                
                comboBatch << TaskLambda::make([=](){ comboText->setVisible(true); });
                comboBatch << TaskAnim::make(comboText,
                                             CCEaseElasticOut::create(CCScaleTo::create(0.3, 1.0)));
                
                result->add(set, comboText);
            }
            else
                result->add(set, NULL);
            
            GemUtils::GemColor setColor = (*set.begin())->color();
            
            // Handle energy bonus. Only handle exact gem count.
            float totalBonus = 0.0f;
            if(player->passiveEnergyBonusByGemCount[setColor].isValid())
            {
                for(auto p : player->passiveEnergyBonusByGemCount[setColor].map())
                {
                    int count = (int) p.second.first;
                    float bonus = p.second.second;
                    totalBonus += set.size() >= count ? bonus : 0.0f;
                }
            }
            
            bool flag = false;
            if( GemUtils::Health == setColor ) flag = true;
            if( player->buffGroup.invokeBuffWhen("superRegen", comboBatch, setColor))
            {
                flag = true;
            }
            
            if(flag && player->isHurt())
            {
                float prevHeal = result->baseAttack.heal;
                
                float extraGemFactor = 0.25f;
                comboBatch << player->getExtraGemsFactor(extraGemFactor);
                result->baseAttack.heal += player->totalRegen() * (1 + (set.size() - 3) * extraGemFactor);
                
                CCFiniteTimeAction* numberSeq = createNumberAnimSeq(prevHeal,
                                                                    result->baseAttack.heal,
                                                                    NUMBER_DURATION);
                if(!result->baseAttack.healLabel)
                {
//                    result->baseAttack.healLabel = player->createHealLabel(this->root);
                    result->baseAttack.healLabel = player->createHealLabel(this->root->getParent());
                }
                
                
                comboBatch << TaskAnim::make(result->baseAttack.healLabel, CCShow::create())
                << TaskAnim::make(result->baseAttack.healLabel, numberSeq, false);
            }
            
            auto buff = player->buffGroup.getBuffContent<BoolBuff>("superRegen");
            if( flag && buff && !buff->d1 )
            {
                comboSeq << comboBatch;
                sweepBatch << comboSeq;
                continue;
            }
            
            for(HeroControl* hc : player->team)
            {
                GemUtils::GemColor heroColor = hc->info.profile->color;
                
                // if hero color is not the same as gem color, ignore this hero
                if(setColor == heroColor)
                {
                    comboBatch << createOrb(this->root, setCenter, hc->center(), false);
                    
                    float prevAttack = result->baseAttack(hc).attack;
                    
                    float extraGemFactor = 0.25f;
                    comboBatch << player->getExtraGemsFactor(extraGemFactor);
                    result->baseAttack(hc).attack += hc->attack * (1 + (set.size() - 3) * extraGemFactor);
                    
                    CCFiniteTimeAction* numberSeq = createNumberAnimSeq(prevAttack,
                                                                        result->baseAttack(hc).attack,
                                                                        NUMBER_DURATION);
                    if(!result->baseAttack(hc).attackLabel)
                    {
                        result->baseAttack(hc).attackLabel = hc->createAttackLabel();
                        comboBatch << TaskAnim::make(result->baseAttack(hc).attackLabel,
                                                     CCSpawn::create(CCFadeIn::create(0.1),
                                                                     numberSeq,
                                                                     NULL),
                                                     false);
                    }
                    else
                    {
                        comboBatch << TaskAnim::make(result->baseAttack(hc).attackLabel,
                                                     numberSeq,
                                                     false);
                    }
                    
                    float energy = set.size() * std::max(player->mEnergyFactor[hc->getColor()],
                                                         player->mEnergyFactor[GemUtils::AllColor]);
                    energy += totalBonus;
                    
                    if( (boostEnergyColor == GemUtils::AllColor || boostEnergyColor == hc->getColor()) &&
                        boostEnergy != 1.f )
                    {
                        boostEnergyCount ++;
                        if( boostEnergyCount == 1)
                        {
                            player->buffGroup.invokeBuff("active.boost.energy.by.factor", sweepBatch);
                        }
                        energy *= boostEnergy;
                    }
                    
                    comboBatch << hc->addEnergy(energy);
                }
            }
            comboSeq << comboBatch;
            sweepBatch << comboSeq;
        }
        
        return sweepBatch;
    }
    
    TaskPtr BoardControl::patchGemGrid(GemColorGrid& patch)
    {
        phassert(patch.Width == mGrid.Width && patch.Height == mGrid.Height && "Grid size mismatch");
        
        auto batch = TaskBatch::make();
        float delay = 0.01;
        for(int y=0; y<patch.Height; y++)
        {
            for(int x=0; x<patch.Width; x++)
            {
                auto seq = TaskSequence::make();
                seq << createDelay(this->root, delay);
                seq << mGrid(x, y)->transformColor(patch(x, y));
                batch << seq;
                delay += 0.02;
            }
        }
        
        return batch;
    }
    
    TaskPtr BoardControl::updateGemGrid()
    {
        auto batch = TaskBatch::make();
        
        std::vector<int> shuffled_seq;
        for(int i=0; i<mGrid.size(); i++)
        {
            //if(mGrid(i)->isModifiable())
            shuffled_seq.push_back(i);
        }
        std::random_shuffle(shuffled_seq.begin(), shuffled_seq.end());
        // --> WARNING <--
        // this may cause crush, since some elements could be filled
        // as null pointer, between task of batch and task of fill.
        for(int i=0; i<shuffled_seq.size(); i++)
        {
            int iter = shuffled_seq[i];
            batch << mGrid(iter)->update(this);
        }
    
        return batch;
    }
    
    void BoardControl::addGem(const Vec2i& pos, GemPtr gem)
    {
        if(pos[0] < 0 || pos[0] >= mGrid.Width ||
           pos[1] < 0 || pos[1] >= mGrid.Height || !gem)
        {
            return;
        }
        
        mGrid(pos) = gem;
        gem->position = pos;
        
        if(gem->root->getParent() != NULL)
            gem->root->removeFromParent();
        
        gem->root->setPosition(g2w_center(pos));
        this->root->addChild(gem->root);
    }
    
    TaskPtr BoardControl::replaceGem(const Vec2i& pos,GemPtr now)
    {
        if(pos[0] < 0 || pos[0] >= mGrid.Width ||
           pos[1] < 0 || pos[1] >= mGrid.Height)
            return TaskIgnore::make();
        
        auto prev = mGrid(pos);
        auto batch = TaskBatch::make();
        
        now->position = pos;
        //now->root->setPosition(g2w_center(pos));
        
        batch << prev->fadeOut(0.8f);
        batch << now->fadeIn(0.8f);
        
        return TaskSequence::make()
        << TaskLambda::make([=]()
        {
            root->addChild(now->root);
            now->root->setPosition(g2w_center(now->position));
        })
        << batch
        << TaskLambda::make([=]()
        {
            mGrid(pos) = now;
        });
    }
    
    TaskPtr BoardControl::replaceGemRaw(const Vec2i &pos, GemPtr now)
    {
        if(pos[0] < 0 || pos[0] >= mGrid.Width ||
           pos[1] < 0 || pos[1] >= mGrid.Height)
            return TaskIgnore::make();
        
        auto prev = mGrid(pos);
        auto batch = TaskBatch::make();
        
        now->position = pos;
        //now->root->setPosition(prev->root->getPosition());
        
        batch << prev->fadeOut(0.8f);
        batch << now->fadeIn(0.8f);
    
        mGrid(pos) = now;
        
        return TaskSequence::make()
        << TaskLambda::make([=]()
        {
            root->addChild(now->root);
            now->root->setPosition(g2w_center(now->position));
        })
        << batch
        << TaskLambda::make([=]()
        {
            auto self = prev;
        });
    }
    
    bool BoardControl::hasEmptyCell()
    {
        for(int i=0; i<mGrid.size(); ++i)
        {
            if(!mGrid(i))
                return true;
        }
        return false;
    }
//
//    TaskPtr calcDamagePvP(CCLayer* layer,
//                          const float attack,
//                          const GemUtils::GemColor color,
//                          const PlayerControlPtr defender,
//                          float& damage)
//    {
//        float resistant = defender->resist.val[color];
//        float realDamage = attack * (1.0f - resistant);
//        
//        float damageFactor = 1.f, reflectFactor = 0.f;
//        GemUtils::GemColor reflectColor = GemUtils::Fire;
//        
//        auto batch = TaskBatch::make();
//        batch << defender->onHitCalculation(layer, color, realDamage,
//                                            damageFactor, reflectFactor, reflectColor);
//        damage = realDamage;
//        
//        return batch;
//    }
//    
    TaskPtr healWithAnim(CCLayer* layer,
                         PlayerControl* player,
                         float healthToHeal,
                         bool isRevival,
                         CCLabelBMFont* healLabel,
                         bool doPassiveHeal)
    {
        // Handle health gems. Also accounts for passive healing skills.
        TaskSequencePtr seq = TaskSequence::make();
        
        if(player->isHurt() && (player->isAlive()||isRevival))
        {
            if(!healLabel)
            {
                healLabel = player->createHealLabel(layer);
                seq << TaskAnim::make(healLabel, CCShow::create());
                
                if(healthToHeal > 1.f)
                {
                    CCFiniteTimeAction* numberSeq = createNumberAnimSeq(0.0f,
                                                                        healthToHeal,
                                                                        NUMBER_DURATION);
                    seq << TaskAnim::make(healLabel, numberSeq);
                }
            }
            
            float totalHeal = healthToHeal;
            
            // passive heal
            
            if(doPassiveHeal && player->passiveHealByFactor.isValid())
            {
                TaskBatchPtr localBatch = TaskBatch::make();
                float from = totalHeal;
                
                for(auto x : player->passiveHealByFactor.map())
                {
                    float regenFactor = 0.f;
                    localBatch << player->getRegenFactor(x.first, regenFactor);
                    totalHeal += x.first->regen * ( regenFactor + x.second );
                }
                
                CCFiniteTimeAction* numberSeq = createNumberAnimSeq(from,
                                                                    totalHeal,
                                                                    NUMBER_DURATION);
                localBatch << TaskAnim::make(healLabel, numberSeq);
                
                seq << localBatch;
            }
            
            {
                auto localBatch = TaskBatch::make();
                if(doPassiveHeal && player->buffGroup.invokeBuff("healByMaxHealth", localBatch))
                {
                    auto buff = player->buffGroup.getBuffContent<FloatBuff>("healByMaxHealth");
                    float from = totalHeal;
                    totalHeal += buff->d1*player->getMaxHealth();
                    CCFiniteTimeAction* numberSeq = createNumberAnimSeq(from,
                                                                        totalHeal,
                                                                        NUMBER_DURATION);
                    localBatch << TaskAnim::make(healLabel, numberSeq);
                    
                    seq << localBatch;
                }
            }
            
            //if(player->reduceHealingFactor.has())
            {
                auto localBatch = TaskBatch::make();
                if( player->buffGroup.invokeBuff("reduceHealingFactor", localBatch) )
                {
                    float reduceHealingFactor = player->buffGroup.getBuffContent<FloatBuff>("reduceHealingFactor")->d1;
                    float from = totalHeal;
                    totalHeal = totalHeal * reduceHealingFactor;
                    CCFiniteTimeAction* numberSeq = createNumberAnimSeq(from,
                                                                        totalHeal,
                                                                        NUMBER_DURATION);
                    localBatch << TaskAnim::make(healLabel, numberSeq);
                    seq << localBatch;
                }
            }
            
            if(totalHeal > 0.0f)
            {
                TaskBatchPtr healBatch = TaskBatch::make();
                healBatch << player->heal(totalHeal);
                healBatch << player->createHealingAnim();
                healBatch << TaskSound::make("sound/v/heal.mp3");
                
                seq << healBatch;
                seq << TaskLambda::make([=]()
                                        {
                                            healLabel->removeFromParent();
                                        });
            }
            else
            {
                // if no healing occurs, discard generated animations
                seq->list.clear();
                healLabel->removeFromParent();
            }
        }
        else if(healLabel)
        {
            seq << TaskLambda::make([=]()
            {
                healLabel->removeFromParent();
            });
        }
        
        return seq;
    }
    
    struct ValuePoint
    {
        int val[GemUtils::GemColorMax];
        std::map<int, std::pair<GemUtils::GemColor,int>> neighbours;
        
        int pattern;
        
        bool is_potential_point;
        GemUtils::GemColor potential_color;
        int potential_dir;
        int potential_count;
        
        ValuePoint()
        : is_potential_point(false), pattern(1)
        {
            memset(val, 0, sizeof(val));
        }
    };
    
    typedef Grid2D<long>            MaskGrid;
    typedef Grid2D<GemUtils::GemColor>   ColorGrid;
    typedef std::vector<int>        PathList;
    
    int grfp_search(ColorGrid & grid, int pos, int nextPos, int original)
    {
        if(pos < 0 || pos >= grid.size()) return 0;
        if(pos == original) return 0;
        if(nextPos < 0 || nextPos >= grid.size()) return 1;
        if(grid(pos) != grid(nextPos)) return 1;
        
        int r1 = pos / grid.Width;
        int c1 = pos % grid.Width;
        int r2 = nextPos / grid.Width;
        int c2 = nextPos % grid.Width;
        
        if( std::abs(r1-r2) + std::abs(c1-c2) != 1) return 1;
        
        return grfp_search(grid, nextPos, 2*nextPos - pos, original) + 1;
    }
    
    int grfp_get_point_value_by_color(ColorGrid &grid, MaskGrid & mask,
                                      GemUtils::GemColor color,
                                      int pos, int original)
    {
        if( pos < 0 || pos >= grid.size() ) return 0;
        if( mask(pos) ) return 0;
        
        //
        int r1 = pos / grid.Width;
        int c1 = pos % grid.Width;
        std::vector<int> dirs;
        if(r1 > 0) dirs.push_back(-grid.Width);
        if(r1 < grid.Height-1) dirs.push_back(grid.Width);
        if(c1 > 0) dirs.push_back(-1);
        if(c1 < grid.Width-1) dirs.push_back(1);
        
        int ret = 0;
        for( auto & item : dirs )
        {
            int val = grfp_search(grid, pos + item, pos + 2*item, original);
            if( val != 0 && grid(pos + item) == color)
            {
                ret += val;
            }
        }
        return ret;
    }
    
    static void grfp_transfer_mask(MaskGrid & src, MaskGrid & dst)
    {
        assert(src.Width == dst.Width && src.Height == dst.Height);
        for(int i=0; i<src.size(); i++)
            dst(i) = src(i);
    }
    
    static ValuePoint grfp_trigger_potential_point(ColorGrid & grid, MaskGrid & mask,
                                                   MaskGrid & srcMask, PathList &path,
                                                   const int pos, const int original,
                                                   bool backup = false)
    {
        ValuePoint ret;
        // invalid position
        if( pos < 0 || pos >= grid.size() ) return ret;
        if( mask(pos) ) return ret;
        
        //
        int r1 = pos / grid.Width;
        int c1 = pos % grid.Width;
        std::vector<int> dirs;
        if(r1 > 0) dirs.push_back(-grid.Width);
        if(r1 < grid.Height-1 ) dirs.push_back(grid.Width);
        if(c1 > 0) dirs.push_back(-1);
        if(c1 < grid.Width-1) dirs.push_back(1);
        for( auto & item : dirs )
        {
            int val = grfp_search(grid, pos + item, pos + 2*item, original);
            if( val != 0 )
            {
                ret.val[grid(pos + item)] += val;
                ret.neighbours[item] = std::make_pair(grid(pos + item), val);
            }
        }
        auto end = ret.neighbours.end();

        //
        int full_dirs[4] = {-1, 1, grid.Width, -grid.Width};
        for(auto item : full_dirs)
        {
            if(ret.neighbours.find(item) != end && ret.neighbours.find(-item) != end)
                if(ret.neighbours[item].first == ret.neighbours[-item].first &&
                   ret.neighbours[item].first == grid(pos))
                {
                    return ret;
                }
        }
        
        //
        for(int col=0; col<GemUtils::GemColorMax; col++)
        {
            if(ret.val[col] >= 3)
            {
                // pattern 1
                int neibCount = 0;
                for(auto item : ret.neighbours)
                    if(item.second.first == col)
                        neibCount ++;
                
                if(neibCount >= 3)
                {
                    bool flag = false;
                    for(auto & item : full_dirs)
                    {
                        if(ret.neighbours.find(item) == end && mask(pos-item) == false)
                        {
                            ret.potential_dir = -item;
                            ret.potential_count = ret.neighbours[-item].second;
                            flag = true;
                            break;
                        }
                    }
                    if(!flag)
                    {
                        for(auto item : ret.neighbours)
                            if(item.second.first != col && mask(pos-item.first) == false)
                            {
                                auto rev = ret.neighbours[-item.first];
                                ret.potential_count = rev.second;
                                ret.potential_dir = -item.first;
                                flag = true;
                                break;
                            }
                    }
                    if(flag)
                    {
                        ret.is_potential_point = true;
                        ret.potential_color = GemUtils::GemColor(col);
                        ret.pattern = 1;
                        break;
                    }
                }
                else if (neibCount == 2)
                {
                    int min = 9999, min_dir = 0;
                    for(auto & item : ret.neighbours)
                    {
                        if(item.second.first == col)
                        {
                            if(item.second.second <= min && !mask(pos+item.first))
                            {
                                min = item.second.second;
                                min_dir = item.first;
                            }
                        }
                    }
                    if(min_dir != 0)
                    {
                        ret.is_potential_point = true;
                        auto item = ret.neighbours[min_dir];
                        ret.potential_color = item.first;
                        ret.potential_dir = min_dir;
                        ret.potential_count = min;
                        ret.pattern = 1;
                        break;
                    }
                }
            }
            // pattern 2
            else if(ret.val[col] == 2 && grid(pos) == col && original != pos)
            {
                std::vector<std::pair<int, std::pair<GemUtils::GemColor,int>>> vec;
                for(auto item : ret.neighbours)
                    if(item.second.first == col && item.second.second == 1)
                        vec.push_back(item);
                
                if(vec.size() > 1)
                {
                    bool flag = false;
                    if(ret.neighbours.size() == 4)
                    {
                        for(int i=0; i<2; i++)
                        {
                            auto item = vec[i];
                            if(item.second.first == col &&
                               mask(pos+item.first) == false &&
                               mask(pos-vec[!i].first) == false)
                            {
                                path.push_back(pos-vec[!i].first);
                                path.push_back(pos);
                                path.push_back(pos+item.first);
                                
                                mask(pos) = true;
                                mask(pos+vec[!i].first) = true;
                                mask(pos+item.first) = true;
                                
                                ret.is_potential_point = true;
                                ret.potential_color = item.second.first;
                                ret.pattern = 2;
                                flag = true;
                                break;
                            }
                        }
                    }else if(ret.neighbours.size() == 3)
                    {
                        for(auto item : ret.neighbours)
                        {
                            if(item.second.first != col)
                            {
                                path.push_back(pos+item.first);
                                path.push_back(pos);
                                mask(pos) = true;
                                mask(pos-item.first) = true;
                                for(int i=0; i<2; i++)
                                {
                                    if(vec[i].first != -item.first)
                                    {
                                        path.push_back(pos + vec[i].first);
                                        mask(pos+vec[i].first) = true;
                                        break;
                                    }
                                }
                                
                                ret.is_potential_point = true;
                                ret.potential_color = grid(pos);
                                ret.pattern = 2;
                                flag = true;
                                break;
                            }
                        }
                    }
                    if(flag) break;
                }
            }
            
            // pattern 3
            if(backup)
            {
                if(ret.val[col] == 2)
                {
                    std::vector<std::pair<int, std::pair<GemUtils::GemColor,int>>> vec;
                    for(auto item : ret.neighbours)
                    {
                        if(item.second.first == col)
                        {
                            vec.push_back(item);
                        }
                    }
                    
                    if(vec.size() == 2)
                    {
                        path.push_back(pos);
                        path.push_back(pos+vec[0].first);
                        
                        mask(pos+vec[0].first) = true;
                        mask(pos+vec[1].first) = true;
                        
                        ret.is_potential_point = true;
                        ret.potential_color = grid(pos+vec[0].first);
                        ret.potential_dir = vec[0].first;
                        ret.pattern = 3;
                        
                        break;
                    }
                }
            }
        }
        
        if(ret.is_potential_point)
        {
            for(int i=0; i<path.size(); i++)
            {
                if(srcMask(path[i]))
                {
                    ret.is_potential_point = false;
                    return ret;
                }
            }
            if(ret.pattern == 1)
            {
                for(auto & item : ret.neighbours)
                {
                    if(item.second.first == ret.potential_color)
                    {
                        int count = item.second.second;
                        //if(item.first == ret.potential_dir)
                        //    count --;
                        int now = pos;
                        for(int i=0; i<count; i++)
                        {
                            now = now + item.first;
                            mask(now) = 1;
                        }
                    }
                }
                
                int count = ret.potential_count;
                int prev = pos;
                path.push_back(pos);
                while(count)
                {
                    prev += ret.potential_dir;
                    path.push_back(prev);
                    count --;
                }
            }
        }
        
        return ret;
    }
    
    static void grfp_modified_mask(MaskGrid & mask, ValuePoint val, int pos)
    {
        if(val.pattern == 1)
        {
            mask(pos) = 1;
            for(auto & item : val.neighbours)
            {
                if(item.second.first == val.potential_color)
                {
                    int count = item.second.second;
                    if(item.first == val.potential_dir)
                        count --;
                    int now = pos;
                    for(int i=0; i<count; i++)
                    {
                        now = now + item.first;
                        mask(now) = 1;
                    }
                }
            }
        }else if(val.pattern == 2)
        {
            std::vector<std::pair<int, std::pair<GemUtils::GemColor,int>>> vec;
            for(auto item : val.neighbours)
                if(item.second.first == val.potential_color && item.second.second == 1)
                    vec.push_back(item);
            
            if(vec.size() > 1)
            {
                if(val.neighbours.size() == 4)
                {
                    for(int i=0; i<2; i++)
                    {
                        auto item = vec[i];
                        if(item.second.first == val.potential_color &&
                           mask(pos+item.first) == false &&
                           mask(pos-vec[!i].first) == false)
                        {
                            mask(pos) = true;
                            mask(pos-vec[!i].first) = true;
                            mask(pos+vec[!i].first) = true;
                            
                            break;
                        }
                    }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
                }else if(val.neighbours.size() == 3)
                {
                    for(auto item : val.neighbours)
                    {
                        if(item.second.first != val.potential_color)
                        {
                            mask(pos) = true;
                            mask(pos+item.first) = true;
                            mask(pos-item.first) = true;
                            break;
                        }
                    }
                }
            }
        }else if(val.pattern == 3)
        {
            mask(pos) = true;
            for(auto item: val.neighbours)
            {
                if(item.second.first == val.potential_color &&
                   item.first != val.potential_dir)
                {
                    mask(pos+item.first) = true;
                }
            }
        }
    
    }
    
    static void grfp_first_search(ColorGrid & copyGrid, MaskGrid & mask, PathList & path)
    {
        assert(path.size() == 0);
        
    }
    
    static bool grfp_find_path_with_mask(int src, int dst, MaskGrid & mask, PathList & path)
    {
        assert(path.size() == 0);

        if(src == dst) return true;
        if(mask(src) || mask(dst)) return false;
        
        //bfs
        MaskGrid flag_map(mask.Width, mask.Height);
        std::list<int> search_seq;
        search_seq.push_back(src);
        flag_map(src) = true;
        
        MaskGrid parent_map(mask.Width, mask.Height);
        for(int i=0; i<parent_map.size(); i++) parent_map(i) = -1;
        
        bool flag = false;
        while(search_seq.size())
        {
            int pos = search_seq.front();
            search_seq.pop_front();
            
            if(pos == dst){
                flag = true;
                break;
            }
            
            int dirs[4] = {-1, 1, -mask.Width, mask.Width};
            for(auto item : dirs)
            {
                int npPos = pos + item;
                int r1 = pos / mask.Width;
                int c1 = pos % mask.Width;
                int r2 = npPos / mask.Width;
                int c2 = npPos % mask.Width;
                
                if( std::abs(r1-r2) + std::abs(c1-c2) != 1) continue;
                if( npPos <0 || npPos >=mask.size()) continue;
                if( mask(npPos) || flag_map(npPos) ) continue;

                search_seq.push_back(npPos);
                flag_map(npPos) = true;
                parent_map(npPos) = pos;
            }
        }
        
        if(flag)
        {
            int next = parent_map(dst);
            
            while(1)
            {
                if(next == -1) break;
                if(next == src) {
                    std::reverse(path.begin(), path.end());
                    return true;
                }
                path.push_back(next);
                next = parent_map(next);
            }
            return false;
        }
        return false;
    }
    
    PathList genReasonableFingerPath(GemGrid & grid, int maxCombo)
    {
        
        //grfp_first_search(copyGrid, mask, path);
        PathList final_path;
        int final_max_combo = -1;
        
        std::vector<int> seq;
        for(int i=0; i<grid.size(); i++) seq.push_back(i);
        std::random_shuffle(seq.begin(), seq.end());
        
        for(int i=0; i<grid.size(); i++)
        {
            int pos = seq[i];
            int nowCombo = 0;
            
            MaskGrid mask(grid.Width, grid.Height);
            MaskGrid footprints(grid.Width, grid.Height);
            PathList path;
            
            ColorGrid copyGrid(grid.Width, grid.Height);
            for(int i=0; i<grid.size(); i++)
                copyGrid(i) = grid(i)->color();
            
            PathList deltaPath;
            MaskGrid temp_mask(copyGrid.Width, copyGrid.Height);
            grfp_transfer_mask(mask, temp_mask);
            auto val = grfp_trigger_potential_point(copyGrid, temp_mask, mask, deltaPath, pos, -1);
            
            if(val.is_potential_point)
            {
                grfp_modified_mask(mask, val, pos);
                for(int j=0; j<deltaPath.size(); j++)
                {
                    path.push_back(deltaPath[j]);
                }
                nowCombo += 2;
            }
            else
            {
                int pos = seq[i];
                PathList deltaPath;
                MaskGrid temp_mask(copyGrid.Width, copyGrid.Height);
                grfp_transfer_mask(mask, temp_mask);
                auto val = grfp_trigger_potential_point(copyGrid, temp_mask, mask, deltaPath, pos, -1, true);
                if(val.is_potential_point)
                {
                    grfp_modified_mask(mask, val, pos);
                    for(int j=0; j<deltaPath.size(); j++)
                    {
                        path.push_back(deltaPath[j]);
                    }
                }
                nowCombo += 1;
            }
            
            if(path.size() == 0) continue;
            
            {
                for(int i=0;i<path.size(); i++)
                {
                    if(i>0)
                        std::swap(copyGrid(path[i]), copyGrid(path[i-1]));
                    footprints(path[i]) = true;
                }
            }
            
            for(float index=0; index<maxCombo; index++)
            {
                PathList shortest;
                ValuePoint target;
                int targetPos = -1;
                int minMoveSize = 9999;
                
                for(int nowPos=0; nowPos<grid.size(); nowPos++){
                    if( mask(nowPos) ) continue;
                    
                    
                    PathList goal, move;
                    //grfp_trigger_potential_point will modified temp_mask
                    MaskGrid temp_mask(grid.Width, grid.Height);
                    grfp_transfer_mask(mask, temp_mask);
                    auto val = grfp_trigger_potential_point(copyGrid, temp_mask,
                                                            mask, goal, nowPos,
                                                            path.back());
                    
                    if(val.is_potential_point)
                    {
                        if(grfp_find_path_with_mask(path.back(), goal.front(),
                                                    temp_mask, move))
                        {
                            move.insert(move.end(), goal.begin(), goal.end());
                            
                            /////////////
                            bool local_flag = true;
                            if(footprints(move[0]) && footprints(path.back()))
                                continue;
                            for(int i=1; i<move.size(); i++)
                            {
                                if(footprints(move[i]) && footprints(move[i-1]))
                                {
                                    local_flag = false;
                                    break;
                                }
                            }
                            if(!local_flag) continue;
                            /////////////
                            
                            if(move.size() < minMoveSize){
                                minMoveSize = move.size();
                                target = val;
                                targetPos = nowPos;
                                shortest = move;
                            }
                        }
                    }
                }
                
                if(targetPos != -1){
                    nowCombo+=3;
                }else
                {
                    for(int nowPos=0; nowPos<grid.size(); nowPos++)
                    {
                        if( mask(nowPos) ) continue;
                        
                        PathList goal, move;
                        
                        //grfp_trigger_potential_point will modified temp_mask
                        MaskGrid temp_mask(grid.Width, grid.Height);
                        grfp_transfer_mask(mask, temp_mask);
                        auto val = grfp_trigger_potential_point(copyGrid, temp_mask,
                                                                mask, goal, nowPos,
                                                                path.back(),
                                                                true);
                        
                        if(val.is_potential_point)
                        {
                            if(grfp_find_path_with_mask(path.back(), goal.front(),
                                                        temp_mask, move))
                            {
                                move.insert(move.end(), goal.begin(), goal.end());
                                
                                /////////////
                                bool local_flag = true;
                                if(footprints(move[0]) && footprints(path.back()))
                                    continue;
                                for(int i=1; i<move.size(); i++){
                                    if(footprints(move[i]) && footprints(move[i-1]))
                                    {
                                        local_flag = false;
                                        break;
                                    }
                                }
                                if(!local_flag) continue;
                                /////////////
                                
                                if(move.size() < minMoveSize){
                                    minMoveSize = move.size();
                                    target = val;
                                    targetPos = nowPos;
                                    shortest = move;
                                }
                            }
                        }
                    }
                    
                    if( targetPos != -1 )
                        nowCombo += 1;
                    else break;
                }
                
                if( path.size() + shortest.size() > 12 ) break;
                
                grfp_modified_mask(mask, target, targetPos);
                if(path.size()>0 && shortest.size() >0)
                    std::swap(copyGrid(path.back()), copyGrid(shortest[0]));

                for(int i=0; i<shortest.size(); i++)
                {
                    path.push_back(shortest[i]);
                    footprints(shortest[i]) = true;
                    if(i > 0)
                        std::swap(copyGrid(shortest[i]), copyGrid(shortest[i-1]));
                }
            }
            
            //printf(" combo count : %d || ",nowCombo);
        
            if(nowCombo > final_max_combo)
            {
                final_path = boost::move(path);
                final_max_combo = nowCombo;
            }
            else if(nowCombo == final_max_combo)
            {
                if(final_path.size() > path.size())
                    final_path = boost::move(path);
            }
        }
        return final_path;
    }
    
}
