//
//  ControlExtensions.h
//  HelloCpp
//
//  Created by Zeyang Li on 12/12/12.
//
//

#pragma once

#include "cocos2d.h"

namespace PH
{
    USING_NS_CC;
    
    class LabelX : public CCLabelTTF
    {        
    public:
        enum FontEffect
        {
            SHADOW,
        };
        
        static LabelX* create(const char* text,
                              const char* font,
                              float size,
                              FontEffect effect = SHADOW,
                              const CCPoint& offset = CCPoint(3, -3));
        
        bool initWithString(const char* text,
                            const char* font,
                            float size,
                            FontEffect effect,
                            const CCPoint& offset);
    };
}