/****************************************************************************
 Copyright (c) 2012 cocos2d-x.org
 Copyright (c) 2010 Sangwoo Im
 
 http://www.cocos2d-x.org
 
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
#ifndef __CCVelocityTracker__
#define __CCVelocityTracker__

#include "cocos2d.h"

NS_CC_BEGIN

class VelocityTrackerState;

/**
 * Velocity tracker
 */
class CC_DLL CCVelocityTracker : public CCObject {
private:
	// state
	VelocityTrackerState* m_state;
	
protected:
	CCVelocityTracker();
	
public:
	virtual ~CCVelocityTracker();
	static CCVelocityTracker* create();
	
    /**
     * Add a user's movement to the tracker.  You should call this for the
     * initial touch began event, the following touch moved events that you receive
     *
     * @param event The CCTouch you received and would like to track.
     */
	void addTouchBegan(CCTouch* event);
	
	/**
     * Add a touch moved event to the tracker.
     *
     * @param event The CCTouch you received and would like to track.
     */
	void addTouchMoved(CCTouch* event);
	
	/**
     * Equivalent to invoking {@link #computeCurrentVelocity(int, float)} with a maximum
     * velocity of max float value
     *
     * @see #computeCurrentVelocity(int, float)
     */
	void computeCurrentVelocity(int units);
	
	/**
     * Compute the current velocity based on the points that have been
     * collected.  Only call this when you actually want to retrieve velocity
     * information, as it is relatively expensive.  You can then retrieve
     * the velocity with {@link #getXVelocity()} and
     * {@link #getYVelocity()}.
     *
     * @param units The units you would like the velocity in.  A value of 1
     * provides pixels per millisecond, 1000 provides pixels per second, etc.
     * @param maxVelocity The maximum velocity that can be computed by this method.
     * This value must be declared in the same unit as the units parameter. This value
     * must be positive.
     */
    void computeCurrentVelocity(int units, float maxVelocity);
	
	/**
     * Retrieve the last computed X velocity.  You must first call
     * {@link #computeCurrentVelocity(int)} before calling this function.
     *
     * @return The previously computed X velocity.
     */
    float getXVelocity();
    
    /**
     * Retrieve the last computed Y velocity.  You must first call
     * {@link #computeCurrentVelocity(int)} before calling this function.
     *
     * @return The previously computed Y velocity.
     */
    float getYVelocity();
	
	/**
     * Retrieve the last computed X velocity.  You must first call
     * {@link #computeCurrentVelocity(int)} before calling this function.
     *
     * @param id Which pointer's velocity to return.
     * @return The previously computed X velocity.
     */
    float getXVelocity(int id);
    
    /**
     * Retrieve the last computed Y velocity.  You must first call
     * {@link #computeCurrentVelocity(int)} before calling this function.
     *
     * @param id Which pointer's velocity to return.
     * @return The previously computed Y velocity.
     */
    float getYVelocity(int id);
};

NS_CC_END

#endif /* defined(__CCVelocityTracker__) */
