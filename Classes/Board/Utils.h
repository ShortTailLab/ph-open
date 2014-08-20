//
//  Utils.h
//  HelloCpp
//
//  Created by Zeyang Li on 11/16/12.
//
//

#pragma once

#include <cocos2d.h>
#include "CCScale9Sprite.h"
#include "gfx/vec2.h"

#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/functional/hash.hpp>
#include <vector>

namespace PH
{
    USING_NS_CC;
    USING_NS_CC_EXT;

    inline CCPoint boundCenter(const CCRect& b)
    {
        return ccp(b.origin.x + b.size.width*0.5f, b.origin.y + b.size.height*0.5f);
    }

    inline CCPoint boundTopLeft(const CCRect& b)
    {
        return ccp(b.origin.x, b.origin.y + b.size.height);
    }

    inline CCPoint boundTopRight(const CCRect& b)
    {
        return ccp(b.origin.x + b.size.width, b.origin.y + b.size.height);
    }

    inline CCPoint boundBottomLeft(const CCRect& b)
    {
        return b.origin;
    }

    inline CCPoint boundBottomRight(const CCRect& b)
    {
        return ccp(b.origin.x + b.size.width, b.origin.y);
    }


    inline ccBezierConfig ccbz(CCPoint a, CCPoint b, CCPoint c)
    {
        ccBezierConfig config;
        config.controlPoint_1 = a;
        config.controlPoint_2 = b;
        config.endPosition = c;

        return config;
    }

    template <typename T>
    std::vector<T> filter(std::vector<T>& l, boost::function<bool (T)> predicate)
    {
        std::vector<T> ll;
        for(int i=0; i<l.size(); i++)
        {
            if(predicate(l[i]))
                ll.push_back(l[i]);
        }
        return ll;
    }

    // Random number generators
    inline float randf()
    {
        return (float)rand() * (1.0f / (float)RAND_MAX);
    }

    inline float randf(float max)
    {
        return randf() * max;
    }

    inline float randf(float min, float max)
    {
        return randf(max - min) + min;
    }

    inline Vec2f randomUnitVec2()
    {
        Vec2f v(randf() - 0.5f, randf() - 0.5f);
        unitize(v);
        return v;
    }

    inline Vec2f randomBand(float min, float max)
    {
        Vec2f normal = randomUnitVec2();
        return normal * randf(min, max);
    }

    // CCPoint wrappers
    inline CCPoint ccRandomUnitVec2()
    {
        Vec2f v = randomUnitVec2();
        return ccp(v[0], v[1]);
    }

    inline CCPoint ccRandomBand(float min, float max)
    {
        Vec2f val = randomBand(min, max);
        return ccp(val[0], val[1]);
    }

    inline bool chance(float percentage)
    {
        return randf() < percentage;
    }

    template <typename T>
    inline std::string toStr(T v)
    {
        return boost::lexical_cast<std::string>(v);
    }

    template <typename T>
    inline float toFloat(T v)
    {
        return boost::lexical_cast<float>(v);
    }

    inline int toInt(std::string v)
    {
        return boost::lexical_cast<int>(v);
    }

    inline float contentScale()
    {
        return CCDirector::sharedDirector()->getContentScaleFactor();
    }

    inline CCScene* runningScene()
    {
        return CCDirector::sharedDirector()->getRunningScene();
    }

    inline std::vector<Vec2f> circlePoints(int count, float radius)
    {
        std::vector<Vec2f> points;

        float delta = M_PI * 2.0f / count;
        float angle = 0.0f;

        for(int i=0; i<count; ++i)
        {
            float x = cosf(angle);
            float y = sinf(angle);

            angle += delta;

            Vec2f p = Vec2f(x,y) * radius;
            points.push_back(p);
        }
        return points;
    }

    template<typename T>
    T clampx(T val, T min, T max)
    {
        return std::min(std::max(val, min), max);
    }

    inline CCPoint getWorldPosition(CCNode* n)
    {
        if(n->getParent())
            return n->getParent()->convertToWorldSpace(n->getPosition());
        else
            return n->getPosition();
    }

    inline CCFiniteTimeAction* AnimPair(CCActionInterval* a)
    {
        return CCSequence::create(a, a->reverse(), NULL);
    }

    inline CCScale9Sprite* Get9SpriteFromFrame(const char* file, float x, float y, float width, float height)
    {
        // spirit bar
		CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(file);
//        float offx = frame->getRect().origin.x;
//        float offy = frame->getRect().origin.y;

		return CCScale9Sprite::createWithSpriteFrame(frame, CCRect(x, y, width, height));
    }

    inline CCScale9Sprite* Get9SpriteFromFrame(const char* file, CCRect r)
    {
        return Get9SpriteFromFrame(file, r.origin.x, r.origin.y, r.size.width, r.size.height);
    }

    // this correctly handles negative numbers
    inline int trueMod(int x, int m)
    {
        return (x % m + m) % m;
    }

    inline CCPoint inWorldSpace(CCNode* node)
    {
        if(node->getParent())
            return node->getParent()->convertToWorldSpace(node->getPosition());
        return node->getPosition();
    }
}
