#pragma once

#include <cocos2d.h>
#include <gfx/vec2.h>

//#include <extensions/utils.h>
//#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <boost/lexical_cast.hpp>
#include <boost/enable_shared_from_this.hpp>

//#include "BuffGroup.h"

#include "SerializationUtils.h"
#include "Task.h"
#include "GemUtils.h"

USING_NS_CC;

namespace PH { // tolua_export
    
    class BoardControl;
    
    class Gem;
    typedef boost::shared_ptr<Gem> GemPtr;
    
    typedef boost::function<TaskPtr(Gem * host)> GemHook;
    typedef boost::function<TaskPtr(Gem * host, bool & isRemoved)> GemOnSweepHook;
    
    class Gem : public boost::enable_shared_from_this<Gem>, IObject
    {
    protected:
        GemUtils::GemColor  mColor;
        int                 mTurn;
        
        CCLabelBMFont*      mCountdown;
        virtual void        attachCountdown();
        virtual TaskPtr     updateCountdown();
        
    public:
        static const size_t kGemWidthPixel, kGemHeightPixel;
        CCSprite*           root;
        Vec2i               position;
        
        // --- IObject ---
        Gem() : root(NULL), mCountdown(NULL), mTurn(-1) {}
        ~Gem();
        
        INITIALIZE_OBJECT();
        virtual void vSerialize(std::ostringstream& out) const
        {
            out << (int)mColor << mTurn;
        }
        virtual void vDeserialize(std::istringstream& in)
        {
            int col, turn;
            in >> col >> turn;
            this->init(static_cast<GemUtils::GemColor>(col), turn);
        }
        
        virtual void vSerialize(rapidjson::Value& o, rapidjson::Document::AllocatorType& alloc) const;
        virtual void vDeserialize(rapidjson::Value& o);
        
        static GemPtr make(GemUtils::GemColor col, int turn=-1)
        {
            GemPtr ret(new Gem);
            if(ret && !ret->init(col, turn))
                ret.reset();
            return ret;
        }
        virtual bool init(GemUtils::GemColor color, int turn);
        
        // --- accessor ---
        virtual GemUtils::GemColor color() const { return mColor; }
        //virtual Vec2i position() const { return position; }
        
        // --- animation presets ---
        virtual TaskPtr fadeOut(float time=0.f);
        virtual void    setOpacity(size_t a);
        virtual TaskPtr fadeIn(float time=0.f);
        virtual TaskPtr modifyDisplayFrameWithPatternAndColor(const std::string & pattern,
                                                              GemUtils::GemColor col,
                                                              float time=0.f);
        virtual TaskPtr transformColor(GemUtils::GemColor toColor, float time=0.f);
        virtual TaskPtr moveTo(const Vec2i & dst, float time=0.f);
        
        // --- game relavants ---
        virtual bool isModifiable() const { return true; }
        virtual bool isMoveable() const { return true; }
        
        virtual TaskPtr sweep(BoardControl* ctrl);
        virtual TaskPtr update(BoardControl* ctrl);
    };

    typedef std::set<GemPtr> GemSet;
    
    bool isSameColorNeighbor(const GemPtr l, const GemPtr r);
    bool isInOrNeighborSameColorGemSet(const GemSet& set, const GemPtr gem);
    GemSet intersectGemSet(const GemSet& l, const GemSet& r);
    
    // --- special gems ---
    class HandGem;
    typedef boost::shared_ptr<HandGem> HandGemPtr;
    
    class HandGem : public Gem
    {
    protected:
        CCSprite*   mBackground;
        float       mDamage;
        
        //
        virtual TaskPtr explosion();

    public:
        virtual ~HandGem(){}
        INITIALIZE_OBJECT();
        virtual void vSerialize(std::ostringstream& out) const
        {
            out << (int)mColor << mTurn << mDamage;
        }
        virtual void vDeserialize(std::istringstream& in)
        {
            int col, turn, damage;
            in >> col >> turn >> damage;
            this->init(static_cast<GemUtils::GemColor>(col),  turn, damage);
        }
        
        static HandGemPtr make(GemUtils::GemColor col, int turn, float damage)
        {
            HandGemPtr ret(new HandGem);
            if(ret && !ret->init(col, turn, damage))
                ret.reset();
            return ret;
        }
        
        virtual bool init(GemUtils::GemColor col, int turn, float damage);
        virtual TaskPtr update(BoardControl* ctrl);
        
        virtual TaskPtr transformColor(GemUtils::GemColor toColor, float time=0.f);
        virtual bool isModifiable() const { return false; }
    };
    
    class BombGem;
    typedef boost::shared_ptr<BombGem> BombGemPtr;
    
    class BombGem : public Gem
    {
    protected:
        CCSprite*   mFuze;
        float       mDamage;
        
        virtual TaskPtr explosion();
        
    public:
        virtual ~BombGem(){}
        INITIALIZE_OBJECT();
        virtual void vSerialize(std::ostringstream& out) const
        {
            out << (int)mColor<< mTurn << mDamage;
        }
        virtual void vDeserialize(std::istringstream& in)
        {
            int col, turn, damage;
            in >> col >> turn >> damage;
            this->init(static_cast<GemUtils::GemColor>(col), turn, damage);
        }
        
        static BombGemPtr make(GemUtils::GemColor col, int turn, float damage)
        {
            BombGemPtr ret(new BombGem);
            if(ret && !ret->init(col, turn, damage))
                ret.reset();
            return ret;
        }
        virtual bool init(GemUtils::GemColor col, int turn, float damage);
        virtual TaskPtr update(BoardControl* ctrl);
        
        virtual TaskPtr transformColor(GemUtils::GemColor toColor, float time=0.f);
        virtual bool isModifiable() const { return false; }
    };
    
    class LockedGem;
    typedef boost::shared_ptr<LockedGem> LockedGemPtr;
    
    class LockedGem : public Gem
    {
    protected:
        CCSprite*   mChain;
        
        TaskPtr detachChain(CCNode* node);
        
    public:
        virtual ~LockedGem(){}
        
        INITIALIZE_OBJECT();
        virtual void vSerialize(std::ostringstream& out) const
        {
            out << (int)mColor<< mTurn;
        }
        virtual void vDeserialize(std::istringstream& in)
        {
            int col, turn;
            in >> col >> turn;
            this->init(static_cast<GemUtils::GemColor>(col), turn);
        }
        
        static LockedGemPtr make(GemUtils::GemColor col, int turn)
        {
            LockedGemPtr ret(new LockedGem);
            if(ret && !ret->init(col, turn))
                ret.reset();
            return ret;
        }
        
        virtual bool init(GemUtils::GemColor col, int turn);
        
        virtual TaskPtr sweep(BoardControl* ctrl);
        virtual TaskPtr update(BoardControl* ctrl);
        
        virtual bool isModifiable() const { return false; }
        virtual bool isMoveable() const { return false; }
    };
    
    class PlagueGem;
    typedef boost::shared_ptr<PlagueGem> PlagueGemPtr;
    
    class PlagueGem : public Gem
    {
    protected:
        float       mDamage;
        int         mGlobalLimits, mLocalLimits, mMaxTurn;
        
        bool        mReady;
        
    public:
        virtual ~PlagueGem(){}
        
        INITIALIZE_OBJECT();
        virtual void vSerialize(std::ostringstream& out) const
        {
            out << (int)mColor << mTurn << mDamage << mGlobalLimits << mLocalLimits;
        }
        virtual void vDeserialize(std::istringstream& in)
        {
            int col, turn, globalLimits, localLimits;
            float damage;
            in >> col >> turn >> damage >> globalLimits >> localLimits;
            this->init(static_cast<GemUtils::GemColor>(col), turn,
                       damage, globalLimits, localLimits);
        }
        
        static PlagueGemPtr make(GemUtils::GemColor col, int turn,
                                 float damage,
                                 int globalLimits, int localLimits)
        {
            PlagueGemPtr ret(new PlagueGem);
            if(ret && !ret->init(col, turn, damage, globalLimits, localLimits))
                ret.reset();
            return ret;
        }
        
        virtual bool init(GemUtils::GemColor col, int turn,
                          float damage,
                          int globalLimits, int localLimits);
        
        virtual TaskPtr update(BoardControl* ctrl);
        virtual TaskPtr fadeIn(float time=0.f);
        
        virtual TaskPtr transformColor(GemUtils::GemColor toColor, float time=0.f);
        virtual bool isModifiable() const { return false; }
    };
    
    class ShieldGem;
    typedef boost::shared_ptr<ShieldGem> ShieldGemPtr;
    
    class ShieldGem : public Gem
    {
    protected:
        TaskPtr detachShield(CCNode* node);
        
    public:
        virtual ~ShieldGem(){}
        
        INITIALIZE_OBJECT();
        virtual void vSerialize(std::ostringstream& out) const
        {
            out << (int)mColor << mTurn ;
        }
        virtual void vDeserialize(std::istringstream& in)
        {
            int col, turn;
            this->init(static_cast<GemUtils::GemColor>(col), turn);
        }
        
        static ShieldGemPtr make(GemUtils::GemColor color,
                                 int turn)
        {
            ShieldGemPtr ret(new ShieldGem);
            if(ret && !ret->init(color, turn))
                ret.reset();
            return ret;
        }
        
        virtual bool init(GemUtils::GemColor color, int turn);
        virtual TaskPtr update(BoardControl* ctrl);
        
        virtual TaskPtr transformColor(GemUtils::GemColor toColor, float time=0.f);
        virtual bool isModifiable() const { return false; }
    };
    
} // tolua_export
