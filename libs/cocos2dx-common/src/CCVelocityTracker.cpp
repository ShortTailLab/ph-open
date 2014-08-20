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
#include "CCVelocityTracker.h"
#include "VelocityTracker.h"
#include "CCUtils.h"
#include "CCMoreMacros.h"

using namespace android;

NS_CC_BEGIN

// Special constant to request the velocity of the active pointer.
static const int ACTIVE_POINTER_ID = -1;
static PointerProperties s_pp;
static PointerCoords s_pc;

static MotionEvent* convertCCTouchToMotionEvent(CCTouch* pcc, int eventMask) {
	CCPoint loc = pcc->getLocation();
	nsecs_t time = (nsecs_t)CCUtils::currentTimeMillis();
	time = milliseconds_to_nanoseconds(time);
	s_pp.id = pcc->getID();
	MotionEvent* me = new MotionEvent();
	me->initialize(0,
				   0,
				   eventMask,
				   0,
				   0,
				   0,
				   0,
				   loc.x,
				   loc.y,
				   loc.x,
				   loc.y,
				   time,
				   time,
				   1,
				   &s_pp,
				   &s_pc);
	return me;
}

class VelocityTrackerState {
public:
    VelocityTrackerState(const char* strategy);
	
    void clear();
    void addMovement(const MotionEvent* event);
    void computeCurrentVelocity(int32_t units, float maxVelocity);
    void getVelocity(int32_t id, float* outVx, float* outVy);
    bool getEstimator(int32_t id, VelocityTracker::Estimator* outEstimator);
	
private:
    struct Velocity {
        float vx, vy;
    };
	
    VelocityTracker mVelocityTracker;
    int32_t mActivePointerId;
    BitSet32 mCalculatedIdBits;
    Velocity mCalculatedVelocity[MAX_POINTERS];
};

VelocityTrackerState::VelocityTrackerState(const char* strategy) :
mVelocityTracker(strategy), mActivePointerId(-1) {
}

void VelocityTrackerState::clear() {
    mVelocityTracker.clear();
    mActivePointerId = -1;
    mCalculatedIdBits.clear();
}

void VelocityTrackerState::addMovement(const MotionEvent* event) {
    mVelocityTracker.addMovement(event);
}

void VelocityTrackerState::computeCurrentVelocity(int32_t units, float maxVelocity) {
    BitSet32 idBits(mVelocityTracker.getCurrentPointerIdBits());
    mCalculatedIdBits = idBits;
	
    for (uint32_t index = 0; !idBits.isEmpty(); index++) {
        uint32_t id = idBits.clearFirstMarkedBit();
		
        float vx, vy;
        mVelocityTracker.getVelocity(id, &vx, &vy);
		
        vx = vx * units / 1000;
        vy = vy * units / 1000;
		
        if (vx > maxVelocity) {
            vx = maxVelocity;
        } else if (vx < -maxVelocity) {
            vx = -maxVelocity;
        }
        if (vy > maxVelocity) {
            vy = maxVelocity;
        } else if (vy < -maxVelocity) {
            vy = -maxVelocity;
        }
		
        Velocity& velocity = mCalculatedVelocity[index];
        velocity.vx = vx;
        velocity.vy = vy;
    }
}

void VelocityTrackerState::getVelocity(int32_t id, float* outVx, float* outVy) {
    if (id == ACTIVE_POINTER_ID) {
        id = mVelocityTracker.getActivePointerId();
    }
	
    float vx, vy;
    if (id >= 0 && id <= MAX_POINTER_ID && mCalculatedIdBits.hasBit(id)) {
        uint32_t index = mCalculatedIdBits.getIndexOfBit(id);
        const Velocity& velocity = mCalculatedVelocity[index];
        vx = velocity.vx;
        vy = velocity.vy;
    } else {
        vx = 0;
        vy = 0;
    }
	
    if (outVx) {
        *outVx = vx;
    }
    if (outVy) {
        *outVy = vy;
    }
}

bool VelocityTrackerState::getEstimator(int32_t id, VelocityTracker::Estimator* outEstimator) {
    return mVelocityTracker.getEstimator(id, outEstimator);
}

CCVelocityTracker::CCVelocityTracker() {
	m_state = new VelocityTrackerState(NULL);
}

CCVelocityTracker::~CCVelocityTracker() {
	delete m_state;
}

CCVelocityTracker* CCVelocityTracker::create() {
	CCVelocityTracker* t = new CCVelocityTracker();
	return (CCVelocityTracker*)t->autorelease();
}

void CCVelocityTracker::addTouchBegan(CCTouch* event) {
	MotionEvent* me = convertCCTouchToMotionEvent(event, AMOTION_EVENT_ACTION_DOWN);
	m_state->clear();
	m_state->addMovement(me);
	delete me;
}

void CCVelocityTracker::addTouchMoved(CCTouch* event) {
	MotionEvent* me = convertCCTouchToMotionEvent(event, AMOTION_EVENT_ACTION_MOVE);
	m_state->addMovement(me);
	delete me;
}

void CCVelocityTracker::computeCurrentVelocity(int units) {
	m_state->computeCurrentVelocity(units, MAX_FLOAT);
}

void CCVelocityTracker::computeCurrentVelocity(int units, float maxVelocity) {
	m_state->computeCurrentVelocity(units, maxVelocity);
}

float CCVelocityTracker::getXVelocity() {
	float vx;
	m_state->getVelocity(ACTIVE_POINTER_ID, &vx, NULL);
	return vx;
}

float CCVelocityTracker::getYVelocity() {
	float vy;
	m_state->getVelocity(ACTIVE_POINTER_ID, NULL, &vy);
	return vy;
}

float CCVelocityTracker::getXVelocity(int id) {
	float vx;
	m_state->getVelocity(id, &vx, NULL);
	return vx;
}

float CCVelocityTracker::getYVelocity(int id) {
	float vy;
	m_state->getVelocity(id, NULL, &vy);
	return vy;
}

NS_CC_END