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
#include "CCTreeFadeOut.h"
#include "CCTreeFadeIn.h"

NS_CC_BEGIN

CCTreeFadeOut* CCTreeFadeOut::create(float d) {
    CCTreeFadeOut* pAction = new CCTreeFadeOut();
    pAction->initWithDuration(d);
    pAction->autorelease();
    return pAction;
}

void CCTreeFadeOut::update(float time) {
    CCFadeOut::update(time);
    
    fadeOutRecursively(getTarget(), time);
}

void CCTreeFadeOut::fadeOutRecursively(CCNode* n, float time) {
    CCArray* children = n->getChildren();
    int cc = n->getChildrenCount();
    for(int i = 0; i < cc; i++) {
        CCNode* child = (CCNode*)children->objectAtIndex(i);
        CCRGBAProtocol* p = dynamic_cast<CCRGBAProtocol*>(child);
        if(p) {
            p->setOpacity((GLubyte)(255 * (1 - time)));
        }
        
        fadeOutRecursively(child, time);
    }
}

CCActionInterval* CCTreeFadeOut::reverse(void) {
    return CCTreeFadeIn::create(m_fDuration);
}

NS_CC_END