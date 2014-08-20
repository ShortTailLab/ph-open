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
#ifndef __CCCalendar__
#define __CCCalendar__

#include "cocos2d.h"

NS_CC_BEGIN

/// calendar c++ wrapper
class CC_DLL CCCalendar : public CCObject {
private:
    /// singleton
    static CCCalendar* s_instance;
    
    /// time, in seconds
    float m_time;
    
protected:
    CCCalendar();
    
public:
    virtual ~CCCalendar();
    static CCCalendar* sharedCalendar();
    
    /// get time, since 1970-1-1
    float getTime() { return m_time; }
    
    /// set time
    void setTime(float time) { m_time = time; }
    
    /// set time of now
    void setNow();
    
    /// get year
    int getYear();
    
    // get month, 1 for January
    int getMonth();
    
    // get day, 1 for first day
    int getDay();
    
    // get day of week, 1 for sunday
    int getWeekday();
    
    // get hour
    int getHour();
    
    // get minute
    int getMinute();
    
    // get second
    int getSecond();
};

NS_CC_END

#endif /* defined(__CCCalendar__) */
