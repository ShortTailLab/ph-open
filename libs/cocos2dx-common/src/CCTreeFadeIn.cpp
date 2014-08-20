/****************************************************************************
 Author: Luma (stubma@gmail.com)
 
 https://github.com/stubma/cocos2dx-common
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "CCTreeFadeIn.h"
#include "CCTreeFadeOut.h"

NS_CC_BEGIN

CCTreeFadeIn* CCTreeFadeIn::create(float d) {
    CCTreeFadeIn* pAction = new CCTreeFadeIn();
    pAction->initWithDuration(d);
    pAction->autorelease();
    return pAction;
}

void CCTreeFadeIn::update(float time) {
    CCFadeIn::update(time);
    
    fadeInRecursively(getTarget(), time);
}

void CCTreeFadeIn::fadeInRecursively(CCNode* n, float time) {
    CCArray* children = n->getChildren();
    int cc = n->getChildrenCount();
    for(int i = 0; i < cc; i++) {
        CCNode* child = (CCNode*)children->objectAtIndex(i);
        CCRGBAProtocol* p = dynamic_cast<CCRGBAProtocol*>(child);
        if(p) {
            p->setOpacity((GLubyte)(255 * time));
        }
        
        fadeInRecursively(child, time);
    }
}

CCActionInterval* CCTreeFadeIn::reverse(void) {
    return CCTreeFadeOut::create(m_fDuration);
}

NS_CC_END