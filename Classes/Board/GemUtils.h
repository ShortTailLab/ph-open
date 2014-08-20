//
//  GemUtils.h
//  HelloCpp
//
//  Created by 毛 靖凯 on 13-9-2.
//
//

#ifndef HelloCpp_GemUtils_h
#define HelloCpp_GemUtils_h

#include <cocos2d.h>
#include <gfx/vec2.h>
#include <boost/lexical_cast.hpp>

using namespace cocos2d;

namespace PH
{

    const static float skGemPixelWidth = 107;
    const static float skGemPixelHeight = 107;
    
    // grid to world and vice versa
    inline CCPoint g2w(const Vec2i& p)
    {
        return ccp(p[0] * skGemPixelWidth,
                   p[1] * skGemPixelHeight);
    }
    
    inline CCPoint g2w_center(const Vec2i & p)
    {
        return ccp(p[0] * skGemPixelWidth + skGemPixelWidth/2,
                   p[1] * skGemPixelHeight + skGemPixelHeight/2);
    }
    
    inline Vec2i w2g(const CCPoint& p)
    {
        return Vec2i(floorf(p.x / skGemPixelWidth),
                     floorf(p.y / skGemPixelHeight));
    }
    
    inline CCPoint roundToGrid(CCPoint& p){  return g2w(w2g(p)); }
    
    struct GemUtils {  // tolua_export
        // tolua_begin
        enum GemColor
        {
            // DO NOT change order of these values.
            // Frst value MUST start from 0.
            // AllColor MUST immediately follow the last gem color.
            // Health must be the last among all actual gem colors.
            Fire = 0,
            Water = 1,
            Wood = 2,
            Dark = 3,
            Light = 4,
            Health = 5,
            AllColor = 6,
            GemColorMax = 7,
        };
        // tolua_end
        
        static inline bool isEqualColorOrAllColor(GemColor col, GemColor tarCol){
            if(col == tarCol || col == GemColor::AllColor) return true;
            return false;
        }

        
        static std::string getNameWithPatternAndColor(std::string patter,
                                                      GemColor color);
        static const char* res(GemColor c);  // tolua_export
        static const char* frame(GemColor c);  // tolua_export
        static const char* name(GemColor c); // tolua_export
        static std::string nameCN(GemColor c); // tolua_export
        static const char* numberFont(GemColor c); //tolua_export
        static const char* resistanceFont(GemColor c);
        static const char* healthBox(GemColor c); // tolua_export
        static const char* healthFill(GemColor c); // tolua_export
        
        static const char* healthBox(GemColor c, bool isLong);
        static const char* healthFill(GemColor c, bool isLong);
        
        static const char* attackSound(GemColor c);
        static GemUtils::GemColor strToColor(std::string str);
        static const char* colorToStr(GemColor c);
        static CCSpriteFrame* energy(GemColor c, int val);
        static const char* attackAnim(GemColor c);
        static const char* stageColorRes(GemColor c);
        static ccColor3B colorToCCColor(GemColor c);
        static inline std::string i(GemColor c){
            return boost::lexical_cast<std::string>((int)c);
        }
        
        static GemUtils::GemColor weak(GemColor l);
        static inline bool strongvsweak(GemColor l, GemColor r){
            return weak(l) == r;
        }
        static inline int compare(GemUtils::GemColor l, GemUtils::GemColor r) {
            // order of comparison should not change, consider Fire vs Light
            if(strongvsweak(l, r)) return 1;
            else if(strongvsweak(r, l)) return -1;
            else
                return 0;
        }
        
        static inline float modifier(GemUtils::GemColor l, GemUtils::GemColor r) {
            int cmp = compare(l,r);
            if(cmp > 0) return 2.0f;
            else if(cmp < 0) return 0.5f;
            else return 1.0f;
        }
    }; //tolua_export
    
}

#endif
