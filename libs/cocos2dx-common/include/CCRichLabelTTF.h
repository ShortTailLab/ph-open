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
#ifndef __CCRichLabelTTF__
#define __CCRichLabelTTF__

#include "cocos2d.h"
#include "CCGradientSprite.h"
#include "ccMoreTypes.h"

NS_CC_BEGIN

class CCTexture2D_richlabel;

/**
 * TTF label which support different style in a string. By using a tag description such as:
 * 'Hell[color=ffff0000]o[/color]", the 'o' character will be in red color. If you want 
 * a '[' character, use '\' to escape.
 *
 * \par
 * supported tags:
 * [color=aarrggbb][/color]: set text color
 * [font=font name or path][/font]: set font
 * [size=integer][/size]: set font size
 * [b][/b]: bold font
 * [i][/i]: italic font
 * [u][/u]: underline
 *
 * \par
 * CCRichLabelTFF is a subclass of CCGradientSprite, so it also can set a gradient
 * effect.
 *
 * \note
 * Currently it only supports iOS and Android, please do it yourself if you want other platform.
 */
class CC_DLL CCRichLabelTTF : public CCGradientSprite, public CCLabelProtocol {
public:
	CCRichLabelTTF();
    virtual ~CCRichLabelTTF();
	
	/** 
	 * Creates an label.
     */
    static CCRichLabelTTF* create();
	
	/**
	 * creates a CCRichLabelTTF with a font name and font size in points
     */
    static CCRichLabelTTF* create(const char *string, const char *fontName, float fontSize);
    
    /** 
	 * creates a CCRichLabelTTF from a fontname, horizontal alignment, dimension in points,  and font size in points.
     */
    static CCRichLabelTTF* create(const char *string, const char *fontName, float fontSize,
                               const CCSize& dimensions, CCTextAlignment hAlignment);
	
    /** 
	 * creates a CCRichLabelTTF from a fontname, alignment, dimension in points and font size in points
     */
    static CCRichLabelTTF* create(const char *string, const char *fontName, float fontSize,
                               const CCSize& dimensions, CCTextAlignment hAlignment,
                               CCVerticalTextAlignment vAlignment);
	
	const char* description();
	
    /** initializes the CCRichLabelTTF with a font name and font size */
    bool initWithString(const char *string, const char *fontName, float fontSize);
    
    /** initializes the CCRichLabelTTF with a font name, alignment, dimension and font size */
    bool initWithString(const char *string, const char *fontName, float fontSize,
                        const CCSize& dimensions, CCTextAlignment hAlignment);
	
    /** initializes the CCRichLabelTTF with a font name, alignment, dimension and font size */
    bool initWithString(const char *string, const char *fontName, float fontSize,
                        const CCSize& dimensions, CCTextAlignment hAlignment,
                        CCVerticalTextAlignment vAlignment);
    
    /** initializes the CCRichLabelTTF with a font name, alignment, dimension and font size */
    bool initWithStringAndTextDefinition(const char *string, ccRichFontDefinition &textDefinition);
    
    /** set the text definition used by this label */
    void setTextDefinition(ccRichFontDefinition *theDefinition);
    
    /** get the text definition used by this label */
    ccRichFontDefinition * getTextDefinition();
    
    
    
    /** enable or disable shadow for the label */
    void enableShadow(const CCSize &shadowOffset, int shadowColor, float shadowBlur, bool mustUpdateTexture = true);
    
    /** disable shadow rendering */
    void disableShadow(bool mustUpdateTexture = true);
    
    /** enable or disable stroke */
    void enableStroke(const ccColor3B &strokeColor, float strokeSize, bool mustUpdateTexture = true);
    
    /** disable stroke */
    void disableStroke(bool mustUpdateTexture = true);
    
    /** set text tinting */
    void setFontFillColor(const ccColor3B &tintColor, bool mustUpdateTexture = true);

    /** initializes the CCRichLabelTTF */
    bool init();
	
    /** changes the string to render
	 * @warning Changing the string is as expensive as creating a new CCRichLabelTTF. To obtain better performance use CCLabelAtlas
	 */
    virtual void setString(const char *label);
    virtual const char* getString(void);
    
    CCTextAlignment getHorizontalAlignment();
    void setHorizontalAlignment(CCTextAlignment alignment);
    
    CCVerticalTextAlignment getVerticalAlignment();
    void setVerticalAlignment(CCVerticalTextAlignment verticalAlignment);
    
    CCSize getDimensions();
    void setDimensions(const CCSize &dim);
    
    float getFontSize();
    void setFontSize(float fontSize);
    
    const char* getFontName();
    void setFontName(const char *fontName);
	
	virtual void setColor(const ccColor3B& color3) {}
    
    virtual void setColor(const ccColor4B& start, const ccColor4B& end, const CCPoint& v);
    
private:
    bool updateTexture();
	
protected:
    
    /** set the text definition for this label */
    void                _updateWithTextDefinition(ccRichFontDefinition & textDefinition, bool mustUpdateTexture = true);
    ccRichFontDefinition    _prepareTextDefinition(bool adjustForResolution = false);
    
    /** Dimensions of the label in Points */
    CCSize m_tDimensions;
    /** The alignment of the label */
    CCTextAlignment         m_hAlignment;
    /** The vertical alignment of the label */
    CCVerticalTextAlignment m_vAlignment;
    /** Font name used in the label */
    std::string * m_pFontName;
    /** Font size of the label */
    float m_fFontSize;
    /** label's string */
    std::string m_string;
    
    /** font shadow */
    bool    m_shadowEnabled;
    CCSize  m_shadowOffset;
    float   m_shadowOpacity;
    float   m_shadowBlur;
    int     m_shadowColor;
    
    
    /** font stroke */
    bool        m_strokeEnabled;
    ccColor3B   m_strokeColor;
    float       m_strokeSize;
	
    /** font tint */
    ccColor3B   m_textFillColor;
};

NS_CC_END

#endif /* defined(__CCRichLabelTTF__) */
