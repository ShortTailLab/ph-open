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
#ifndef __CCTiledSprite_h__
#define __CCTiledSprite_h__

#include "cocos2d.h"

NS_CC_BEGIN

/**
 * A node which can tile a texture in vertical and horizontal direction.
 * Can be used to fill game background and it is good to fill dynamic-size area.
 *
 * \note
 * Use setContentSize to specify the tile area. It will automatically tile
 * sprite to fill whole area
 */
class CC_DLL CCTiledSprite : public CCNodeRGBA, public CCBlendProtocol {
private:
	/// base sprite
	CCSprite* m_sprite;
	
	/// dirty flag
	bool m_dirty;
	
	/// atlas
	CCTextureAtlas* m_atlas;
	
private:
	/// update atlas
	void updateAtlas();
	
public:
	CCTiledSprite(CCSprite* sprite);
	virtual ~CCTiledSprite();
	
	static CCTiledSprite* create(const char* fileImage);
	static CCTiledSprite* createWithTexture(CCTexture2D* tex);
	static CCTiledSprite* createWithSpriteFrameName(const char* name);
	static CCTiledSprite* createWithSpriteFrame(CCSpriteFrame* frame);
	static CCTiledSprite* createWithSprite(CCSprite* sprite);
	
	/// @{
    /// @name Functions inherited from CCTextureProtocol
    inline void setBlendFunc(ccBlendFunc blendFunc) { m_sprite->setBlendFunc(blendFunc); }
    inline ccBlendFunc getBlendFunc(void) { return m_sprite->getBlendFunc(); }
    /// @}
	
	/// @see CCNode::draw
	virtual void draw();
	
	/// @see CCNode::setContentSize
	virtual void setContentSize(const CCSize& contentSize);
	
	/// set height
	virtual void setHeight(float h);

	/// set width
	virtual void setWidth(float w);
	
	/// get internal sprite
	CCSprite* getSprite() { return m_sprite; }
};

NS_CC_END

#endif // __CCTiledSprite_h__