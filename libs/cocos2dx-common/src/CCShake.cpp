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
#include "CCShake.h"

NS_CC_BEGIN

CCShake::~CCShake() {
}

CCShake* CCShake::create(float duration, float radius) {
	CCShake* a = new CCShake();
	a->initWithDuration(duration, radius);
	return (CCShake*)a->autorelease();
}

bool CCShake::initWithDuration(float d, float r) {
	m_fDuration = d;
	m_radius = r;
	return true;
}

CCObject* CCShake::copyWithZone(CCZone *pZone) {
    CCZone* pNewZone = NULL;
    CCShake* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) {
        // in case of being called at sub class
        pCopy = (CCShake*)(pZone->m_pCopyObject);
    } else {
        pCopy = new CCShake();
        pZone = pNewZone = new CCZone(pCopy);
    }
	
    CCActionInterval::copyWithZone(pZone);
    pCopy->initWithDuration(m_fDuration, m_radius);
    
    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

CCActionInterval* CCShake::reverse() {
	return create(m_fDuration, m_radius);
}

void CCShake::update(float t) {
	if(t >= 1) {
		getTarget()->setPosition(m_originalX, m_originalY);
	} else {
		getTarget()->setPosition(m_originalX + m_radius * CCRANDOM_MINUS1_1(),
				m_originalY + m_radius * CCRANDOM_MINUS1_1());
	}
}

void CCShake::startWithTarget(CCNode* pTarget) {
	CCActionInterval::startWithTarget(pTarget);

	m_originalX = pTarget->getPositionX();
	m_originalY = pTarget->getPositionY();
}

NS_CC_END