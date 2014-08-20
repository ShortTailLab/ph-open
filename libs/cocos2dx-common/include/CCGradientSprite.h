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
#ifndef __CCGradientSprite_h__
#define __CCGradientSprite_h__

#include "cocos2d.h"

NS_CC_BEGIN

/// sprite can show a gradient effect
class CC_DLL CCGradientSprite : public CCSprite {
private:
    /// update color
    void updateColor();
    
public:
	/**
     * Default constructor
     */
    CCGradientSprite();
    
    /**
     * Default destructor
     */
    virtual ~CCGradientSprite();
	
	/// @{
    /// @name Creators
    
    /**
     * Creates an empty sprite without texture. You can call setTexture method subsequently.
     *
     * @return An empty sprite object that is marked as autoreleased.
     */
    static CCGradientSprite* create();
    
    /**
     * Creates a sprite with an image filename.
     *
     * After creation, the rect of sprite will be the size of the image,
     * and the offset will be (0,0).
     *
     * @param   pszFileName The string which indicates a path to image file, e.g., "scene1/monster.png".
     * @return  A valid sprite object that is marked as autoreleased.
     */
    static CCGradientSprite* create(const char *pszFileName);
    
    /**
     * Creates a sprite with an image filename and a rect.
     *
     * @param   pszFileName The string wich indicates a path to image file, e.g., "scene1/monster.png"
     * @param   rect        Only the contents inside rect of pszFileName's texture will be applied for this sprite.
     * @return  A valid sprite object that is marked as autoreleased.
     */
    static CCGradientSprite* create(const char *pszFileName, const CCRect& rect);
    
    /**
     * Creates a sprite with an exsiting texture contained in a CCTexture2D object
     * After creation, the rect will be the size of the texture, and the offset will be (0,0).
     *
     * @param   pTexture    A pointer to a CCTexture2D object.
     * @return  A valid sprite object that is marked as autoreleased.
     */
    static CCGradientSprite* createWithTexture(CCTexture2D *pTexture);
    
    /**
     * Creates a sprite with a texture and a rect.
     *
     * After creation, the offset will be (0,0).
     *
     * @param   pTexture    A pointer to an existing CCTexture2D object.
     *                      You can use a CCTexture2D object for many sprites.
     * @param   rect        Only the contents inside the rect of this texture will be applied for this sprite.
     * @return  A valid sprite object that is marked as autoreleased.
     */
    static CCGradientSprite* createWithTexture(CCTexture2D *pTexture, const CCRect& rect);
    
    /**
     * Creates a sprite with an sprite frame.
     *
     * @param   pSpriteFrame    A sprite frame which involves a texture and a rect
     * @return  A valid sprite object that is marked as autoreleased.
     */
    static CCGradientSprite* createWithSpriteFrame(CCSpriteFrame *pSpriteFrame);
    
    /**
     * Creates a sprite with an sprite frame name.
     *
     * A CCSpriteFrame will be fetched from the CCSpriteFrameCache by pszSpriteFrameName param.
     * If the CCSpriteFrame doesn't exist it will raise an exception.
     *
     * @param   pszSpriteFrameName A null terminated string which indicates the sprite frame name.
     * @return  A valid sprite object that is marked as autoreleased.
     */
    static CCGradientSprite* createWithSpriteFrameName(const char *pszSpriteFrameName);
    
    /// @}  end of creators group
    
    /// @{
    /// @name Functions inherited from CCNodeRGBA
    virtual void setColor(const ccColor3B& color3);
    virtual void updateDisplayedColor(const ccColor3B& parentColor);
    virtual void setOpacity(GLubyte opacity);
    virtual void setOpacityModifyRGB(bool modify);
    virtual void updateDisplayedOpacity(GLubyte parentOpacity);
    /// @}
    
    bool isCompressedInterpolation() {
        return getCompressedInterpolation();
    }
    
    // set gradient
    virtual void setColor(const ccColor4B& start, const ccColor4B& end, const CCPoint& v);
    
    CC_PROPERTY_PASS_BY_REF(ccColor3B, m_startColor, StartColor)
    CC_PROPERTY_PASS_BY_REF(ccColor3B, m_endColor, EndColor)
    CC_PROPERTY(GLubyte, m_cStartOpacity, StartOpacity)
    CC_PROPERTY(GLubyte, m_cEndOpacity, EndOpacity)
    CC_PROPERTY_PASS_BY_REF(CCPoint, m_AlongVector, Vector)
    CC_PROPERTY(bool, m_bCompressedInterpolation, CompressedInterpolation);
};

NS_CC_END

#endif // __CCGradientSprite_h__