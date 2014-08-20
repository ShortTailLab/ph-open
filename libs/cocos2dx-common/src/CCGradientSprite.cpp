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
#include "CCGradientSprite.h"

NS_CC_BEGIN

CCGradientSprite::CCGradientSprite() :
        m_startColor(ccWHITE),
        m_endColor(ccWHITE),
        m_cEndOpacity(0),
        m_cStartOpacity(255),
        m_AlongVector(ccp(0, 1)),
        m_bCompressedInterpolation(true) {
}

CCGradientSprite::~CCGradientSprite() {
}

CCGradientSprite* CCGradientSprite::create() {
	CCGradientSprite *pSprite = new CCGradientSprite();
    if (pSprite && pSprite->init())
    {
        pSprite->autorelease();
        return pSprite;
    }
    CC_SAFE_DELETE(pSprite);
    return NULL;
}

CCGradientSprite* CCGradientSprite::create(const char *pszFileName) {
	CCGradientSprite *pobSprite = new CCGradientSprite();
    if (pobSprite && pobSprite->initWithFile(pszFileName))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    CC_SAFE_DELETE(pobSprite);
    return NULL;
}

CCGradientSprite* CCGradientSprite::create(const char *pszFileName, const CCRect& rect) {
	CCGradientSprite *pobSprite = new CCGradientSprite();
    if (pobSprite && pobSprite->initWithFile(pszFileName, rect))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    CC_SAFE_DELETE(pobSprite);
    return NULL;
}

CCGradientSprite* CCGradientSprite::createWithTexture(CCTexture2D *pTexture) {
	CCGradientSprite *pobSprite = new CCGradientSprite();
    if (pobSprite && pobSprite->initWithTexture(pTexture))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    CC_SAFE_DELETE(pobSprite);
    return NULL;
}

CCGradientSprite* CCGradientSprite::createWithTexture(CCTexture2D *pTexture, const CCRect& rect) {
    CCGradientSprite *pobSprite = new CCGradientSprite();
    if (pobSprite && pobSprite->initWithTexture(pTexture, rect))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    CC_SAFE_DELETE(pobSprite);
    return NULL;
}

CCGradientSprite* CCGradientSprite::createWithSpriteFrame(CCSpriteFrame *pSpriteFrame) {
	CCGradientSprite *pobSprite = new CCGradientSprite();
    if (pSpriteFrame && pobSprite && pobSprite->initWithSpriteFrame(pSpriteFrame))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    CC_SAFE_DELETE(pobSprite);
    return NULL;
}

CCGradientSprite* CCGradientSprite::createWithSpriteFrameName(const char *pszSpriteFrameName) {
	CCSpriteFrame *pFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pszSpriteFrameName);
    
#if COCOS2D_DEBUG > 0
    char msg[256] = {0};
    sprintf(msg, "Invalid spriteFrameName: %s", pszSpriteFrameName);
    CCAssert(pFrame != NULL, msg);
#endif
    
    return createWithSpriteFrame(pFrame);
}

void CCGradientSprite::setOpacity(GLubyte opacity) {
    CCNodeRGBA::setOpacity(opacity);
    
    updateColor();
}

void CCGradientSprite::setColor(const ccColor3B& color3) {
    CCNodeRGBA::setColor(color3);
    
    updateColor();
}

void CCGradientSprite::setOpacityModifyRGB(bool modify) {
    if (m_bOpacityModifyRGB != modify) {
        m_bOpacityModifyRGB = modify;
        updateColor();
    }
}

void CCGradientSprite::updateDisplayedColor(const ccColor3B& parentColor) {
    CCNodeRGBA::updateDisplayedColor(parentColor);
    
    updateColor();
}

void CCGradientSprite::updateDisplayedOpacity(GLubyte opacity) {
    CCNodeRGBA::updateDisplayedOpacity(opacity);
    
    updateColor();
}

void CCGradientSprite::setColor(const ccColor4B& start, const ccColor4B& end, const CCPoint& v) {
	CCSprite::setColor(ccc3(start.r, start.g, start.b));
    m_endColor.r = end.r;
    m_endColor.g = end.g;
    m_endColor.b = end.b;
    m_cEndOpacity = end.a;
    m_cStartOpacity = start.a;
    m_AlongVector = v;
    
    updateColor();
}

bool CCGradientSprite::getCompressedInterpolation() {
    return m_bCompressedInterpolation;
}

void CCGradientSprite::setCompressedInterpolation(bool compress) {
    m_bCompressedInterpolation = compress;
    updateColor();
}

const ccColor3B& CCGradientSprite::getStartColor() {
    return _realColor;
}

void CCGradientSprite::setStartColor(const ccColor3B& color) {
    setColor(color);
}

void CCGradientSprite::setEndColor(const ccColor3B& color) {
    m_endColor = color;
    updateColor();
}

const ccColor3B& CCGradientSprite::getEndColor() {
    return m_endColor;
}

void CCGradientSprite::setStartOpacity(GLubyte o) {
    m_cStartOpacity = o;
    updateColor();
}

GLubyte CCGradientSprite::getStartOpacity() {
    return m_cStartOpacity;
}

void CCGradientSprite::setEndOpacity(GLubyte o) {
    m_cEndOpacity = o;
    updateColor();
}

GLubyte CCGradientSprite::getEndOpacity() {
    return m_cEndOpacity;
}

void CCGradientSprite::setVector(const CCPoint& var) {
    m_AlongVector = var;
    updateColor();
}

const CCPoint& CCGradientSprite::getVector() {
    return m_AlongVector;
}

void CCGradientSprite::updateColor() {
    // update color of super
    CCSprite::updateColor();
    
    // direction length, if zero, do nothing
    float h = ccpLength(m_AlongVector);
    if (h > 0) {
        float c = sqrtf(2.0f);
        CCPoint u = ccp(m_AlongVector.x / h, m_AlongVector.y / h);
        
        // Compressed Interpolation mode
        if (m_bCompressedInterpolation) {
            float h2 = 1 / ( fabsf(u.x) + fabsf(u.y) );
            u = ccpMult(u, h2 * (float)c);
        }
        
        float opacityf = (float)_displayedOpacity / 255.0f;
        
        ccColor4F S = {
            _displayedColor.r / 255.0f,
            _displayedColor.g / 255.0f,
            _displayedColor.b / 255.0f,
            m_cStartOpacity * opacityf / 255.0f
        };
        
        ccColor4F E = {
            m_endColor.r / 255.0f,
            m_endColor.g / 255.0f,
            m_endColor.b / 255.0f,
            m_cEndOpacity * opacityf / 255.0f
        };
        
        // bl
        m_sQuad.bl.colors.r = (E.r + (S.r - E.r) * ((c + u.x + u.y) / (2.0f * c))) * 255;
        m_sQuad.bl.colors.g = (E.g + (S.g - E.g) * ((c + u.x + u.y) / (2.0f * c))) * 255;
        m_sQuad.bl.colors.b = (E.b + (S.b - E.b) * ((c + u.x + u.y) / (2.0f * c))) * 255;
        m_sQuad.bl.colors.a = (E.a + (S.a - E.a) * ((c + u.x + u.y) / (2.0f * c))) * 255;
        
        // br
        m_sQuad.br.colors.r = (E.r + (S.r - E.r) * ((c - u.x + u.y) / (2.0f * c))) * 255;
        m_sQuad.br.colors.g = (E.g + (S.g - E.g) * ((c - u.x + u.y) / (2.0f * c))) * 255;
        m_sQuad.br.colors.b = (E.b + (S.b - E.b) * ((c - u.x + u.y) / (2.0f * c))) * 255;
        m_sQuad.br.colors.a = (E.a + (S.a - E.a) * ((c - u.x + u.y) / (2.0f * c))) * 255;
        
        // tl
        m_sQuad.tl.colors.r = (E.r + (S.r - E.r) * ((c + u.x - u.y) / (2.0f * c))) * 255;
        m_sQuad.tl.colors.g = (E.g + (S.g - E.g) * ((c + u.x - u.y) / (2.0f * c))) * 255;
        m_sQuad.tl.colors.b = (E.b + (S.b - E.b) * ((c + u.x - u.y) / (2.0f * c))) * 255;
        m_sQuad.tl.colors.a = (E.a + (S.a - E.a) * ((c + u.x - u.y) / (2.0f * c))) * 255;
        
        // tr
        m_sQuad.tr.colors.r = (E.r + (S.r - E.r) * ((c - u.x - u.y) / (2.0f * c))) * 255;
        m_sQuad.tr.colors.g = (E.g + (S.g - E.g) * ((c - u.x - u.y) / (2.0f * c))) * 255;
        m_sQuad.tr.colors.b = (E.b + (S.b - E.b) * ((c - u.x - u.y) / (2.0f * c))) * 255;
        m_sQuad.tr.colors.a = (E.a + (S.a - E.a) * ((c - u.x - u.y) / (2.0f * c))) * 255;
    }
    
    // renders using batch node
    if (m_pobBatchNode) {
        if (m_uAtlasIndex != CCSpriteIndexNotInitialized) {
            m_pobTextureAtlas->updateQuad(&m_sQuad, m_uAtlasIndex);
        } else {
            // no need to set it recursively
            // update dirty_, don't update recursiveDirty_
            setDirty(true);
        }
    }
    
    // self render
    // do nothing
}

NS_CC_END