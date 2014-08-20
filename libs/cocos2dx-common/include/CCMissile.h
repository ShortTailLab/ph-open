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
#ifndef __CCMissile_h__
#define __CCMissile_h__

#include "cocos2d.h"

NS_CC_BEGIN

/**
 * node is moved to another node, called aimed node. The target node
 * and aimed node must belong to same parent node. When moving, target
 * node will head to aimed node so its rotation will be adjusted according 
 * to aimed node position. This action doesn't retain aimed node, you should
 * ensure the aimed node is not released before action completed.
 *
 * because aimed node may move, so the duration of missile action can't be 
 * predicated. So it can be sequenced
 */
class CC_DLL CCMissile : public CCActionInterval {
protected:
    /// velocity
    float m_velocity;
    
	/// aimed node
	CCNode* m_aimed;
	
	/// preset degree
	float m_presetDegree;
    
    /// dest degree
    float m_dstDegree;
	
	/// done callback
	CCCallFunc* m_doneCallFunc;
	
public:
	/**
	 * static factory method
	 *
	 * @param velocity move velocity
	 * @param aimed aimed node
	 * @param targetPresetDegree a degree should be reduced when set target node rotation,
	 *		positive means clockwise
	 */
	static CCMissile* create(float velocity, CCNode* aimed, float targetPresetDegree = 0, CCCallFunc* doneCallFunc = NULL);
	
	CCMissile();
	virtual ~CCMissile();
	
	/** initializes the action */
    bool initWithVelocity(float velocity, CCNode* aimed, float targetPresetDegree, CCCallFunc* doneCallFunc);
	
	/// @see CCObject::copyWithZone
	virtual CCObject* copyWithZone(CCZone* pZone);
	
    /// @see CCAction::step
    virtual void step(float dt);
    
    /// @see CCAction::isDone
    virtual bool isDone();
};

NS_CC_END

#endif // __CCMissile_h__
