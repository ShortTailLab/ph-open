/*
 * Copyright (c) 2010 WiYun Inc.
 * Author: luma(stubma@gmail.com)
 *
 * For all entities this program is free software; you can redistribute
 * it and/or modify it under the terms of the 'WiEngine' license with
 * the additional provision that 'WiEngine' must be credited in a manner
 * that can be be observed by end users, for example, in the credits or during
 * start up. (please find WiEngine logo in sdk's logo folder)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef __CCScroller_h__
#define __CCScroller_h__

#include "cocos2d.h"

NS_CC_BEGIN

/**
 * it calculates scrolling position and do nothing without UI. It is a
 * great helper class for scroll or fling implmementation.
 */
class CC_DLL CCScroller : public CCObject {
private:
    /**
     * friction coefficient, default is 0.015
     */
	float m_scrollFriction;

	/**
	 * current mode, scroll or fling
	 */
	int m_mode;

	/**
	 * start x position
	 */
	float m_startX;

	/**
	 * start y position
	 */
	float m_startY;

	/**
	 * end x position
	 */
	float m_finalX;

	/**
	 * end y position
	 */
	float m_finalY;

	/**
	 * minimum x position
	 */
	float m_minX;

	/**
	 * maximum x position
	 */
	float m_maxX;

	/**
	 * minimum y position
	 */
	float m_minY;

	/**
	 * maximum y position
	 */
	float m_maxY;

	/**
	 * current x position
	 */
	float m_currX;

	/**
	 * current y position
	 */
	float m_currY;

	/**
	 * start time in millisecond
	 */
	int m_startTime;

	/**
	 * fling duration time
	 */
	int m_duration;

	/**
	 * reciprocal of duartion time
	 */
	float m_durationReciprocal;

	/**
	 * x offset to scroll
	 */
	float m_deltaX;

	/**
	 * y offset to scroll
	 */
	float m_deltaY;

	/**
	 * viscous fluid coefficient
	 */
	float m_viscousFluidScale;

	/**
	 * viscous fluid normalization value
	 */
	float m_viscousFluidNormalize;

	/**
	 * is scroll or fling finished
	 */
	bool m_finished;

	/**
	 * proportion of x velocity and total velocity
	 */
	float m_coeffX;

	/**
	 * proportion of y velocity and total velocity
	 */
	float m_coeffY;

	/**
	 * velocity of fling
	 */
	float m_velocity;

	/**
	 * deceleration of fling
	 */
	float m_deceleration;

private:
	/**
	 * based on viscous fluid coefficient, convert linear time to non-linear
	 * time
	 *
	 * @param x from 0 to 1, means a linear time range
	 * @return the non-linear time converted
	 */
    float viscousFluid(float x);

public:
    static CCScroller* create();

	CCScroller();
	virtual ~CCScroller();

    /**
     * is scroll or fling finished
     *
     * @return true means scroll or fling finished
     */
    bool isFinished() const { return m_finished; }

    /**
     * force scrolling or flinging finish
     *
     * @param finished true means finish scrolling or flinging, or false if not
     */
    void forceFinished(bool finished) { m_finished = finished; }

    /**
     * return the duration time of scrolling
     *
     * @return scroll duration, in millisceond
     */
    int getDuration() const { return m_duration; }

    /**
     * return current x position
     *
     * @return current x position
     */
    int getCurrX() const { return m_currX; }

    /**
     * return current y position
     *
     * @return current y position
     */
    int getCurrY() const { return m_currY; }

    /**
     * get current velocity
     *
     * @return current velocity, maybe negative
     */
    float getCurrVelocity() const { return m_velocity - m_deceleration * timePassed() / 2000.0f; }

    /**
     * get current velocity
     *
     * @return current velocity, maybe negative
     */
    float getStartX() const { return m_startX; }

    /**
     * get start y position
     *
     * @return start y position
     */
    float getStartY() const { return m_startY; }

    /**
     * get final x position, only used for flinging
     *
     * @return final x position of flinging
     */
    float getFinalX() const { return m_finalX; }

    /**
     * get final y position of flinging
     *
     * @return final y position of flinging
     */
    float getFinalY() const { return m_finalY; }

    /**
     * get current scrolling position
     *
     * @return true means scrolling is not over, false means finished
     */
    bool computeScrollOffset();

    /**
     * start scrolling, duration time is 0.25 second
     *
     * @param startX start x position
     * @param startY start y position
     * @param dx x offset to scroll
     * @param dy y offset to scroll
     */
    void startScroll(float startX, float startY, float dx, float dy);

    /**
     * start scrolling
     *
     * @param startX start x position
     * @param startY start y position
     * @param dx x offset to scroll
     * @param dy y offset to scroll
     * @param duration duration time of scroll
     */
    void startScroll(float startX, float startY, float dx, float dy, int duration);

    /**
     * start fling. duration time of fling is computed.
     *
     * @param startX start x position
     * @param startY start y position
     * @param velocityX initial x velocity
     * @param velocityY initial y velocity
     * @param minX minimum x position
     * @param maxX maximum x position
     * @param minY minimum y position
     * @param maxY maximum y position
     */
    void fling(float startX, float startY, float velocityX, float velocityY,
    		float minX, float maxX, float minY, float maxY);

    /**
     * stop fling
     */
    void abortAnimation();

    /**
     * extend duration time of scrolling
     *
     * @param extend time to be extended
     */
    void extendDuration(int extend);

    /**
     * get time passed from scrolling start to now, in millisecond
     *
     * @return time passed from scrolling start to now, in millisecond
     */
    int timePassed() const;

    /**
     * set final x position
     *
     * @param newX final x position
     */
    void setFinalX(float newX);

    /**
     * set final y position
     *
     * @param newY final y position
     */
    void setFinalY(float newY);

    /**
     * friction coefficient, default is 0.0.15
     *
     * @param friction friction coefficient
     */
    void setScrollFriction(float friction) { m_scrollFriction = friction; }
};

NS_CC_END

#endif // __CCScroller_h__
