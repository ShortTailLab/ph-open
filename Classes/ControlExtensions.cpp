//
//  ControlExtensions.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 12/12/12.
//
//

#include "ControlExtensions.h"
#include "gfx/vec2.h"

namespace PH
{
    LabelX* LabelX::create(const char* text,
                           const char* font,
                           float size,
                           FontEffect effect,
                           const CCPoint& offset)
    {
        LabelX* o = new LabelX();
        if (o && o->initWithString(text, font, size, effect, offset))
        {
            o->autorelease();
            return o;
        }
        
        CC_SAFE_DELETE(o);
        return o;
    }
    
    bool LabelX::initWithString(const char* text,
                                const char* font,
                                float size,
                                FontEffect effect,
                                const CCPoint& offset)
    {
        if(!CCLabelTTF::initWithString(text, font, size))
            return false;
        
        this->setColor(ccWHITE);
        this->enableShadow(offset, 1.0f, 0.0f);
        
        return true;
    }
}