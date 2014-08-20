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
#ifndef __CCAntiArtifactSprite_h__
#define __CCAntiArtifactSprite_h__

#include "cocos2d.h"

NS_CC_BEGIN

/**
 * sprite which enables anti-artifact by default. To use
 * this sprite so that you don't need to turn on the compile flag
 * in cocos2d-x config, which affects everything.
 */
class CC_DLL CCAntiArtifactSprite : public CCSprite {
public:
	/**
     * Default constructor
     */
    CCAntiArtifactSprite();
    
    /**
     * Default destructor
     */
    virtual ~CCAntiArtifactSprite();
	
	/// @{
    /// @name Creators
    
    /**
     * Creates an empty sprite without texture. You can call setTexture method subsequently.
     *
     * @return An empty sprite object that is marked as autoreleased.
     */
    static CCAntiArtifactSprite* create();
    
    /**
     * Creates a sprite with an image filename.
     *
     * After creation, the rect of sprite will be the size of the image,
     * and the offset will be (0,0).
     *
     * @param   pszFileName The string which indicates a path to image file, e.g., "scene1/monster.png".
     * @return  A valid sprite object that is marked as autoreleased.
     */
    static CCAntiArtifactSprite* create(const char *pszFileName);
    
    /**
     * Creates a sprite with an image filename and a rect.
     *
     * @param   pszFileName The string wich indicates a path to image file, e.g., "scene1/monster.png"
     * @param   rect        Only the contents inside rect of pszFileName's texture will be applied for this sprite.
     * @return  A valid sprite object that is marked as autoreleased.
     */
    static CCAntiArtifactSprite* create(const char *pszFileName, const CCRect& rect);
    
    /**
     * Creates a sprite with an exsiting texture contained in a CCTexture2D object
     * After creation, the rect will be the size of the texture, and the offset will be (0,0).
     *
     * @param   pTexture    A pointer to a CCTexture2D object.
     * @return  A valid sprite object that is marked as autoreleased.
     */
    static CCAntiArtifactSprite* createWithTexture(CCTexture2D *pTexture);
    
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
    static CCAntiArtifactSprite* createWithTexture(CCTexture2D *pTexture, const CCRect& rect);
    
    /**
     * Creates a sprite with an sprite frame.
     *
     * @param   pSpriteFrame    A sprite frame which involves a texture and a rect
     * @return  A valid sprite object that is marked as autoreleased.
     */
    static CCAntiArtifactSprite* createWithSpriteFrame(CCSpriteFrame *pSpriteFrame);
    
    /**
     * Creates a sprite with an sprite frame name.
     *
     * A CCSpriteFrame will be fetched from the CCSpriteFrameCache by pszSpriteFrameName param.
     * If the CCSpriteFrame doesn't exist it will raise an exception.
     *
     * @param   pszSpriteFrameName A null terminated string which indicates the sprite frame name.
     * @return  A valid sprite object that is marked as autoreleased.
     */
    static CCAntiArtifactSprite* createWithSpriteFrameName(const char *pszSpriteFrameName);
    
    /// @}  end of creators group
	
protected:
	/// @see CCSprite::setTextureCoords
    virtual void setTextureCoords(CCRect rect);
};

NS_CC_END

#endif // __CCAntiArtifactSprite_h__