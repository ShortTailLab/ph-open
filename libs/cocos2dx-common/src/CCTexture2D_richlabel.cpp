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
#include "CCTexture2D_richlabel.h"
#include "CCImage_richlabel.h"

NS_CC_BEGIN

CCTexture2D_richlabel::CCTexture2D_richlabel() {
}

CCTexture2D_richlabel::~CCTexture2D_richlabel() {
}

const char* CCTexture2D_richlabel::description(void) {
    return CCString::createWithFormat("<CCTexture2D_richlabel | Name = %u | Dimensions = %u x %u | Coordinates = (%.2f, %.2f)>", m_uName, m_uPixelsWide, m_uPixelsHigh, m_fMaxS, m_fMaxT)->getCString();
}

bool CCTexture2D_richlabel::initWithRichString(const char *text, const char *fontName, float fontSize) {
    return initWithRichString(text,  fontName, fontSize, CCSizeMake(0,0), kCCTextAlignmentCenter, kCCVerticalTextAlignmentTop);
}

bool CCTexture2D_richlabel::initWithRichString(const char *text, const char *fontName, float fontSize, const CCSize& dimensions, CCTextAlignment hAlignment, CCVerticalTextAlignment vAlignment) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    
	ccFontDefinition tempDef;
	
	tempDef.m_shadow.m_shadowEnabled = false;
	tempDef.m_stroke.m_strokeEnabled = false;
	
	
	tempDef.m_fontName      = std::string(fontName);
	tempDef.m_fontSize      = fontSize;
	tempDef.m_dimensions    = dimensions;
	tempDef.m_alignment     = hAlignment;
	tempDef.m_vertAlignment = vAlignment;
	tempDef.m_fontFillColor = ccWHITE;
    
	return initWithString(text, &tempDef);
	
#else
    
#if CC_ENABLE_CACHE_TEXTURE_DATA
	// cache the texture data
    VolatileTexture::addStringTexture(this, text, textDefinition->m_dimensions, textDefinition->m_alignment, textDefinition->m_vertAlignment, textDefinition->m_fontName.c_str(), textDefinition->m_fontSize);
#endif
	
	bool bRet = false;
	CCImage::ETextAlign eAlign;
	
	if (kCCVerticalTextAlignmentTop == vAlignment)
	{
		eAlign = (kCCTextAlignmentCenter == hAlignment) ? CCImage::kAlignTop
		: (kCCTextAlignmentLeft == hAlignment) ? CCImage::kAlignTopLeft : CCImage::kAlignTopRight;
	}
	else if (kCCVerticalTextAlignmentCenter == vAlignment)
	{
		eAlign = (kCCTextAlignmentCenter == hAlignment) ? CCImage::kAlignCenter
		: (kCCTextAlignmentLeft == hAlignment) ? CCImage::kAlignLeft : CCImage::kAlignRight;
	}
	else if (kCCVerticalTextAlignmentBottom == vAlignment)
	{
		eAlign = (kCCTextAlignmentCenter == hAlignment) ? CCImage::kAlignBottom
		: (kCCTextAlignmentLeft == hAlignment) ? CCImage::kAlignBottomLeft : CCImage::kAlignBottomRight;
	}
	else
	{
		CCAssert(false, "Not supported alignment format!");
		return false;
	}
	
	do
	{
		CCImage* pImage = new CCImage();
		CC_BREAK_IF(NULL == pImage);
		bRet = pImage->initWithString(text, (int)dimensions.width, (int)dimensions.height, eAlign, fontName, (int)fontSize);
		CC_BREAK_IF(!bRet);
		bRet = initWithImage(pImage);
		CC_SAFE_RELEASE(pImage);
	} while (0);
    
    
	return bRet;
    
#endif
}

bool CCTexture2D_richlabel::initWithRichString(const char *text, ccRichFontDefinition *textDefinition) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    
#if CC_ENABLE_CACHE_TEXTURE_DATA
	// cache the texture data
//    VolatileTexture::addStringTexture(this, text, textDefinition->m_dimensions, textDefinition->m_alignment, textDefinition->m_vertAlignment, textDefinition->m_fontName.c_str(), textDefinition->m_fontSize);
#endif
	
	bool bRet = false;
	CCImage::ETextAlign eAlign;
	
	if (kCCVerticalTextAlignmentTop == textDefinition->m_vertAlignment)
	{
		eAlign = (kCCTextAlignmentCenter == textDefinition->m_alignment) ? CCImage::kAlignTop
		: (kCCTextAlignmentLeft == textDefinition->m_alignment) ? CCImage::kAlignTopLeft : CCImage::kAlignTopRight;
	}
	else if (kCCVerticalTextAlignmentCenter == textDefinition->m_vertAlignment)
	{
		eAlign = (kCCTextAlignmentCenter == textDefinition->m_alignment) ? CCImage::kAlignCenter
		: (kCCTextAlignmentLeft == textDefinition->m_alignment) ? CCImage::kAlignLeft : CCImage::kAlignRight;
	}
	else if (kCCVerticalTextAlignmentBottom == textDefinition->m_vertAlignment)
	{
		eAlign = (kCCTextAlignmentCenter == textDefinition->m_alignment) ? CCImage::kAlignBottom
		: (kCCTextAlignmentLeft == textDefinition->m_alignment) ? CCImage::kAlignBottomLeft : CCImage::kAlignBottomRight;
	}
	else
	{
		CCAssert(false, "Not supported alignment format!");
		return false;
	}
	
	// handle shadow parameters
	bool  shadowEnabled =  false;
	float shadowDX      = 0.0;
	float shadowDY      = 0.0;
	float shadowBlur    = 0.0;
    int shadowColor     = 0;
	
	if ( textDefinition->m_shadow.m_shadowEnabled )
	{
		shadowEnabled =  true;
		shadowDX      = textDefinition->m_shadow.m_shadowOffset.width;
		shadowDY      = textDefinition->m_shadow.m_shadowOffset.height;
		shadowBlur    = textDefinition->m_shadow.m_shadowBlur;
        shadowColor   = textDefinition->m_shadowColor;
	}
	
	// handle stroke parameters
	bool strokeEnabled = false;
	float strokeColorR = 0.0;
	float strokeColorG = 0.0;
	float strokeColorB = 0.0;
	float strokeSize   = 0.0;
	
	if ( textDefinition->m_stroke.m_strokeEnabled )
	{
		strokeEnabled = true;
		strokeColorR = textDefinition->m_stroke.m_strokeColor.r / 255;
		strokeColorG = textDefinition->m_stroke.m_strokeColor.g / 255;
		strokeColorB = textDefinition->m_stroke.m_strokeColor.b / 255;
		strokeSize   = textDefinition->m_stroke.m_strokeSize;
	}
	
	CCImage_richlabel* pImage = new CCImage_richlabel();
	do
	{
		CC_BREAK_IF(NULL == pImage);
		
		bRet = pImage->initWithRichStringShadowStroke(text,
												  (int)textDefinition->m_dimensions.width,
												  (int)textDefinition->m_dimensions.height,
												  eAlign,
												  textDefinition->m_fontName.c_str(),
												  textDefinition->m_fontSize,
												  textDefinition->m_fontFillColor.r / 255.0f,
												  textDefinition->m_fontFillColor.g / 255.0f,
												  textDefinition->m_fontFillColor.b / 255.0f,
												  shadowEnabled,
												  shadowDX,
												  shadowDY,
                                                  shadowColor,
												  shadowBlur,
												  strokeEnabled,
												  strokeColorR,
												  strokeColorG,
												  strokeColorB,
												  strokeSize);
		
#if CC_ENABLE_CACHE_TEXTURE_DATA
        // cache the texture data
        VolatileTexture::addCCImage(this, pImage);
#endif
		
		CC_BREAK_IF(!bRet);
		bRet = initWithImage(pImage);
		
	} while (0);
	
	CC_SAFE_RELEASE(pImage);
	
	return bRet;
    
    
#else
    
	CCAssert(false, "Currently only supported on iOS and Android!");
	return false;
    
#endif
}

NS_CC_END
