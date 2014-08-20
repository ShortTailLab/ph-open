//
//  CalUtils.cpp
//  HelloCpp
//
//  Created by zql on 13-6-7.
//
//

#include "PvPUtils.h"

namespace PH
{
    bool GridArrow::init()
    {
        if(!CCNode::init())
            return false;
        return true;
    }
    
    void GridArrow::clear()
    {
        path.clear();
        this->removeAllChildren();
    }
    
    static bool eq(const Vec2i& a, const Vec2i& b)
    {
        return a[0] == b[0] && a[1] == b[1];
    }
    
    static float angleByDirDiff(const Vec2i& dirDiff)
    {
        if(eq(dirDiff, Vec2i(-1, -1)))
            ;
        else if(eq(dirDiff, Vec2i(-1, 1)))
            return 90.0f;
        else if(eq(dirDiff, Vec2i(1, 1)))
            return 180.0f;
        else if(eq(dirDiff, Vec2i(1, -1)))
            return -90.0f;
        return 0.0f;
    }
    
    static float angleByDir(const Vec2i& delta)
    {
        if(eq(delta, Vec2i(-1, 0)))
            return 0.0f;
        else if(eq(delta, Vec2i(0, 1)))
            return 90.0f;
        else if(eq(delta, Vec2i(0, -1)))
            return -90.0f;
        else if(eq(delta, Vec2i(1, 0)))
            return 180.0f;
        return 0.0f;
    }
    
    void GridArrow::addPoint(const Vec2i& p)
    {
        // keep only 3 points in the path
        if(path.size() == 3)
        {
            path[0] = path[1];
            path[1] = path[2];
            path[2] = p;
        }
        else
            path.push_back(p);
        
        if(path.size() < 2)
            return;
        
        this->removeAllChildren();
        
        // compute the direction array
        std::vector<Vec2i> dirPath;
        for(int i=0; i<path.size()-1; i++)
            dirPath.push_back(path[i+1]-path[i]);
        dirPath.push_back(dirPath.back());
        
        for(int i=0; i<path.size(); i++)
        {
            CCSprite * arrow = NULL;
            
            // if a turn
            if(i != path.size()-1 && i != 0 && !eq(dirPath[i], dirPath[i-1]))
            {
                arrow = GetSprite("jiantou_2.png");
                Vec2i dirDiff = dirPath[i] - dirPath[i-1];
                if(eq(dirDiff, Vec2i(-1, -1)))
                    ;
                else if(eq(dirDiff, Vec2i(-1, 1)))
                    arrow->setRotation(90);
                else if(eq(dirDiff, Vec2i(1, 1)))
                    arrow->setRotation(180);
                else if(eq(dirDiff, Vec2i(1, -1)))
                    arrow->setRotation(-90);
            }
            else
            {
                if(i==0)
                    arrow = GetSprite("jiantou_3.png");
                else if(i == path.size() - 1)
                    arrow = GetSprite("jiantou_1.png");
                else
                    arrow = GetSprite("jiantou_4.png");
                
                Vec2i delta = dirPath[i];
                if(eq(delta, Vec2i(-1, 0)))
                    ;
                else if(eq(delta, Vec2i(0, 1)))
                    arrow->setRotation(90);
                else if(eq(delta, Vec2i(0, -1)))
                    arrow->setRotation(-90);
                else if(eq(delta, Vec2i(1, 0)))
                    arrow->setRotation(180);
            }
            
            arrow->setAnchorPoint(ccp(0.5f, 0.5f));
            arrow->setPosition(g2w_center(path[i]));
            this->addChild(arrow);
        }
    }
    
    std::string getTitleOfRepu(int reputation)
    {
        std::string title = "";
        if(reputation < 1200)
            title = "无名小卒";
        else if (reputation < 1400)
            title = "声名鹊起";
        else if (reputation < 1600)
            title = "武林高手";
        else if (reputation < 1800)
            title = "独步天下";
        else if (reputation < 2000)
            title = "一代大侠";
        else if (reputation < 2200)
            title = "绝代宗师";
        else if (reputation < 2500)
            title = "笑傲江湖";
        else
            title = "武林神话";
        return title;
    }

}