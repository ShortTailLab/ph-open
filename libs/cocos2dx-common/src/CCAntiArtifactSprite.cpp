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
#include "CCAntiArtifactSprite.h"

NS_CC_BEGIN

CCAntiArtifactSprite::CCAntiArtifactSprite() {
}

CCAntiArtifactSprite::~CCAntiArtifactSprite() {
}

CCAntiArtifactSprite* CCAntiArtifactSprite::create() {
	CCAntiArtifactSprite *pSprite = new CCAntiArtifactSprite();
    if (pSprite && pSprite->init())
    {
        pSprite->autorelease();
        return pSprite;
    }
    CC_SAFE_DELETE(pSprite);
    return NULL;
}

CCAntiArtifactSprite* CCAntiArtifactSprite::create(const char *pszFileName) {
	CCAntiArtifactSprite *pobSprite = new CCAntiArtifactSprite();
    if (pobSprite && pobSprite->initWithFile(pszFileName))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    CC_SAFE_DELETE(pobSprite);
    return NULL;
}

CCAntiArtifactSprite* CCAntiArtifactSprite::create(const char *pszFileName, const CCRect& rect) {
	CCAntiArtifactSprite *pobSprite = new CCAntiArtifactSprite();
    if (pobSprite && pobSprite->initWithFile(pszFileName, rect))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    CC_SAFE_DELETE(pobSprite);
    return NULL;
}

CCAntiArtifactSprite* CCAntiArtifactSprite::createWithTexture(CCTexture2D *pTexture) {
	CCAntiArtifactSprite *pobSprite = new CCAntiArtifactSprite();
    if (pobSprite && pobSprite->initWithTexture(pTexture))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    CC_SAFE_DELETE(pobSprite);
    return NULL;
}

CCAntiArtifactSprite* CCAntiArtifactSprite::createWithTexture(CCTexture2D *pTexture, const CCRect& rect) {
    CCAntiArtifactSprite *pobSprite = new CCAntiArtifactSprite();
    if (pobSprite && pobSprite->initWithTexture(pTexture, rect))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    CC_SAFE_DELETE(pobSprite);
    return NULL;
}

CCAntiArtifactSprite* CCAntiArtifactSprite::createWithSpriteFrame(CCSpriteFrame *pSpriteFrame) {
	CCAntiArtifactSprite *pobSprite = new CCAntiArtifactSprite();
    if (pSpriteFrame && pobSprite && pobSprite->initWithSpriteFrame(pSpriteFrame))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    CC_SAFE_DELETE(pobSprite);
    return NULL;
}

CCAntiArtifactSprite* CCAntiArtifactSprite::createWithSpriteFrameName(const char *pszSpriteFrameName) {
	CCSpriteFrame *pFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pszSpriteFrameName);
    
#if COCOS2D_DEBUG > 0
    char msg[256] = {0};
    sprintf(msg, "Invalid spriteFrameName: %s", pszSpriteFrameName);
    CCAssert(pFrame != NULL, msg);
#endif
    
    return createWithSpriteFrame(pFrame);
}

void CCAntiArtifactSprite::setTextureCoords(CCRect rect) {
    rect = CC_RECT_POINTS_TO_PIXELS(rect);
	
    CCTexture2D *tex = m_pobBatchNode ? m_pobTextureAtlas->getTexture() : m_pobTexture;
    if (! tex)
    {
        return;
    }
	
    float atlasWidth = (float)tex->getPixelsWide();
    float atlasHeight = (float)tex->getPixelsHigh();
	
    float left, right, top, bottom;
	
    if (m_bRectRotated)
    {
        left    = (2*rect.origin.x+1)/(2*atlasWidth);
        right    = left+(rect.size.height*2-2)/(2*atlasWidth);
        top        = (2*rect.origin.y+1)/(2*atlasHeight);
        bottom    = top+(rect.size.width*2-2)/(2*atlasHeight);
		
        if (m_bFlipX)
        {
            CC_SWAP(top, bottom, float);
        }
		
        if (m_bFlipY)
        {
            CC_SWAP(left, right, float);
        }
		
        m_sQuad.bl.texCoords.u = left;
        m_sQuad.bl.texCoords.v = top;
        m_sQuad.br.texCoords.u = left;
        m_sQuad.br.texCoords.v = bottom;
        m_sQuad.tl.texCoords.u = right;
        m_sQuad.tl.texCoords.v = top;
        m_sQuad.tr.texCoords.u = right;
        m_sQuad.tr.texCoords.v = bottom;
    }
    else
    {
        left    = (2*rect.origin.x+1)/(2*atlasWidth);
        right    = left + (rect.size.width*2-2)/(2*atlasWidth);
        top        = (2*rect.origin.y+1)/(2*atlasHeight);
        bottom    = top + (rect.size.height*2-2)/(2*atlasHeight);
		
        if(m_bFlipX)
        {
            CC_SWAP(left,right,float);
        }
		
        if(m_bFlipY)
        {
            CC_SWAP(top,bottom,float);
        }
		
        m_sQuad.bl.texCoords.u = left;
        m_sQuad.bl.texCoords.v = bottom;
        m_sQuad.br.texCoords.u = right;
        m_sQuad.br.texCoords.v = bottom;
        m_sQuad.tl.texCoords.u = left;
        m_sQuad.tl.texCoords.v = top;
        m_sQuad.tr.texCoords.u = right;
        m_sQuad.tr.texCoords.v = top;
    }
}

NS_CC_END