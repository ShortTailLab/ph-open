//
//  BuffGroup.h
//  HelloCpp
//
//  Created by 毛 靖凯 on 13-8-29.
//
//

#ifndef HelloCpp_BuffGroup_h
#define HelloCpp_BuffGroup_h

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/move/move.hpp>

#include "Task.h"
#include "StatusLabel.h"
#include "SerializationUtils.h"

namespace PH{
    
    // --- passive buff ---
    // kind of flag during game
    class IPassiveBuff;
    

    // --- generic buff with data ---
    class IBuff;
    typedef boost::shared_ptr<IBuff> BuffPtr;
    typedef boost::function<TaskPtr (IBuff*)> OnTurnCallback;
    
    class IBuff : public IObject
    {
    protected:
        int             mTurnLeft;
        int             mColor;
        StatusLabelPtr  mLabel;
        std::string     mText;
        
    public:
        
        static BuffPtr make(GemUtils::GemColor color,
                            int turn,
                            const std::string& text)
        {
            BuffPtr ret(new IBuff);
            if(ret && ret->init(static_cast<int>(color), turn, text))
                return ret;
            return BuffPtr();
        }
        virtual bool init(int color, int turn, const std::string& text)
        {
            mColor = color;
            mTurnLeft = turn;
            mLabel = StatusLabelPtr(new StatusLabel);
            mText = text;
            return true;
        }
        
        // nothing happped here
        INITIALIZE_OBJECT();
        virtual void vSerialize(std::ostringstream& out) const
        {
            out << mTurnLeft << mColor << mText;
        }
        virtual void vDeserialize(std::istringstream& in)
        {
            in >> mTurnLeft >> mColor >> mText;
        }
        
        virtual void vSerialize(rapidjson::Value& o, rapidjson::Document::AllocatorType& alloc) const {};
        virtual void vDeserialize(rapidjson::Value& o) {};
        
        TaskPtr update();
        TaskPtr attach();
        
        inline bool isActive() const { return mTurnLeft > 0; }
        inline int getTurnLeft() const { return mTurnLeft; }
        inline GemUtils::GemColor getColor() const { return static_cast<GemUtils::GemColor>(mColor); }
        inline StatusLabelPtr getLabel() { return mLabel; }
        inline std::string getText() const { return mText; }
    };
    // --- end ---
    
    typedef std::map<std::string, std::pair<size_t, BuffPtr>> BuffMap;
    
    // --- buff group ---
    class BuffGroup : public IObject
    {
    private:
        BuffMap                         mBuffMap;
        std::vector<StatusLabelPtr>     mStatusSlots;
        boost::function<CCPoint(int)>   mEvalPosition;
        
    public:
        
        virtual ~BuffGroup();
        // fundamentials
        INITIALIZE_OBJECT();
        virtual void vSerialize(std::ostringstream& out) const;
        virtual void vDeserialize(std::istringstream& in);
        
        virtual void vSerialize(rapidjson::Value& o, rapidjson::Document::AllocatorType& alloc) const {};
        virtual void vDeserialize(rapidjson::Value& o) {};
        
        TaskPtr attachBuff(const size_t groupId, const std::string& key, CCNode* parent,
                           BuffPtr buff);
        TaskPtr detachBuff(const std::string& key);
        TaskPtr updateBuff(const std::string & key);
        TaskPtr updateBuff(const size_t groupId);
        
        inline bool isBuffActive(const std::string& key) const
        {
            return mBuffMap.find(key) != mBuffMap.end();
        }
        inline bool isBuffLastTurn(const std::string& key) const
        {
            auto findIt = mBuffMap.find(key);
            if(findIt != mBuffMap.end() &&
               findIt->second.second->getTurnLeft() == 1) return true;
            return false;
        }
        
        template<class Task>
        inline bool invokeBuff(const std::string& key, Task t)
        {
            auto findIt = mBuffMap.find(key);
            if(findIt == mBuffMap.end()) return false;
            if(mEvalPosition)
                t << this->createLabelAnim(findIt->second.second->getLabel());
            return true;
        }
        
        template<class Task>
        inline bool invokeBuffWhen(const std::string& key,
                                   boost::shared_ptr<Task> t,
                                   GemUtils::GemColor col)
        {
            auto findIt = mBuffMap.find(key);
            if( findIt == mBuffMap.end() ) return false;
            auto buff = findIt->second.second;
            if( buff->getColor() != GemUtils::AllColor && buff->getColor() != col ) return false;
            if( mEvalPosition )
                t << this->createLabelAnim(findIt->second.second->getLabel());
            return true;
        }
        
        template<class Task>
        inline bool invokeBuffWhen(const std::string& key,
                                   boost::shared_ptr<Task> t,
                                   boost::function<bool (BuffPtr)> eval)
        {
            auto findIt = mBuffMap.find(key);
            if( findIt == mBuffMap.end() ) return false;
            auto buff = findIt->second.second;
            if( !eval(buff) ) return false;
            if( mEvalPosition )
                t << this->createLabelAnim(findIt->second.second->getLabel());
            return true;
        }
        
        // Type here needs to be a subclass of IBuff
        template<class Type>
        boost::shared_ptr<Type> getBuffContent(const std::string & key)
        {
            auto findIt = mBuffMap.find(key);
            if(findIt == mBuffMap.end())
                return boost::shared_ptr<Type>();
            
            return boost::dynamic_pointer_cast<Type>(findIt->second.second);
        }
        
        inline void setEvalFunction(boost::function<CCPoint(int)> evalProc)
        {
            this->mEvalPosition = boost::move(evalProc);
        }
        
    protected:
        // --- anim presets ---
        TaskPtr setLabelWithAnim(CCNode* layer, BuffPtr buff);
        TaskPtr createLabelAnim(StatusLabelPtr label);
        TaskPtr clearLabelWithAnim(StatusLabelPtr label);
    };
    
    // helper for registering globally
    template<class SubClass>
    class BuffFactoryReg
    {
    public:
        BuffFactoryReg(int key)
        {
            IntObjectFactory::getInstance()->reg<SubClass>(key);
        }
    };
#define REG_BUFF(Type, Id) \
    BuffFactoryReg<Type>(Id);
    // --- end ---

    // --- collection of buff ---
    template<class Type>
    class Buff1 : public IBuff
    {
    public:
        Type d1;
        
        static boost::shared_ptr<Buff1<Type>> make(GemUtils::GemColor color,
                            int turn,
                            const std::string& text,
                            Type t1)
        {
            boost::shared_ptr<Buff1<Type>> ret(new Buff1<Type>);
            if(ret && ret->init(static_cast<int>(color), turn, text, t1))
                return ret;
            return boost::shared_ptr<Buff1<Type>>();
        }
        
        virtual bool init(int color, int turn, const std::string& text, Type t1)
        {
            mColor = color;
            mTurnLeft = turn;
            mLabel = StatusLabelPtr(new StatusLabel);
            mText = text;
            d1 = t1;
            return true;
        }
        
        INITIALIZE_OBJECT();
        virtual void vSerialize(std::ostringstream& out) const
        {
            IBuff::vSerialize(out);
            out << d1;
        }
        
        virtual void vDeserialize(std::istringstream& in)
        {
            IBuff::vDeserialize(in);
            in >> d1;
        }
        
        virtual void vSerialize(rapidjson::Value& o, rapidjson::Document::AllocatorType& alloc) const {};
        virtual void vDeserialize(rapidjson::Value& o) {};
        
    };
    
    typedef Buff1<float> FloatBuff;
    typedef boost::shared_ptr<FloatBuff> FloatBuffPtr;
    
    typedef Buff1<int> IntBuff;
    typedef boost::shared_ptr<IntBuff> IntBuffPtr;
    
    typedef Buff1<GemUtils::GemColor> ColorBuff;
    typedef boost::shared_ptr<ColorBuff> ColorBuffPtr;
    
    typedef Buff1<bool> BoolBuff;
    typedef boost::shared_ptr<BoolBuff> BoolBuffPtr;
    
    template<class Type1, class Type2>
    class Buff2 : public IBuff
    {
    public:
        Type1 d1;
        Type2 d2;
        
        static boost::shared_ptr<Buff2<Type1, Type2>> make(GemUtils::GemColor color,
                                                           int turn,
                                                           const std::string& text,
                                                           Type1 t1,
                                                           Type2 t2)
        {
            boost::shared_ptr<Buff2<Type1, Type2>> ret(new Buff2<Type1, Type2>);
            if(ret && ret->init(static_cast<int>(color), turn, text, t1, t2))
                return ret;
            return boost::shared_ptr<Buff2<Type1, Type2>>();
        }
        
        virtual bool init(int color, int turn, const std::string& text, Type1 t1, Type2 t2)
        {
            mColor = color;
            mTurnLeft = turn;
            mLabel = StatusLabelPtr(new StatusLabel);
            mText = text;
            d1 = t1; d2 = t2;
            return true;
        }
        
        INITIALIZE_OBJECT();
        virtual void vSerialize(std::ostringstream& out) const
        {
            IBuff::vSerialize(out);
            out << d1 << d2;
        }
        virtual void vDeserialize(std::istringstream& in)
        {
            IBuff::vDeserialize(in);
            in >> d1 >> d2;
        }
        
        virtual void vSerialize(rapidjson::Value& o, rapidjson::Document::AllocatorType& alloc) const {};
        virtual void vDeserialize(rapidjson::Value& o) {};
    };
    
    typedef Buff2<float, float>                 Float2Buff;
    typedef boost::shared_ptr<Float2Buff>       Float2BuffPtr;
    
    typedef Buff2<float, int>                   FloatIntBuff;
    typedef boost::shared_ptr<FloatIntBuff>     FloatIntBuffPtr;
    
    typedef Buff2<int, int>                     Int2Buff;
    typedef boost::shared_ptr<Int2Buff>         Int2BuffPtr;
    
    typedef Buff2<float, bool>                  FloatBoolBuff;
    typedef boost::shared_ptr<FloatBoolBuff>    FloatBoolBuffPtr;
    
    typedef Buff2<int, bool>                    IntBoolBuff;
    typedef boost::shared_ptr<IntBoolBuff>      IntBoolBuffPtr;
    
    template<class Type1, class Type2, class Type3>
    class Buff3 : public IBuff
    {
    public:
        Type1 d1;
        Type2 d2;
        Type3 d3;
        
        static boost::shared_ptr<Buff3<Type1, Type2, Type3>> make(GemUtils::GemColor color,
                                                                  int turn,
                                                                  const std::string& text,
                                                                  Type1 t1,
                                                                  Type2 t2,
                                                                  Type3 t3)
        {
            boost::shared_ptr<Buff3<Type1, Type2, Type3>> ret(new Buff3<Type1, Type2, Type3>);
            if(ret && ret->init(static_cast<int>(color), turn, text, t1, t2, t3))
                return ret;
            return boost::shared_ptr<Buff3<Type1, Type2, Type3>>();
        }
        
        virtual bool init(int color, int turn, const std::string& text,
                          Type1 t1, Type2 t2, Type3 t3)
        {
            mColor = color;
            mTurnLeft = turn;
            mLabel = StatusLabelPtr(new StatusLabel);
            mText = text;
            d1 = t1; d2 = t2; d3 = t3;
            return true;
        }
        
        INITIALIZE_OBJECT();
        virtual void vSerialize(std::ostringstream& out) const
        {
            IBuff::vSerialize(out);
            out << d1 << d2 << d3;
        }
        virtual void vDeserialize(std::istringstream& in)
        {
            IBuff::vDeserialize(in);
            in >> d1 >> d2 >> d3;
        }
        
        virtual void vSerialize(rapidjson::Value& o, rapidjson::Document::AllocatorType& alloc) const {};
        virtual void vDeserialize(rapidjson::Value& o) {};
    };
    
    typedef Buff3<float, float, float> Float3Buff;
    typedef boost::shared_ptr<Float3Buff> Float3BuffPtr;
    
    typedef Buff3<float, float, bool> FLoat2BoolBuff;
    typedef boost::shared_ptr<FLoat2BoolBuff> Float2BoolBuffPtr;
    
    typedef Buff3<float, float, int> Float2IntBuff;
    typedef boost::shared_ptr<Float2IntBuff> Float2IntPtr;
    // --- end ---
}
#endif