//
//  StatusLabel.h
//  HelloCpp
//
//  Created by Zeyang Li on 11/30/12.
//
//

#pragma once

#include <string>

#include "cocos2d.h"
#include "Board/Task.h"
#include "Common.h"
#include "GemUtils.h"
#include <boost/enable_shared_from_this.hpp>

USING_NS_CC;

namespace PH
{
    //enum GemUtils::GemColor;
    class StatusLabel : public boost::enable_shared_from_this<StatusLabel>
    {
    private:
        CCNode* root;
        CCSprite* bg;
        CCLabelBMFont* textLabel;
        
    public:
        StatusLabel();
        virtual ~StatusLabel();

        void setPosition(CCPoint p) { root->setPosition(p); }
        CCPoint getPosition() const { return root->getPosition(); }
        
        void setTextAndColor(std::string text, GemUtils::GemColor c);
        
        TaskPtr show();
        TaskPtr hide();
        
        void showRaw();
        void hideAndRemoveRaw();
        
        CCNode* getRoot() { return root; }
    };
    
    typedef boost::shared_ptr<StatusLabel> StatusLabelPtr;
    
    class StatusLabelGroup
    {
    private:
        std::vector<StatusLabelPtr> statusSlots;
        boost::function<CCPoint(int)> evalPosition;
        
    public:
        StatusLabelGroup(boost::function<CCPoint (int)> eval) : evalPosition(eval) {}
        
        TaskPtr setLabelWithAnim(CCLayer* layer,
                                 StatusLabelPtr label,
                                 std::string text,
                                 GemUtils::GemColor color,
                                 int zorder = ORDER_MASK+1);

        TaskPtr clearLabelWithAnim(StatusLabelPtr label, bool occupy=false);
    };
}
